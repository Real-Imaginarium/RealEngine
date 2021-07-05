#pragma once

#include "IRegionsAllocator.h"
#include "Error_Custom.h"
#include "Error_Param.h"
//#include "Types.h"
#include "Utilites.hpp"

#include <cassert>

class Descriptor;
class DescriptorsAllocator;
class RegionsAllocator;

template<class T, class Alloc = RegionsAllocator>
class Vector
{
public:
    class Iterator
    {
    public:
        using iterator_category = std::random_access_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = T *;
        using reference = T &;

        Iterator() : m_ptr( nullptr ) {}
        Iterator( T *ptr ) : m_ptr( ptr ) {}
        Iterator( const Iterator &rhs ) : m_ptr( rhs.m_ptr ) {}

        inline Iterator &operator=( T *rhs ) { m_ptr = rhs; return *this; }                                 // Maybe incorrect
        inline Iterator &operator=( const Iterator &rhs ) { m_ptr = rhs.m_ptr; return *this; }              // Maybe incorrect
        inline Iterator &operator+=( difference_type rhs ) { m_ptr += rhs; return *this; }
        inline Iterator &operator-=( difference_type rhs ) { m_ptr -= rhs; return *this; }
        inline T &operator*() const { return *m_ptr; }
        inline T *operator->() const { return m_ptr; }
        inline T &operator[]( difference_type rhs ) const { return m_ptr[rhs]; }
        inline Iterator &operator++() { ++m_ptr; return *this; }
        inline Iterator &operator--() { --m_ptr; return *this; }
        inline Iterator operator++( int ) const { Iterator tmp( *this ); ++m_ptr; return tmp; }
        inline Iterator operator--( int ) const { Iterator tmp( *this ); --m_ptr; return tmp; }
        inline Iterator operator+( const Iterator &rhs ) { return Iterator( m_ptr + rhs.m_ptr ); }          // Maybe incorrect
        inline difference_type operator-( const Iterator &rhs ) const { return m_ptr - rhs.m_ptr; }
        inline Iterator operator+( difference_type rhs ) const { return Iterator( m_ptr + rhs ); }
        inline Iterator operator-( difference_type rhs ) const { return Iterator( m_ptr - rhs ); }
        friend inline Iterator operator+( difference_type lhs, const Iterator &rhs ) { return Iterator( lhs + rhs.m_ptr ); }
        friend inline Iterator operator-( difference_type lhs, const Iterator &rhs ) { return Iterator( lhs - rhs.m_ptr ); }
        inline bool operator==( const Iterator &rhs ) const { return m_ptr == rhs.m_ptr; }
        inline bool operator!=( const Iterator &rhs ) const { return m_ptr != rhs.m_ptr; }
        inline bool operator>( const Iterator &rhs ) const { return m_ptr > rhs.m_ptr; }
        inline bool operator<( const Iterator &rhs ) const { return m_ptr < rhs.m_ptr; }
        inline bool operator>=( const Iterator &rhs ) const { return m_ptr >= rhs.m_ptr; }
        inline bool operator<=( const Iterator &rhs ) const { return m_ptr <= rhs.m_ptr; }

    private:
        T *m_ptr;
    };

    // Дефолтный к-тор создаёт "сырой" вектор, который перед использованием нужно инициализировать с помощью Initialize(...)
    Vector() = default;

    Vector( size_t cap, Alloc *alloc );

    virtual ~Vector();

    void Initialize( size_t cap, Alloc *alloc );

    void Deinitialize();

    void Clean();

    Iterator Begin() { return Iterator( m_begin ); }

    Iterator End() { return Iterator( m_begin + m_range_size ); }

    size_t Size() const { return m_range_size; }

    bool Empty() const { return m_range_size == 0; }

    size_t Capacity() const { return m_capacity; }

    template<class... Args>
    std::tuple<Error_BasePtr, int64_t> EmplaceBack( Args &&... args );

    // TODO: Emplace, Insert, InsertBack, Erase, EraseBack

    inline T& operator[]( size_t pos );

    inline const T &operator[]( size_t pos ) const;

protected:
    inline Error_BasePtr Reallocate();

    inline void DestructAllElements();

    Alloc  *m_allocator = nullptr;
    T      *m_begin = nullptr;
    T      *m_end = nullptr;
    size_t m_capacity = 0;
    size_t m_range_size = 0;
    size_t m_size_factor = 1;
    bool   m_ready_to_use = false;
};



template<class T, class Alloc>
Vector<T, Alloc>::Vector( size_t cap, Alloc *alloc )
{
    Initialize( cap, alloc );
}


template<class T, class Alloc>
Vector<T, Alloc>::~Vector()
{
    Deinitialize();
}


template<class T, class Alloc>
void Vector<T, Alloc>::Initialize( size_t cap, Alloc *alloc )
{
    if( m_ready_to_use ) {
        return;
    }

    static_assert( std::is_base_of<IRegionsAllocator, Alloc>::value, "Allocator must derive from \'IRegionsAllocator\'" );
    static_assert( !( std::is_same_v<T, Descriptor> && !std::is_base_of<DescriptorsAllocator, Alloc>::value ), "\'Descriptor\' can be allocated only by \'DescriptorsAllocator\'" );

    assert( ( sizeof(T) >= alloc->CellSize()) && "Vector's element size must be >= than allocator's cell size" );
    assert( ( sizeof(T) % alloc->CellSize() == 0 ) && "Vector's element size must be multiple of allocator's cell size" );

    m_allocator = alloc;
    m_capacity = cap ? cap : 1;
    m_range_size = 0;
    m_size_factor = sizeof( T ) / m_allocator->CellSize();

    try {
        m_begin = (T*)m_allocator->Allocate( m_capacity * m_size_factor );
        m_end = m_begin;
        m_ready_to_use = true;
    }
    catch( Error_BasePtr err )
    {
        TRACE_CUSTOM_THR_ERR( err, "Can't allocate the block with size: " + std::to_string( m_capacity ) + "cells" );
    }
}


template<class T, class Alloc>
void Vector<T, Alloc>::Deinitialize()
{
    if( !m_ready_to_use ) {
        return;
    }
    DestructAllElements();
    auto err = m_allocator->Deallocate( m_begin, m_capacity * m_size_factor );      TRACE_CUSTOM_PRNT( err, "Can't deallocate the region during Vector destruction: " + utils::to_string( RegionP<T>{m_begin, m_capacity} ) );
    m_ready_to_use = false;
}



template<class T, class Alloc>
void Vector<T, Alloc>::Clean()
{
    DestructAllElements();
    m_end = m_begin;
    m_range_size = 0;
}


template<class T, class Alloc>
template<class... Args>
std::tuple<Error_BasePtr, int64_t> Vector<T, Alloc>::EmplaceBack( Args &&... args )
{
    if( m_range_size == m_capacity ) {
        auto err = Reallocate();                                                    TRACE_CUSTOM_RET_VAL( err, std::tuple( err, -1 ), "Fatal: Vector is broken because of Reallocation() failed." );
    }
    ::new( (void *)m_end ) T( std::forward<Args>( args )... );
    m_end++;
    m_range_size++;
    m_capacity;
    return std::tuple( nullptr, m_range_size - 1 );
}


template<class T, class Alloc>
T& Vector<T, Alloc>::operator[]( size_t pos )
{
    return m_begin[pos];
}


template<class T, class Alloc>
const T& Vector<T, Alloc>::operator[]( size_t pos ) const
{
    return m_begin[pos];
}


template<class T, class Alloc>
Error_BasePtr Vector<T, Alloc>::Reallocate()
{
    T *new_range = nullptr;
    try {
        m_capacity *= 2;
        new_range = (T*)m_allocator->Allocate( m_capacity * m_size_factor );
    }
    catch( Error_BasePtr err ) {
        TRACE_CUSTOM_THR_ERR( err, "Can't allocate the block with size: " + std::to_string( m_capacity ) );
    }
    for( size_t i = 0; i < m_range_size; ++i )
    {
        ::new( (void *)( new_range + i ) ) T( std::move( m_begin[i]) );
        m_begin[i].~T();
    }
    auto err = m_allocator->Deallocate( m_begin, m_range_size * m_size_factor );
    
    if(err)
    {
        throw "Can't deallocate the old m_range during Vector reallocation: " + utils::to_string( RegionP<T>{m_begin, m_capacity / 2} );
    }
    m_begin = new_range;
    m_end = m_begin + m_range_size;
    return nullptr;
}


template<class T, class Alloc>
void Vector<T, Alloc>::DestructAllElements()
{
    for( size_t i = 0; i < m_range_size; ++i )
    {
        ( m_begin + i )->~T();
    }
}
