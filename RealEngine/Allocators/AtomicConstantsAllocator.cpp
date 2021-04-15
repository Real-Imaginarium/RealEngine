#include "AtomicConstantsAllocator.h"

#include "DxException.h"
#include "RegionsList.hpp"


const size_t ManagedBlockC_Impl::g_minimum_size_of_constant = 256;

//  онструируетс€ "сырой" аллокатор, использовать который можно будет только после инициализации управл€емого блока одной из версий SetupManagedBlock().
AtomicConstantsAllocator::AtomicConstantsAllocator( uint8_t mode, ManagedBlockC_Impl *mb_impl )
    : RegionsAllocator( mode, mb_impl ? mb_impl : new ManagedBlockC_Impl( nullptr ))
    , m_casted_mb( dynamic_cast<ManagedBlockC_Impl*>( m_mb_impl.get()))
{}


//  онструктор создаЄт управл€емый блок пам€ти размером, достаточным дл€ размещени€ 'num_constants' элементарных (256 байт) констант
AtomicConstantsAllocator::AtomicConstantsAllocator( ID3D12Device *dev, size_t num_constants, uint8_t mode, ManagedBlockC_Impl *mb_impl )
    : RegionsAllocator( mode, mb_impl ? mb_impl : new ManagedBlockC_Impl( nullptr ) )
    , m_casted_mb( dynamic_cast<ManagedBlockC_Impl *>( m_mb_impl.get() ) )
{
    SetupManagedBlock( dev, num_constants * ManagedBlockC_Impl::g_minimum_size_of_constant );
}


// —оздать управл€емый блок пам€ти дл€ размещени€ 'num_constants' элементарных (256 байт) констант
void AtomicConstantsAllocator::SetupManagedBlock( ID3D12Device *dev, size_t num_constants )
{
    m_casted_mb->SetDevice( dev );
    IAllocatorCore::SetupManagedBlock( nullptr, num_constants * ManagedBlockC_Impl::g_minimum_size_of_constant );
}


// «апрашивает пачку элементарных (256 байт) констант количеством 'size' (возвращаемый указатель предполагает приведение к константе)
uint8_t *AtomicConstantsAllocator::Allocate( size_t size )
{
    return RegionsAllocator::Allocate( size * ManagedBlockC_Impl::g_minimum_size_of_constant );
}


// –елизит пачку констант минимального размера (256 байт) с началом в 'start' и количеством 'size' констант
Error_BasePtr AtomicConstantsAllocator::Deallocate( void *start, size_t size )
{
    return RegionsAllocator::Deallocate( start, size * ManagedBlockC_Impl::g_minimum_size_of_constant );
}


// ¬ычисл€ет виртуальный адрес ресурса в GPU дл€ отображЄнного на этот ресурс указател€ в CPU
D3D12_GPU_VIRTUAL_ADDRESS AtomicConstantsAllocator::CalculateVirtualAddress( void *ptr )
{
    if( !ptr || ptr > ManagedBlockStart() || ptr < ManagedBlockStart() )
    {
        return 0;
    }

    // ¬ычисл€ем оффсет указател€ относительно начала управл€емого блока, затем прибавл€ем его к виртуальному адресу на начало ресурса в GPU
    return m_casted_mb->m_constants_buffer->GetGPUVirtualAddress() + ( ManagedBlockStart() - (uint8_t*)ptr );
}


//  -тор блока
ManagedBlockC_Impl::ManagedBlockC_Impl( ID3D12Device *dev )
    : m_device( dev )
    , m_constants_buffer( nullptr )
{}


// »нициализаци€ блока
void ManagedBlockC_Impl::SetupManagedBlockImpl( uint8_t *mem_start, size_t mem_size )
{
    mem_start = nullptr;    // ќбнул€ем значение указател€, т.к. в данной реализации оно не играет роли, указатель определ€етс€ через ID3D12Resource::map() (см. далее)

    // ≈сли m_device в конструкторе был установлен в nullptr - нужно сначала вызвать SetDevice() 
    if( !m_device )
    {
        return;
    }

    // —оздаЄм буфер на GPU размером 'mem_size' дл€ хранени€ нужного количества констант (определ€етс€ к-тором аллокатора)
    THROW_ON_FAIL( m_device->CreateCommittedResource( &CD3DX12_HEAP_PROPERTIES( D3D12_HEAP_TYPE_UPLOAD ),
                                                      D3D12_HEAP_FLAG_NONE,
                                                      &CD3DX12_RESOURCE_DESC::Buffer( mem_size ),
                                                      D3D12_RESOURCE_STATE_GENERIC_READ,
                                                      nullptr,
                                                      IID_PPV_ARGS( &m_constants_buffer )));

    // ћапимс€ на созданный ресурс и завершаем инициализацию блока
    THROW_ON_FAIL( m_constants_buffer->Map( 0, nullptr, reinterpret_cast<void**>( &mem_start )));

    ManagedBlockRB_Impl::SetupManagedBlockImpl( mem_start, mem_size );
}


// —брос блока
void ManagedBlockC_Impl::ResetManagedBlockImpl()
{
    m_constants_buffer->Unmap( 0, nullptr );
}


// –азмер €чейки пам€ти, байт, которыми оперирует аллокатор
size_t ManagedBlockC_Impl::CellSizeImpl()
{
    return g_minimum_size_of_constant;
}


// ”становка m_device, если в к-тор был передан nullptr
void ManagedBlockC_Impl::SetDevice( ID3D12Device *dev )
{
    if( !m_device )
    {
        m_device = dev;
    }
}