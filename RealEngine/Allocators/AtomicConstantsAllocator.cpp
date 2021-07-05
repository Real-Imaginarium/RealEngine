#include "AtomicConstantsAllocator.h"

#include "DxException.h"
#include "RegionsList.hpp"


const size_t ManagedBlockC_Impl::g_minimum_size_of_constant = 256;

//  онструируетс€ "сырой" аллокатор, использовать который можно будет только после инициализации управл€емого блока одной из версий SetupManagedBlock().
AtomicConstantsAllocator::AtomicConstantsAllocator( uint8_t mode, ManagedBlockC_Impl *mb_impl )
    : GPURegionsAllocator( mode, 
                           mb_impl ? mb_impl : new ManagedBlockC_Impl( nullptr ))
    , m_casted_mb( dynamic_cast<ManagedBlockC_Impl*>( m_mb_impl.get()))
{}


//  онструктор создаЄт управл€емый блок пам€ти размером, достаточным дл€ размещени€ 'num_constants' элементарных (256 байт) констант
AtomicConstantsAllocator::AtomicConstantsAllocator( ID3D12Device *dev, size_t num_constants, uint8_t mode, ManagedBlockC_Impl *mb_impl )
    : GPURegionsAllocator( mode, 
                           mb_impl ? mb_impl : new ManagedBlockC_Impl( nullptr ))
    , m_casted_mb( dynamic_cast<ManagedBlockC_Impl*>( m_mb_impl.get()))
{
    SetupManagedBlock( dev, num_constants );
}


//  онструктор создаЄт управл€емый блок пам€ти размером 'num_constants' элементарных (256 байт) констант, запрашива€ его у родительского аллокатора 'parent_alloc'
AtomicConstantsAllocator::AtomicConstantsAllocator( ID3D12Device *dev, size_t num_constants, GPURegionsAllocator *parent_alloc, bool clean_when_dealloc, ManagedBlockC_Impl *mb_impl )
    : GPURegionsAllocator( static_cast<uint8_t>( clean_when_dealloc ? Mode::CLEAN_WHEN_DEALLOC : Mode::NO_FREE_NO_CLEANUP ),
                           mb_impl ? mb_impl : new ManagedBlockC_Impl( nullptr ))
    , m_casted_mb( dynamic_cast<ManagedBlockC_Impl*>( m_mb_impl.get()))
{
    SetupManagedBlock( dev, num_constants, parent_alloc );
}


//  онструктору передаЄтс€ управл€емый блок пам€ти на GPU с началом в 'mem_start', размером 'num_constants' элементарных (256 байт) констант, созданный заранее
AtomicConstantsAllocator::AtomicConstantsAllocator( ID3D12Device *dev, CP_ID3D12Resource buffer, uint8_t *mem_start, size_t num_constants, uint8_t mode, ManagedBlockC_Impl *mb_impl )
    : GPURegionsAllocator( mode,
                           mb_impl ? mb_impl : new ManagedBlockC_Impl( nullptr ))
    , m_casted_mb( dynamic_cast<ManagedBlockC_Impl*>( m_mb_impl.get()))
{
    SetupManagedBlock( dev, buffer, mem_start, num_constants );
}


// —оздать управл€емый блок пам€ти дл€ размещени€ 'num_constants' элементарных (256 байт) констант
void AtomicConstantsAllocator::SetupManagedBlock( ID3D12Device *dev, size_t num_constants )
{
    GPURegionsAllocator::SetupManagedBlock( dev, num_constants * ManagedBlockC_Impl::g_minimum_size_of_constant );
}


// —оздать управл€емый блок пам€ти дл€ размещени€ 'num_constants' элементарных (256 байт) констант, запрашива€ его у родительского аллокатора 'parent_alloc'
void AtomicConstantsAllocator::SetupManagedBlock( ID3D12Device *dev, size_t num_constants, GPURegionsAllocator *parent_alloc )
{
    GPURegionsAllocator::SetupManagedBlock( dev, num_constants, parent_alloc );
}


// —оздать управл€емый блок пам€ти дл€ размещени€ 'num_constants' элементарных (256 байт) констант, запрашива€ его у родительского аллокатора 'parent_alloc'
void AtomicConstantsAllocator::SetupManagedBlock( ID3D12Device *dev, CP_ID3D12Resource buffer, uint8_t *mem_start, size_t num_constants )
{
    GPURegionsAllocator::SetupManagedBlock( dev, buffer, mem_start, num_constants * ManagedBlockC_Impl::g_minimum_size_of_constant );
}


// «апрашивает пачку элементарных (256 байт) констант количеством 'size' (возвращаемый указатель предполагает приведение к константе)
uint8_t *AtomicConstantsAllocator::Allocate( size_t size )
{
    return GPURegionsAllocator::Allocate( size * ManagedBlockC_Impl::g_minimum_size_of_constant );
}


// –елизит пачку констант минимального размера (256 байт) с началом в 'start' и количеством 'size' констант
Error_BasePtr AtomicConstantsAllocator::Deallocate( void *start, size_t size )
{
    return GPURegionsAllocator::Deallocate( start, size * ManagedBlockC_Impl::g_minimum_size_of_constant );
}

//  -тор блока
ManagedBlockC_Impl::ManagedBlockC_Impl( ID3D12Device *dev ) : ManagedBlockRWS_Impl( dev )
{}


// –азмер €чейки пам€ти, байт, которыми оперирует аллокатор
size_t ManagedBlockC_Impl::CellSizeImpl()
{
    return g_minimum_size_of_constant;
}