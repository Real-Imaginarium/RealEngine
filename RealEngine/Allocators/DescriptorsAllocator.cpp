#include "DescriptorsAllocator.h"

#include "RegionsList.hpp"


//  онструируетс€ "сырой" аллокатор, использовать который можно будет только после инициализации управл€емого блока одной из версий SetupManagedBlock().
DescriptorsAllocator::DescriptorsAllocator( DescriptorType d_type, uint8_t mode, ManagedBlockD_Impl *mb_impl )
    : RegionsAllocator( mode, mb_impl ? mb_impl : new ManagedBlockD_Impl( nullptr, d_type ))
    , m_casted_mb( dynamic_cast<ManagedBlockD_Impl*>( m_mb_impl.get()))
{}

//  онструктор создаЄт управл€емый блок пам€ти размером, достаточным дл€ размещени€ 'num_descriptors' дескрипторов.
DescriptorsAllocator::DescriptorsAllocator( ID3D12Device *dev, DescriptorType d_type, size_t num_descriptors, uint8_t mode, ManagedBlockD_Impl *mb_impl )
    : RegionsAllocator( num_descriptors * sizeof(Descriptor), mode, mb_impl ? mb_impl : new ManagedBlockD_Impl( dev, d_type ))
    , m_casted_mb( dynamic_cast<ManagedBlockD_Impl*>( m_mb_impl.get()))
{}

//  онструктор создаЄт управл€емый блок пам€ти размером, достаточным дл€ размещени€ 'num_descriptors' дескрипторов, запрашива€ его у внешнего аллокатора 'parent_alloc'
DescriptorsAllocator::DescriptorsAllocator( ID3D12Device *dev, DescriptorType d_type, size_t num_descriptors, RegionsAllocator *parent_alloc, bool clean_when_dealloc, ManagedBlockD_Impl *mb_impl )
    : RegionsAllocator( num_descriptors, parent_alloc, clean_when_dealloc, mb_impl ? mb_impl : new ManagedBlockD_Impl( dev, d_type))
    , m_casted_mb( dynamic_cast<ManagedBlockD_Impl*>( m_mb_impl.get()))
{}

//  онструктору передаЄтс€ управл€емый блок пам€ти с началом в 'mem_start' и размером, достаточным дл€ размещени€ 'num_descriptors' дескрипторов, созданный заранее
DescriptorsAllocator::DescriptorsAllocator( ID3D12Device *dev, DescriptorType d_type, uint8_t *mem_start, size_t num_descriptors, uint8_t mode, ManagedBlockD_Impl *mb_impl )
    : RegionsAllocator( mem_start, num_descriptors * sizeof(Descriptor), mode, mb_impl ? mb_impl : new ManagedBlockD_Impl( dev, d_type ))
    , m_casted_mb( dynamic_cast<ManagedBlockD_Impl*>( m_mb_impl.get()))
{}


// —оздать управл€емый блок пам€ти дл€ размещени€ 'num_descriptors' дескрипторов
void DescriptorsAllocator::SetupManagedBlock( ID3D12Device *dev, size_t num_descriptors )
{
    m_casted_mb->SetDevice( dev );
    SetupManagedBlock( num_descriptors * sizeof(Descriptor));
}


// «апросить у внешнего аллокатора 'parent_alloc' блок пам€ти дл€ размещени€ 'num_descriptors' дескрипторов
void DescriptorsAllocator::SetupManagedBlock( ID3D12Device *dev, size_t num_descriptors, RegionsAllocator *parent_alloc )
{
    m_casted_mb->SetDevice( dev );
    SetupManagedBlock( num_descriptors, parent_alloc );
}


// ”становить созданный заранее управл€емый блок пам€ти с началом в 'mem_start', размером 'num_descriptors' дескрипторов
void DescriptorsAllocator::SetupManagedBlock( ID3D12Device *dev, uint8_t *mem_start, size_t num_descriptors )
{
    m_casted_mb->SetDevice( dev );
    SetupManagedBlock( mem_start, num_descriptors * sizeof(Descriptor));
}


// «апрашивает пачку дескрипторов количеством 'size' (возвращаемый указатель предполагает приведение к Descriptor*)
uint8_t* DescriptorsAllocator::Allocate( size_t size )
{
    return RegionsAllocator::Allocate( size * sizeof(Descriptor));
}


// –елизит пачку дескрипторов с началом в 'start' и количеством 'size' дескрипторов
Error_BasePtr DescriptorsAllocator::Deallocate( void *start, size_t size )
{
    return RegionsAllocator::Deallocate( start, size * sizeof(Descriptor));
}


//  -тор блока
ManagedBlockD_Impl::ManagedBlockD_Impl( ID3D12Device *dev, DescriptorType m_descr_type )
    : m_device( dev )
    , m_descr_heap( nullptr )
    , m_descr_type( m_descr_type )
    , m_descr_size( 0 )
{}


// »нициализаци€ блока
void ManagedBlockD_Impl::SetupManagedBlockImpl( uint8_t *mem_start, size_t mem_size )
{
    // ≈сли m_device в конструкторе был установлен в nullptr - нужно сначала вызвать SetDevice() 
    if( !m_device )
    {
        return;
    }

    ManagedBlockRB_Impl::SetupManagedBlockImpl( mem_start, mem_size );

    D3D12_DESCRIPTOR_HEAP_DESC heap_desc;

    switch( m_descr_type )
    {
        case DescriptorType::CBV:
        case DescriptorType::SRV:
        case DescriptorType::UAV: heap_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV; break;
        case DescriptorType::DSV: heap_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV; break;
        case DescriptorType::RTV: heap_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV; break;
        default: heap_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES; break;                      // Will fail the heap creation
    }

    heap_desc.NumDescriptors = static_cast<UINT>( mem_size / sizeof(Descriptor));
    heap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    heap_desc.NodeMask = 0;

    if( FAILED( m_device->CreateDescriptorHeap( &heap_desc, IID_PPV_ARGS( m_descr_heap.GetAddressOf() ) ) ) )
    {
        throw "Descriptors heap creation failed";
    }

    // ѕробегаемс€ по m_managed_block и заполн€ем его "сырыми" дескрипторами (в них только cpu/gpu хандлеры)
    m_descr_size = m_device->GetDescriptorHandleIncrementSize( heap_desc.Type );

    Descriptor *descr = reinterpret_cast<Descriptor*>( m_start );

    for( size_t i = 0; i < heap_desc.NumDescriptors; ++i )
    {
        auto cpu_handle = m_descr_heap->GetCPUDescriptorHandleForHeapStart();
        auto gpu_handle = m_descr_heap->GetGPUDescriptorHandleForHeapStart();
        descr[i].m_cpu_handle = CD3DX12_CPU_DESCRIPTOR_HANDLE( cpu_handle, static_cast<INT>( i ), m_descr_size );
        descr[i].m_gpu_handle = CD3DX12_GPU_DESCRIPTOR_HANDLE( gpu_handle, static_cast<INT>( i ), m_descr_size );
    }
}


// –азмер €чейки пам€ти, байт, которыми оперирует аллокатор
size_t ManagedBlockD_Impl::CellSizeImpl()
{
    return sizeof( Descriptor );
}


// ”становка m_device, если в к-тор был передан nullptr
void ManagedBlockD_Impl::SetDevice( ID3D12Device *dev )
{
    if( !m_device )
    {
        m_device = dev;
    }
}