#include "ResourceManager.h"

#include "ConstantsPull.hpp"
#include "DxException.h"
#include "MemoryManager.h"

#include <cassert>

DescriptorsAllocator ResourceManager::rtv_alloc = DescriptorsAllocator( DescriptorType::RTV, static_cast<uint8_t>( Mode::FREE_WHEN_DESTRUCT ) );      // ��������� RTV
DescriptorsAllocator ResourceManager::dsv_alloc = DescriptorsAllocator( DescriptorType::DSV, static_cast<uint8_t>( Mode::FREE_WHEN_DESTRUCT ) );      // ��������� DSV
DescriptorsAllocator ResourceManager::cbv_alloc = DescriptorsAllocator( DescriptorType::CBV, static_cast<uint8_t>( Mode::FREE_WHEN_DESTRUCT ) );      // ��������� CBV
AtomicConstantsAllocator ResourceManager::constants_alloc = AtomicConstantsAllocator( Mode::FREE_WHEN_DESTRUCT | Mode::CLEAN_WHEN_DEALLOC );


// �����������.
ResourceManager::ResourceManager()
    //: m_cbv_broker_alloc( static_cast<uint8_t>( Mode::NO_FREE_NO_CLEANUP ))
{}


// ����������.
ResourceManager::~ResourceManager() = default;


// �������������� ResourceManager.
//      dsv_format  - ������ ����������� �� ����� �������
bool ResourceManager::Initialize( ID3D12Device *device, CP_IDXGISwapChain swapchain, CP_ID3D12Resource ds_buff, DXGI_FORMAT dsv_format )
{
    SetupAllocators( device );

    ConstantsPull<int> constPull( device, 10, &MemoryManager::generic_allocator, &constants_alloc, &cbv_alloc );

    m_main_bb_views.Initialize( max_buffering_level, &rtv_alloc );
    m_main_ds_views.Initialize( 1, &dsv_alloc );

    bool ret = false;
    do {
        if( !ResetSwapchain( swapchain, device ) )
            continue;
        if( !ResetDepthStencil( device, ds_buff, dsv_format ) )
            continue;
        ret = true;
    } while( 0 );
    return ret;
}


// ��������� ����������� �������� ����� ����������� ResourceManager.
bool ResourceManager::Deinitialize()
{
    m_main_bb_views.Clean();
    m_main_ds_views.Clean();

    rtv_alloc.ResetManagedBlock();
    return true;
}


// ����� ����� ������� ��������� � ������������� ����� �������� ����� �������, ��� ��������� ������ ��������� � ������ ��������� ���������
//      dsv_format  - ������ ����������� �� ����� �������
bool ResourceManager::Resize( ID3D12Device *device, size_t new_width, size_t new_height, CP_ID3D12Resource ds_buff, DXGI_FORMAT dsv_format )
{
    if( !ResizeSwapChain( device, new_width, new_height ) ) {
        return false;
    }
    if( !ResetDepthStencil( device, ds_buff, dsv_format ) ) {
        return false;
    }
    return true;
}


// �������� ����������� ������ ��������� ������ ���������
void ResourceManager::AdvanceBackBufferIndex()
{
    if( m_curr_bb_index + 1 >= m_back_buffers_num )
        m_curr_bb_index = 0;
    else
        m_curr_bb_index++;
}


// ������ ����� ��������� ��� ������ ��� ����������� �� �����
void ResourceManager::SwapBackBuffers()
{
    THROW_ON_FAIL( m_swapchain->Present( 0, 0 ) );
}


// ����������� ���������� ������������
void ResourceManager::SetupAllocators( ID3D12Device *device )
{
    rtv_alloc.SetupManagedBlock( device, 16, &MemoryManager::generic_allocator );       // TODO: ����� RTV �������� �� ��������
    dsv_alloc.SetupManagedBlock( device, 16, &MemoryManager::generic_allocator );       // TODO: ����� DSV �������� �� ��������
    cbv_alloc.SetupManagedBlock( device, 16, &MemoryManager::generic_allocator );       // TODO: ����� CBV �������� �� ��������
    constants_alloc.SetupManagedBlock( device, 16 );                                    // TODO: ����� ������������ �������� �������� �� ��������
    //m_cbv_broker_alloc.SetupManagedBlock( 16, &cbv_alloc );                             // TODO: ����� CBV �������� �� ��������
}


// ������������� ����� ��������. �������� ������ ��������� � �� ����������� ����� ����������� - ��. ReleaseBackBuffers() / SetupBackBuffers().
bool ResourceManager::ResetSwapchain( CP_IDXGISwapChain swapchain, ID3D12Device *device )
{
    if( !swapchain ) {
        return false;
    }
    m_swapchain.Reset();
    m_swapchain = swapchain;

    DXGI_SWAP_CHAIN_DESC desc;
    m_swapchain->GetDesc( &desc );
    m_back_buffers_num = desc.BufferCount;

    if( m_back_buffers_num > max_buffering_level ) {
        return false;
    }
    ReleaseBackBuffers();

    return SetupBackBuffers( device );
}


// ������������� ����� �������� ����� ������� (��� ���������� ��� ���� �����������).
bool ResourceManager::ResetDepthStencil( ID3D12Device *device, CP_ID3D12Resource ds_buff, DXGI_FORMAT dsv_format )
{
    if( !ds_buff ) {
        return false;
    }
    ReleaseDepthStencilBuffer();
    m_main_ds = ds_buff;

    D3D12_DEPTH_STENCIL_VIEW_DESC dsv_desc;
    dsv_desc.Flags = D3D12_DSV_FLAG_NONE;
    dsv_desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
    dsv_desc.Format = dsv_format;
    dsv_desc.Texture2D.MipSlice = 0;
    //m_main_ds_views = Descriptor::CreateDescriptorUPtr( device, "main_dsv", dsv_desc, m_main_ds.Get(), &dsv_alloc );
    m_main_ds_views.EmplaceBack( device, "main_dsv", dsv_desc, m_main_ds.Get() );
    return true;
}


// ����� ����� ��������� ���������. �������� ������ ��������� � �� ����������� ����� ����������� - ��. ReleaseBackBuffers() / SetupBackBuffers().
bool ResourceManager::ResizeSwapChain( ID3D12Device *device, size_t width, size_t height )
{
    ReleaseBackBuffers();
    DXGI_SWAP_CHAIN_DESC desc;
    m_swapchain->GetDesc( &desc );
    m_swapchain->ResizeBuffers( m_back_buffers_num, static_cast<UINT>( width ), static_cast<UINT>( height ), desc.BufferDesc.Format, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH );
    return SetupBackBuffers( device );
}


// ���������� ��� ����� ��������� ��������� (Reset ��� Resize). ��������� �������� ������ ��������� � �� �����������.
bool ResourceManager::SetupBackBuffers( ID3D12Device *device )
{
    if( !m_swapchain )
    {
        return false;
    }
    for( size_t i = 0; i < m_back_buffers_num; ++i )
    {
        THROW_ON_FAIL( m_swapchain->GetBuffer( static_cast<UINT>( i ), IID_PPV_ARGS( &m_main_bb[i] )));
        m_main_bb_views.EmplaceBack( device, "main_rtv_" + std::to_string( i ), D3D12_RENDER_TARGET_VIEW_DESC{}, m_main_bb[i].Get() );
    }
    return true;
}


// ������� ������� � ����������� �������� ������� ���������.
void ResourceManager::ReleaseBackBuffers()
{
    for( uint8_t i = 0; i < max_buffering_level; ++i )
    {
        m_main_bb[i].Reset();
    }
    m_main_bb_views.Clean();
    m_curr_bb_index = 0;
}


// ������� ������ � ���������� ��������� ������ �������.
void ResourceManager::ReleaseDepthStencilBuffer()
{
    m_main_ds.Reset();
    m_main_ds_views.Clean();
}