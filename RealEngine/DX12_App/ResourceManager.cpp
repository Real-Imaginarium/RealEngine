#include "ResourceManager.h"

//#include "BasicMaterial.h"
#include "ConstantsPull.hpp"
//#include "ConstantsVector.hpp"
//#include "DataTypes.h"
#include "DxException.h"
#include "DX12_Utils.h"
#include "MemoryManager.h"

#include <cassert>
#include <d3dcompiler.h>


using namespace DirectX;

DescriptorsAllocator ResourceManager::rtv_alloc = DescriptorsAllocator( DescriptorType::RTV, static_cast<uint8_t>( Mode::NO_FREE_NO_CLEANUP ));     // Аллокатор RTV
DescriptorsAllocator ResourceManager::dsv_alloc = DescriptorsAllocator( DescriptorType::DSV, static_cast<uint8_t>( Mode::NO_FREE_NO_CLEANUP ));     // Аллокатор DSV
DescriptorsAllocator ResourceManager::cbv_alloc = DescriptorsAllocator( DescriptorType::CBV, static_cast<uint8_t>( Mode::NO_FREE_NO_CLEANUP ));     // Аллокатор CBV
DescriptorsAllocator ResourceManager::srv_alloc = DescriptorsAllocator( DescriptorType::SRV, static_cast<uint8_t>( Mode::NO_FREE_NO_CLEANUP ));     // Аллокатор SRV
AtomicConstantsAllocator ResourceManager::constants_alloc = AtomicConstantsAllocator( static_cast<uint8_t>( Mode::NO_FREE_NO_CLEANUP ));            // Аллокатор констант
GPURegionsAllocator ResourceManager::gpu_generic_alloc = GPURegionsAllocator( Mode::CLEAN_WHEN_DEALLOC | Mode::FREE_WHEN_DESTRUCT );                // Аллокатор GPU памяти общий


// Конструктор.
ResourceManager::ResourceManager()
{}


// Деструктор.
ResourceManager::~ResourceManager() = default;




// Инициализирует ResourceManager.
//      dsv_format  - формат дескриптора на буфер глубины
bool ResourceManager::Initialize( ID3D12Device *device, ID3D12GraphicsCommandList *cmd_list, CP_IDXGISwapChain swapchain, CP_ID3D12Resource ds_buff, DXGI_FORMAT dsv_format )
{
    SetupAllocators( device );

    m_main_bb_views.Initialize( max_buffering_level, &rtv_alloc );
    m_main_ds_views.Initialize( 1, &dsv_alloc );

    m_object_constants_pull.Initialize( 10, &MemoryManager::generic_allocator, &constants_alloc );

    bool ret = false;
    do {
        if( !ResetSwapchain( swapchain, device ) )
            continue;
        if( !ResetDepthStencil( device, ds_buff, dsv_format ) )
            continue;
        ret = true;
    } while( 0 );

    // Materials
    BuildMaterials( device );

    // RenderItems
    BuildRenderItems( device );

    // Константа main_pass
    uint16_t m_size_256 = (( sizeof( PassConstant ) + 255 ) & ~255 );
    PassConstant *mapped = (PassConstant *)constants_alloc.Allocate( m_size_256 / 256 );
    m_main_pass_constant = std::make_unique<ConstantCPU<PassConstant>>("main_pass", mapped, constants_alloc.CalculateVirtualAddress( mapped ));

    // Build the view matrix, update 'view' part of main_pass
    XMVECTOR pos = XMVectorSet( 0.0f, 0.0f, -50.0f, 1.0f );
    XMVECTOR target = XMVectorZero();
    XMVECTOR up = XMVectorSet( 0.0f, 1.0f, 0.0f, 0.0f );
    XMMATRIX view = XMMatrixLookAtLH( pos, target, up );

    XMStoreFloat4x4( &m_main_pass_constant->map->ViewMat, XMMatrixTranspose( view ));

    // Строим меш коробки
    std::vector<Vertex> vertices = {{{-2.5f,-2.5f,-2.5f}},{{-2.5f,+2.5f,-2.5f}},{{+2.5f,+2.5f,-2.5f}},{{+2.5f,-2.5f,-2.5f}},    // Front
                                    {{-2.5f,-2.5f,+2.5f}},{{+2.5f,-2.5f,+2.5f}},{{+2.5f,+2.5f,+2.5f}},{{-2.5f,+2.5f,+2.5f}},    // Back
                                    {{-2.5f,+2.5f,-2.5f}},{{-2.5f,+2.5f,+2.5f}},{{+2.5f,+2.5f,+2.5f}},{{+2.5f,+2.5f,-2.5f}},    // Top
                                    {{-2.5f,-2.5f,-2.5f}},{{+2.5f,-2.5f,-2.5f}},{{+2.5f,-2.5f,+2.5f}},{{-2.5f,-2.5f,+2.5f}},    // Bottom
                                    {{-2.5f,-2.5f,+2.5f}},{{-2.5f,+2.5f,+2.5f}},{{-2.5f,+2.5f,-2.5f}},{{-2.5f,-2.5f,-2.5f}},    // Left
                                    {{+2.5f,-2.5f,-2.5f}},{{+2.5f,+2.5f,-2.5f}},{{+2.5f,+2.5f,+2.5f}},{{+2.5f,-2.5f,+2.5f}}};   // Right
                                    /*  Front          Back           Top              Bottom                Left               Right     */
    std::vector<uint16_t> indices = {0,1,2,0,2,3,  4,5,6,4,6,7,  8,9,10,8,10,11,  12,13,14,12,14,15,  16,17,18,16,18,19,  20,21,22,20,22,23};

    const UINT vbByteSize = (UINT)vertices.size() * sizeof( Vertex );
    const UINT ibByteSize = (UINT)indices.size() * sizeof( uint16_t );

    m_geometry = {};
    THROW_ON_FAIL( D3DCreateBlob( vbByteSize, &m_geometry.VertexBufferCPU ));
    CopyMemory( m_geometry.VertexBufferCPU->GetBufferPointer(), vertices.data(), vbByteSize );

    THROW_ON_FAIL( D3DCreateBlob( ibByteSize, &m_geometry.IndexBufferCPU ) );
    CopyMemory( m_geometry.IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize );

    m_geometry.VertexBufferGPU = DX12_Utils::CreateDefaultBuffer( device, cmd_list, vertices.data(), vbByteSize, m_geometry.VertexBufferUploader );
    m_geometry.IndexBufferGPU = DX12_Utils::CreateDefaultBuffer( device, cmd_list, indices.data(), ibByteSize, m_geometry.IndexBufferUploader );

    m_geometry.VertexByteStride = sizeof( Vertex );
    m_geometry.VertexBufferByteSize = vbByteSize;
    m_geometry.IndexFormat = DXGI_FORMAT_R16_UINT;
    m_geometry.IndexBufferByteSize = ibByteSize;

    // Шейдеры и InputLayout
    BuildShadersAndInputLayout();

    // RootSignature
    BuildRootSignature( device );

    // PSO
    BuildPSOs( device );

    return ret;
}


// Выполняет необходимые операции перед разрушением ResourceManager.
bool ResourceManager::Deinitialize()
{
    m_main_ds_views.Clean();
    m_main_bb_views.Clean();
    m_render_items.Deinitialize();
    m_materials.Deinitialize();
    m_material_structs.Deinitialize();
    m_object_constants_pull.Deinitialize();

    rtv_alloc.ResetManagedBlock();
    return true;
}


// Задаёт новые размеры свапчейна и устанавливает новый основной буфер глубины, его дименшоны должны совпадать с новыми размерами свапчейна
//      dsv_format  - формат дескриптора на буфер глубины
bool ResourceManager::Resize( ID3D12Device *device, size_t new_width, size_t new_height, CP_ID3D12Resource ds_buff, DXGI_FORMAT dsv_format )
{
    if( !ResizeSwapChain( device, new_width, new_height ) ) {
        return false;
    }
    if( !ResetDepthStencil( device, ds_buff, dsv_format ) ) {
        return false;
    }

    // Build the proj and viewProj matrix, update 'proj' and 'viewProj' part of main_pass
    XMMATRIX proj = XMMatrixPerspectiveFovLH( 0.25f * 3.1415926535f, static_cast<float>( new_width ) / new_height, 1.0f, 1000.0f );
    XMMATRIX viewProj = XMMatrixMultiply( XMMatrixTranspose( XMLoadFloat4x4( &m_main_pass_constant->map->ViewMat )), proj );
    XMStoreFloat4x4( &m_main_pass_constant->map->ProjMat, XMMatrixTranspose( proj ));
    XMStoreFloat4x4( &m_main_pass_constant->map->ViewProjMat, XMMatrixTranspose( viewProj ));

    return true;
}


// Циклично инкрементит индекс основного буфера отрисовки
void ResourceManager::AdvanceBackBufferIndex()
{
    if( m_curr_bb_index + 1 >= m_back_buffers_num )
        m_curr_bb_index = 0;
    else
        m_curr_bb_index++;
}


// Свапит буфер отрисовки для вывода его содержимого на экран
void ResourceManager::SwapBackBuffers()
{
    THROW_ON_FAIL( m_swapchain->Present( 0, 0 ) );
}


// Настраивает аллокаторы дескрипторов
void ResourceManager::SetupAllocators( ID3D12Device *device )
{
    rtv_alloc.SetupManagedBlock( device, 16, &MemoryManager::generic_allocator );       // TODO: число RTV задавать из конфигов
    dsv_alloc.SetupManagedBlock( device, 16, &MemoryManager::generic_allocator );       // TODO: число DSV задавать из конфигов
    cbv_alloc.SetupManagedBlock( device, 16, &MemoryManager::generic_allocator );       // TODO: число CBV задавать из конфигов
    srv_alloc.SetupManagedBlock( device, 16, &MemoryManager::generic_allocator );       // TODO: число SRV задавать из конфигов
    gpu_generic_alloc.SetupManagedBlock( device, 8196 );                                // TODO: размер GPU памяти общего пользования задавать из конфигов
    constants_alloc.SetupManagedBlock( device, 16, &gpu_generic_alloc );                // TODO: число элементарных констант задавать из конфигов

}


// Устанавливает новый свапчейн. Основные буферы отрисовки и их дескрипторы также обновляются - см. ReleaseBackBuffers() / SetupBackBuffers().
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


// Устанавливает новый основной буфер глубины (его дескриптор при этом обновляется).
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


// Задаёт новые дименшоны свапчейна. Основные буферы отрисовки и их дескрипторы также обновляются - см. ReleaseBackBuffers() / SetupBackBuffers().
bool ResourceManager::ResizeSwapChain( ID3D12Device *device, size_t width, size_t height )
{
    ReleaseBackBuffers();
    DXGI_SWAP_CHAIN_DESC desc;
    m_swapchain->GetDesc( &desc );
    m_swapchain->ResizeBuffers( m_back_buffers_num, static_cast<UINT>( width ), static_cast<UINT>( height ), desc.BufferDesc.Format, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH );
    return SetupBackBuffers( device );
}


// Вызывается при любом изменении свапчейна (Reset или Resize). Обновляет Основные буферы отрисовки и их дескрипторы.
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


// Релизит ресурсы и дескрипторы основных буферов отрисовки.
void ResourceManager::ReleaseBackBuffers()
{
    for( uint8_t i = 0; i < max_buffering_level; ++i )
    {
        m_main_bb[i].Reset();
    }
    m_main_bb_views.Clean();
    m_curr_bb_index = 0;
}


// Релизит ресурс и дескриптор основного буфера глубины.
void ResourceManager::ReleaseDepthStencilBuffer()
{
    m_main_ds.Reset();
    m_main_ds_views.Clean();
}


// Строит по схеме вектор материалов и вектор констант, используемых материалами
void ResourceManager::BuildMaterials( ID3D12Device *dev /*NamedVector, Schema*/ )
{
    m_material_structs.Initialize( 4, &gpu_generic_alloc );
    m_materials.Initialize( 4, &MemoryManager::generic_allocator );

    // "M_Red" - имя и материала, и константы, им используемой. Будет браться из схемы.
    m_materials.EmplaceBack( "M_Red", &m_material_structs, m_material_structs.EmplaceBack(), XMFLOAT4{ 1.0f, 0.0f, 0.0f, 1.0f } );
    m_materials.EmplaceBack( "M_Green", &m_material_structs, m_material_structs.EmplaceBack(), XMFLOAT4{ 0.0f, 1.0f, 0.0f, 1.0f } );
    m_materials.EmplaceBack( "M_Blue", &m_material_structs, m_material_structs.EmplaceBack(), XMFLOAT4{ 0.0f, 0.0f, 1.0f, 1.0f } );
    m_materials.EmplaceBack( "M_White", &m_material_structs, m_material_structs.EmplaceBack(), XMFLOAT4{ 1.0f, 1.0f, 1.0f, 1.0f } );

}


// Строит по схеме вектор RenderItem'ов
void ResourceManager::BuildRenderItems( ID3D12Device *dev /*NamedVector материалов, Schema*/ )
{
    m_render_items.Initialize( 3, &MemoryManager::generic_allocator );
    m_render_items.EmplaceBack( "GreenStone", &m_object_constants_pull, &m_materials, "M_Green", -15.0f, 0.0f, 10.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f );
    m_render_items.EmplaceBack( "RedStone", &m_object_constants_pull, &m_materials, "M_Red",   0.0f, 0.0f, 10.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f );
    m_render_items.EmplaceBack( "BlueStone", &m_object_constants_pull, &m_materials, "M_Blue",  15.0f, 0.0f, 10.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f );

}


void ResourceManager::BuildShadersAndInputLayout()
{
    m_shaders["VS_Standard"] = DX12_Utils::CompileShader( L"VS_Standard.hlsl", nullptr, "VS", "vs_5_1" );
    m_shaders["PS_Opaque"] = DX12_Utils::CompileShader( L"PS_Opaque.hlsl", nullptr, "PS", "ps_5_1" );

    m_input_layout =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
    };
}


void ResourceManager::BuildRootSignature( ID3D12Device *dev )
{
    CD3DX12_ROOT_PARAMETER slotRootParameter[4];
    slotRootParameter[0].InitAsConstantBufferView( 0 ); // register b0 - cbPerObject
    slotRootParameter[1].InitAsConstantBufferView( 1 ); // register b1 - cbPass
    slotRootParameter[2].InitAsConstantBufferView( 2 ); // register b2 - cbMaterial
    slotRootParameter[3].InitAsShaderResourceView( 0 ); // register t0 - gMaterials

    // A root signature is an array of root parameters.
    CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc( 4, slotRootParameter, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT );

    // create a root signature with a single slot which points to a descriptor range consisting of a single constant buffer
    CP_ID3DBlob serializedRootSig = nullptr;
    CP_ID3DBlob errorBlob = nullptr;
    HRESULT hr = D3D12SerializeRootSignature( &rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1, serializedRootSig.GetAddressOf(), errorBlob.GetAddressOf() );
   
    if( errorBlob != nullptr )
    {
        ::OutputDebugStringA( (char *)errorBlob->GetBufferPointer() );
    }
    THROW_ON_FAIL( hr );
    THROW_ON_FAIL( dev->CreateRootSignature( 0, serializedRootSig->GetBufferPointer(), serializedRootSig->GetBufferSize(), IID_PPV_ARGS( m_root_signature.GetAddressOf() ) ) );
}


void ResourceManager::BuildPSOs( ID3D12Device *dev )
{
    D3D12_GRAPHICS_PIPELINE_STATE_DESC opaquePsoDesc;

    // PSO for opaque objects.
    ZeroMemory( &opaquePsoDesc, sizeof( D3D12_GRAPHICS_PIPELINE_STATE_DESC ) );
    opaquePsoDesc.InputLayout = { m_input_layout.data(), (UINT)m_input_layout.size() };
    opaquePsoDesc.pRootSignature = m_root_signature.Get();
    opaquePsoDesc.VS =
    {
        reinterpret_cast<BYTE *>( m_shaders["VS_Standard"]->GetBufferPointer() ),
        m_shaders["VS_Standard"]->GetBufferSize()
    };
    opaquePsoDesc.PS =
    {
        reinterpret_cast<BYTE *>( m_shaders["PS_Opaque"]->GetBufferPointer() ),
        m_shaders["PS_Opaque"]->GetBufferSize()
    };
    opaquePsoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC( D3D12_DEFAULT );
    opaquePsoDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
    opaquePsoDesc.BlendState = CD3DX12_BLEND_DESC( D3D12_DEFAULT );
    opaquePsoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC( D3D12_DEFAULT );
    opaquePsoDesc.SampleMask = UINT_MAX;
    opaquePsoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    opaquePsoDesc.NumRenderTargets = 1;
    opaquePsoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;       // TODO: DX12_App::m_backbuffer_format, когда переместится в DX12_App
    opaquePsoDesc.SampleDesc.Count = 1;                             // TODO: DX12_App::m_msaa_enabled ? 4 : 1, когда переместится в DX12_App
    opaquePsoDesc.SampleDesc.Quality = 0;                           // TODO: DX12_App::m_msaa_enabled ? (DX12_App::m_msaa_quality - 1) : 0;
    opaquePsoDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;        // TODO: DX12_App::m_depthstencil_format, когда переместится в DX12_App
    THROW_ON_FAIL( dev->CreateGraphicsPipelineState( &opaquePsoDesc, IID_PPV_ARGS( &mPSOs["opaque"] ) ) );
    
    // PSO for opaque wireframe objects.
    D3D12_GRAPHICS_PIPELINE_STATE_DESC opaqueWireframePsoDesc = opaquePsoDesc;
    opaqueWireframePsoDesc.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
    THROW_ON_FAIL( dev->CreateGraphicsPipelineState( &opaqueWireframePsoDesc, IID_PPV_ARGS( &mPSOs["opaque_wireframe"] ) ) );
}