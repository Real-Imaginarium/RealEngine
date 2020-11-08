#include "IRegionsBasedAllocator.h"
#include "Error_Custom.h"
#include "Error_Param.h"
#include "Types.h"
#include "Utilites.hpp"


template<class Elem, class MemCell = Elem>
class Vector
{
public:
    class Iterator
    {
    public:
        using iterator_category = std::random_access_iterator_tag;
        using value_type = Elem;
        using difference_type = std::ptrdiff_t;
        using pointer = Elem*;
        using reference = Elem&;
        Iterator() : m_ptr( nullptr ) {}
        Iterator( Elem *ptr ) : m_ptr( ptr ) {}
        Iterator( const Iterator &rhs ) : m_ptr( rhs.m_ptr ) {}
        inline Iterator &operator=( Elem *rhs ) { m_ptr = rhs; return *this; }                          // Maybe incorrect
        inline Iterator &operator=( const Iterator &rhs ) { m_ptr = rhs.m_ptr; return *this; }          // Maybe incorrect
        inline Iterator &operator+=( difference_type rhs ) { m_ptr += rhs; return *this; }
        inline Iterator &operator-=( difference_type rhs ) { m_ptr -= rhs; return *this; }
        inline Elem &operator*() const { return *m_ptr; }
        inline Elem *operator->() const { return m_ptr; }
        inline Elem &operator[]( difference_type rhs ) const { return m_ptr[rhs]; }
        inline Iterator &operator++() { ++m_ptr; return *this; }
        inline Iterator &operator--() { --m_ptr; return *this; }
        inline Iterator operator++( int ) const { Iterator tmp( *this ); ++m_ptr; return tmp; }
        inline Iterator operator--( int ) const { Iterator tmp( *this ); --m_ptr; return tmp; }
        inline Iterator operator+( const Iterator &rhs ) { return Iterator( m_ptr + rhs.m_ptr ); }      // Maybe incorrect
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
        Elem *m_ptr;
    };

    Vector( size_t cap, std::shared_ptr<IRegionsBasedAllocator<MemCell>> alloc );

    ~Vector();

    Iterator Begin() { return Iterator( m_begin ); }

    Iterator End() { return Iterator( m_begin + m_range_size ); }

    size_t Size() const { return m_range_size; }

    bool Empty() const { return m_range_size == 0; }

    size_t Capacity() const { return m_capacity; }

    Error_BasePtr ShrinkToFit();

    template<class... Args>
    std::tuple<Error_BasePtr, int64_t> EmplaceBack( Args &&... args );



private:
    Error_BasePtr Reallocate();
    Elem *m_range;
    Elem *m_begin;
    Elem *m_end;
    size_t m_range_size;
    size_t m_capacity;
    size_t m_factor;        // Во сколько раз элемент вектора больше ячейки аллокатора
    std::shared_ptr<IRegionsBasedAllocator<MemCell>> allocator;
};


template<class Elem, class MemCell>
Vector<Elem, MemCell>::Vector( size_t cap, std::shared_ptr<IRegionsBasedAllocator<MemCell>> alloc )
{
    static_assert( sizeof( Elem ) >= sizeof( MemCell ), "Size of Vector's element < size of Allocator's cells!" );
    static_assert( !( sizeof( Elem ) % sizeof( MemCell ) ), "Size of Vector's element isn't multiple of Allocator's cell size!" );

    if( !cap ) {
        Error_BasePtr err = ERR_PARAM( 1, "size_t cap", "> 0", "0" );       TRACE_CUSTOM_THR_ERR( err, "Can't construct Vector" );
    }
    allocator = alloc;
    m_capacity = cap;
    m_range_size = 0;
    m_factor = sizeof( Elem ) / sizeof( MemCell );

    try {
        m_range = (Elem*)allocator->Allocate( m_capacity * m_factor );
    }
    catch( Error_BasePtr err ) {
        TRACE_CUSTOM_THR_ERR( err, "Can't allocate the block with size: " + std::to_string( m_capacity * m_factor ) + "cells");
    }
    m_end = m_begin = m_range;
}


template<class Elem, class MemCell>
Vector<Elem, MemCell>::~Vector()
{
    for( size_t i = 0; i < m_range_size; ++i )
    {
        ( m_begin + i )->~Elem();
    }
    auto err = allocator->Deallocate( m_range, m_capacity * m_factor );                     TRACE_CUSTOM_PRNT( err, "Can't deallocate the region during Vector destruction: " + utils::to_string( RegionP<Elem>{m_range, m_capacity} ) );
}


template<class Elem, class MemCell>
Error_BasePtr Vector<Elem, MemCell>::ShrinkToFit()
{
    if( m_capacity == m_range_size ) {
        return nullptr;
    }
    auto err = allocator->Deallocate( m_end, ( m_capacity - m_range_size ) * m_factor );    TRACE_CUSTOM_PRNT( err, "Can't deallocate the region during ShrinkToFit: " + utils::to_string( RegionP<Elem>{m_end, ( m_capacity - m_range_size ) * m_factor} ) );
    m_capacity = m_range_size;
    return nullptr;
}


template<class Elem, class MemCell>
template<class... Args>
std::tuple<Error_BasePtr, int64_t> Vector<Elem, MemCell>::EmplaceBack( Args &&... args )
{
    if( m_range_size == m_capacity ) {
        auto err = Reallocate();                                            TRACE_CUSTOM_RET_VAL( err, std::tuple( err, -1 ), "Fatal: Vector is broken because of Reallocation() failed." );
    }
    ::new( (void *)m_end ) Elem( std::forward<Args>( args )... );
    m_end++;
    m_range_size++;
    m_capacity;
    return std::tuple( nullptr, m_range_size - 1 );
}


template<class Elem, class MemCell>
Error_BasePtr Vector<Elem, MemCell>::Reallocate()
{
    Elem *new_range = nullptr;
    try {
        m_capacity *= 2;
        new_range = (Elem*)allocator->Allocate( m_capacity * m_factor );
    }
    catch( Error_BasePtr err ) {
        TRACE_CUSTOM_THR_ERR( err, "Can't allocate the block with size: " + std::to_string( m_capacity * m_factor ) );
    }
    for( size_t i = 0; i < m_range_size; ++i )
    {
        ::new( (void *)( new_range + i ) ) Elem( m_range[i] );
        m_range[i].~Elem();
    }
    auto err = allocator->Deallocate( m_range, (m_capacity / 2) * m_factor );           TRACE_CUSTOM_RET_ERR( err, "Can't deallocate the old m_range during Vector reallocation: " + utils::to_string( RegionP<Elem>{m_range, (m_capacity/2)*m_factor} ));
    m_range = new_range;
    m_begin = m_range;
    m_end = m_begin + m_range_size;
    return nullptr;
}
