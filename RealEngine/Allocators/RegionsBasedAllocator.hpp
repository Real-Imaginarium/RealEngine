#include "IRegionsBasedAllocator.h"

#include "Error_Allocator.h"
#include "RegionsList.hpp"
#include "LogError.h"

#include <memory>


class RegionsBasedAllocator_Tester;

template<class T, bool cleanup = false>
class RegionsBasedAllocator : public IRegionsBasedAllocator<T>
{
public:
    friend class RegionsBasedAllocator_Tester;

    RegionsBasedAllocator( size_t mem_size );

    RegionsBasedAllocator( T *mem_start, size_t mem_size, bool free_the_memory = true );

    ~RegionsBasedAllocator() override;

    T *Allocate( size_t size ) override;

    Error_BasePtr Deallocate( void *start, size_t size ) override;

private:
    inline void InitializeManagedBlock( T *start, size_t size );
    std::unique_ptr<RegionsList<T>> m_reg_list;
    RegionP<T> m_managed_block;
    bool m_free_the_memory;
};


template<class T, bool cleanup>
RegionsBasedAllocator<T, cleanup>::RegionsBasedAllocator( size_t mem_size )
    : m_free_the_memory( true )
{
    T *mem_start = nullptr;
    auto err = utils::Attempt_calloc( 20, 100, mem_size, mem_start );            TRACE_ALLOCATOR_THR_ERR( err, EA_Type::MANAGED_BLOCK_CREATION_ERROR, mem_size );
    InitializeManagedBlock( mem_start, mem_size );
}


template<class T, bool cleanup>
RegionsBasedAllocator<T, cleanup>::RegionsBasedAllocator( T *mem_start, size_t mem_size, bool free_the_memory )
    : m_free_the_memory( free_the_memory )
{
    InitializeManagedBlock( mem_start, mem_size );
}


template<class T, bool cleanup>
RegionsBasedAllocator<T, cleanup>::~RegionsBasedAllocator()
{
    if( m_free_the_memory ) {
        if( m_managed_block.start ) {
            free( m_managed_block.start );
        }
    }
}


template<class T, bool cleanup>
void RegionsBasedAllocator<T, cleanup>::InitializeManagedBlock( T *start, size_t size )
{
    m_managed_block.start = start;
    m_managed_block.size = size;

    try {
        m_reg_list = std::make_unique<RegionsList<T>>( 3, m_managed_block );
    }
    catch( Error_BasePtr e )
    {
        TRACE_ALLOCATOR_THR_ERR( e, EA_Type::REGIONS_LIST_CREATION_ERROR, m_managed_block.start, m_managed_block.size );
    }
}


template<class T, bool cleanup>
T *RegionsBasedAllocator<T, cleanup>::Allocate( size_t size )
{
    T *p = nullptr;
    auto err = m_reg_list->GrabRegion( size, &p );

    if( err )
    {
        auto err_rl = ErrorCast<Error_RegionsList>( err );

        if( err_rl && ( err_rl->Elem() == ERL_Type::GRAB_FROM_EMPTY_LIST || err_rl->Elem() == ERL_Type::CONSISTENT_REG_NOTFOUND ))
        {
            err = ERR_ALLOCATOR( EA_Type::ALLOC_SIZE_TOO_BIG, size );
            throw err;
        }
        else {
            TRACE_CUSTOM_THR_ERR( err, "Can't allocate the memory block with size: " + std::to_string( size ) + " by some reasons (see trace)");
        }
    }
    return p;
}


template<class T, bool cleanup>
Error_BasePtr RegionsBasedAllocator<T, cleanup>::Deallocate( void *start, size_t size )
{
    // Проверяем, что деаллоцируемый регион находится в пределах управляемого блока
    if( start < m_managed_block.start || (T*)start + size > m_managed_block.start + m_managed_block.size )
    {
        return ERR_ALLOCATOR( EA_Type::DEALLOC_REGION_OUT_OF_BOUNDS, start, size, m_managed_block.start, m_managed_block.size );
    }
    if constexpr( cleanup ) {
        memset( start, 0, sizeof( T ) * size );
    }
    return m_reg_list->ReleaseRegion( { (T*)start, size } );
}