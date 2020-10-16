#include "IAllocator.h"

#include "RegionsList.hpp"
#include "LogError.h"

#include <memory>


template<class T>
class RegionsBasedAllocator : public IAllocator<T>
{
public:
    RegionsBasedAllocator( T *mem_start, size_t mem_size );

    T *Allocate( size_t size ) override;

    Error_BasePtr Deallocate( void *start, size_t size ) override;

private:
    std::unique_ptr<RegionsList<T>> m_reg_list;
    RegionP<T> managed_block;
};


template<class T>
RegionsBasedAllocator<T>::RegionsBasedAllocator( T *mem_start, size_t mem_size )
{
    managed_block = RegionP<T>{ mem_start, mem_size };
    try {
        m_reg_list = std::make_unique<RegionsList<T>>( 3, managed_block );
    }
    catch( Error_BasePtr e )
    {
        TRACE_CUSTOM_THR_ERR( e, "Can't create RegionsList for RegionsBasedAllocator. Managed block is: " + utils::to_string( managed_block ) );
    }
}


template<class T>
T *RegionsBasedAllocator<T>::Allocate( size_t size )
{
    T *p = nullptr;
    auto err = m_reg_list->GrabRegion( size, &p );                                                  TRACE_CUSTOM_THR_ERR( err, "Can't grab the region of such size: " + std::to_string( size ) );
    return p;
}


template<class T>
Error_BasePtr RegionsBasedAllocator<T>::Deallocate( void *start, size_t size )
{
    // Проверяем, что деаллоцируемый регион находится в пределах управляемого блока
    if( start < managed_block.start || (T*)start + size > managed_block.start + managed_block.size )
    {
        return ERR_CUSTOM( "Deallocated region is out of bounds\nManaged block: " + utils::to_string( managed_block ) + "\nDeallocated region: " + utils::to_string( RegionP<T>{(T*)start, size} ) );
    }
    return m_reg_list->ReleaseRegion( { (T*)start, size } );
}