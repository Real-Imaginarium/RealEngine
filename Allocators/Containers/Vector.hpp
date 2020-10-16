#include "IAllocator.h"
#include "Error_Custom.h"
#include "Error_Param.h"
#include "Types.h"
#include "Utilites.hpp"


template<class Type>
class Vector
{
public:
    Vector( size_t cap, std::shared_ptr<IAllocator<Type>> alloc );
    ~Vector();

    template<class... Args>
    std::tuple<Error_BasePtr, int64_t> EmplaceBack( Args &&... args );

private:
    Error_BasePtr Reallocate();

    Type *range;
    Type *begin;
    Type *end;
    size_t range_size;
    size_t capacity;
    std::shared_ptr<IAllocator<Type>> allocator;
};


template<class Type>
Error_BasePtr Vector<Type>::Reallocate()
{
    capacity *= 2;
    Type *new_range = nullptr;
    try {
        new_range = allocator->Allocate( capacity );
    }
    catch( Error_BasePtr err ) {
        TRACE_CUSTOM_THR_ERR( err, "Can't allocate the block with size: " + std::to_string( capacity ) );
    }
    for( size_t i = 0; i < range_size; ++i )
    {
        ::new( (void *)( new_range + i ) ) Type( range[i] );
        range[i].~Type();
    }
    auto err = allocator->Deallocate( range, capacity / 2 );                TRACE_CUSTOM_RET_ERR( err, "Can't deallocate the old range during Vector reallocation: " + utils::to_string( RegionP<Type>{range, capacity / 2} ) );
    range = new_range;
    begin = range;
    end = begin + range_size;
    return nullptr;
}


template<class Type>
Vector<Type>::Vector( size_t cap, std::shared_ptr<IAllocator<Type>> alloc )
{
    if( !cap ) {
        Error_BasePtr err = ERR_PARAM( 1, "size_t cap", "> 0", "0" );       TRACE_CUSTOM_THR_ERR( err, "Can't construct Vector" );
    }
    allocator = alloc;
    capacity = cap;
    range_size = 0;
    try {
        range = allocator->Allocate( capacity );
    }
    catch( Error_BasePtr err ) {
        TRACE_CUSTOM_THR_ERR( err, "Can't allocate the block with size: " + std::to_string( cap ) );
    }
    end = begin = range;
}


template<class Type>
Vector<Type>::~Vector()
{
    for( size_t i = 0; i < range_size; ++i )
    {
        ( begin + i )->~Type();
    }
    auto err = allocator->Deallocate( range, capacity );                    TRACE_CUSTOM_PRNT( err, "Can't deallocate the region during Vector destruction: " + utils::to_string( RegionP<Type>{range, capacity} ) );
}


template<class Type>
template<class... Args>
std::tuple<Error_BasePtr, int64_t> Vector<Type>::EmplaceBack( Args &&... args )
{
    if( range_size == capacity ) {
        auto err = Reallocate();                                            TRACE_CUSTOM_RET_VAL( err, std::tuple( err, -1 ), "Fatal: Vector is broken because of Reallocation() failed." );
    }
    ::new( (void *)end ) Type( std::forward<Args>( args )... );
    end++;
    range_size++;
    capacity;
    return std::tuple( nullptr, range_size - 1 );
}