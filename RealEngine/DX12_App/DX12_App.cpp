#include "DX12_App.h"

#include "BasicMaterial.h"
#include "ConstantsVector.hpp"
#include "DxException.h"
#include "LogError.h"
#include "LogInfo.h"
#include "MemoryManager.h"
#include "RenderItem.h"

#include <cassert>
#include <DirectXColors.h>
#include <WinUser.h>


DX12_App* DX12_App::m_dx12_app = nullptr;


DX12_App::DX12_App( HINSTANCE app_inst )
    : m_window_manager( new WinWorkflowManager(app_inst, OnWindowActivation, OnWindowStartResizing, OnWindowStopResizing, OnWindowResized, OnWindowResizedByAPI, OnMouseToggle, OnMouseMove, OnKeyUp ))
    , m_resource_manager( new ResourceManager )
    , m_cmd_list( nullptr )
    , m_cmd_list_alloc( nullptr )
    , m_cmd_queue( nullptr )
    , m_dxgi_factory4( nullptr )
    , m_device( nullptr )
    , m_fence( nullptr )
    , m_backbuffer_format( DXGI_FORMAT_R8G8B8A8_UNORM )
    , m_depthstencil_format( DXGI_FORMAT_D24_UNORM_S8_UINT )
    , m_current_fence( 0 )
    , m_buffering_level( 2 )
    , m_msaa_samples( 4 )
    , m_msaa_quality( 0 )       // Запрашивается для заданного формата backbuffer'а
    , m_msaa_enabled( false )
    , m_app_paused( false )
    , m_updater( new Updater )
{
    assert( m_dx12_app == nullptr && "There can be only one DX12_App" );
    m_dx12_app = this;
}


bool DX12_App::Initialize()
{
    bool ret = false;
    do {
        if( !Init_MemoryManager() )
            continue;
        if( !Init_WindowManager() )
            continue;
        if( !Init_Device() )
            continue;

        // Reset the command list to prep for initialization commands.
        THROW_ON_FAIL( m_cmd_list->Reset( m_cmd_list_alloc.Get(), nullptr ) );

        if( !Init_ResourceManager() )
            continue;

        MakeDepthBufferReady();
        SetupViewPort( 0, 0, m_window_manager->Width(), m_window_manager->Height(), 0.0f, 1.0f );
        SetupScissorRect( 0, 0, m_window_manager->Width(), m_window_manager->Height());
        ret = true;
    } 
    while( 0 );

    Resize( m_window_manager->Width(), m_window_manager->Height() );

    return ret;
}


bool DX12_App::Deinitialize()
{
    bool ret = false;
    do
    {
        if( !Deinit_ResourceManager() )
            continue;
        if( !Deinit_Device() )
            continue;
        if( !Deinit_WindowManager() )
            continue;
        if( !Deinit_MemoryManager() )
            continue;
        ret = true;
    } while( 0 );

    return ret;
}


int DX12_App::Run()
{
    MSG msg = { 0 };
    m_timer.Reset();

    while( msg.message != WM_QUIT )
    {
        // If there are Window messages then process them.
        if( PeekMessage( &msg, 0, 0, 0, PM_REMOVE ) )
        {
            TranslateMessage( &msg );
            DispatchMessage( &msg );
        }
        // Otherwise, do animation/game stuff.
        else
        {
            m_timer.Tick();

            if( !m_app_paused )
            {
                CalculateFrameStats();
                Update( &m_timer );
                Draw( &m_timer );
            }
            else
            {
                Sleep( 100 );
            }
        }
    }
    return static_cast<int>( msg.wParam );
}


void DX12_App::Update( GameTimer *gt )
{
    float angle = gt->DeltaTime() * 36; // 36 град/сек
    m_resource_manager->m_render_items["GreenStone"].RotateX( angle );
    m_resource_manager->m_render_items["RedStone"].RotateY( angle );
    m_resource_manager->m_render_items["BlueStone"].RotateZ( angle );

    m_updater->AddItemToUpdate( &m_resource_manager->m_render_items["GreenStone"] );
    m_updater->AddItemToUpdate( &m_resource_manager->m_render_items["RedStone"] );
    m_updater->AddItemToUpdate( &m_resource_manager->m_render_items["BlueStone"] );

    m_updater->UpdateItems();
}


void DX12_App::Draw( GameTimer *gt )
{
    // Reuse the memory associated with command recording. We can only reset when the associated command lists have finished execution on the GPU.
    THROW_ON_FAIL( m_cmd_list_alloc->Reset());

    // A command list can be reset after it has been added to the command queue via ExecuteCommandList. Reusing the command list reuses memory.
    THROW_ON_FAIL( m_cmd_list->Reset( m_cmd_list_alloc.Get(), m_resource_manager->mPSOs["opaque_wireframe"].Get()));

    // Set the viewport and scissor rect.  This needs to be reset whenever the command list is reset.
    m_cmd_list->RSSetViewports( 1, &m_screen_viewport );
    m_cmd_list->RSSetScissorRects( 1, &m_scissor_rect );

    // Indicate a state transition on the resource usage.
    m_cmd_list->ResourceBarrier( 1, &CD3DX12_RESOURCE_BARRIER::Transition( m_resource_manager->CurrentBackBuffer(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET ));

    // Clear the back buffer and depth buffer.
    m_cmd_list->ClearRenderTargetView( m_resource_manager->CurrentBackBufferView(), DirectX::Colors::Black, 0, nullptr );
    m_cmd_list->ClearDepthStencilView( m_resource_manager->DepthStencilView(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr );

    // Specify the buffers we are going to render to.
    m_cmd_list->OMSetRenderTargets( 1, &m_resource_manager->CurrentBackBufferView(), true, &m_resource_manager->DepthStencilView());

    m_cmd_list->SetGraphicsRootSignature( m_resource_manager->m_root_signature.Get() );

    m_cmd_list->SetGraphicsRootConstantBufferView( 1, m_resource_manager->m_main_pass_constant->va );   // cbPass

    // Draw render items
    m_cmd_list->SetGraphicsRootShaderResourceView( 3, m_resource_manager->m_material_structs.VirtualStartAddress());
    DrawRenderItems();

    // Indicate a state transition on the resource usage.
    m_cmd_list->ResourceBarrier( 1, &CD3DX12_RESOURCE_BARRIER::Transition( m_resource_manager->CurrentBackBuffer(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT ));

    // Done recording commands.
    THROW_ON_FAIL( m_cmd_list->Close() );

    // Add the command list to the queue for execution.
    ID3D12CommandList *cmdsLists[] = { m_cmd_list.Get() };
    m_cmd_queue->ExecuteCommandLists( _countof( cmdsLists ), cmdsLists );

    // swap the back and front buffers
    m_resource_manager->SwapBackBuffers();
    m_resource_manager->AdvanceBackBufferIndex();

    // Wait until frame commands are complete. This waiting is inefficient and is done for simplicity. Later we will show how to organize our rendering code so we do not have to wait per frame.
    FlushCommandQueue();
}


void DX12_App::DrawRenderItems()
{
    // For each render item...
    for( size_t i = 0; i < m_resource_manager->m_render_items.Size(); ++i )
    {
        auto ri = m_resource_manager->m_render_items[i];

        m_cmd_list->IASetVertexBuffers( 0, 1, &m_resource_manager->m_geometry.VertexBufferView());
        m_cmd_list->IASetIndexBuffer( &m_resource_manager->m_geometry.IndexBufferView());
        m_cmd_list->IASetPrimitiveTopology( ri.primitive_type );

        m_cmd_list->SetGraphicsRootConstantBufferView( 0, ri.obj_constant->va );                                                                // cbPerObject

        size_t mat_index = *ri.material_index;

        //m_cmd_list->SetGraphicsRootConstantBufferView( 2, ri.vec_materials->operator[]( mat_index ).m_cvec_ptr->operator[]( mat_index ).va );   // cbPerMaterial

        m_cmd_list->DrawIndexedInstanced( 36, 1, 0, 0, 0 );
    }
}


bool DX12_App::Init_MemoryManager()
{
    return MemoryManager::Initialize();
}


bool DX12_App::Init_WindowManager()
{
    return m_window_manager->Initialize();
}


bool DX12_App::Init_Device()
{
    {
        Microsoft::WRL::ComPtr<ID3D12Debug> debugController;
        THROW_ON_FAIL( D3D12GetDebugInterface( IID_PPV_ARGS( &debugController )));
        debugController->EnableDebugLayer();
    }

    // Создаём dxgi_factory, она перечисляет хардварные и Warp-адаптеры, создаёт SwapChain
    THROW_ON_FAIL( CreateDXGIFactory1( IID_PPV_ARGS( &m_dxgi_factory4 )));

    // Создаём хардварный device, если не получается - пробуем создать WARP device
    if( FAILED( D3D12CreateDevice( nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS( &m_device ))))            // nullptr - дефолтный адаптер, можно указать другой если их несколько
    {
        Microsoft::WRL::ComPtr<IDXGIAdapter> warp_adapter;
        THROW_ON_FAIL( m_dxgi_factory4->EnumWarpAdapter( IID_PPV_ARGS( &warp_adapter )));
        THROW_ON_FAIL( D3D12CreateDevice( warp_adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS( &m_device )));
    }
    // Создаём fence для синхронизации CPU и GPU
    THROW_ON_FAIL( m_device->CreateFence( 0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS( &m_fence )));

    // Проверяем поддержку 4X MSAA-quality для текущего формата back-буфера. Все Direct3D'11-совместимые девайсы поддерживают 4X MSAA для всех форматов - остаётся проверить только поддержку quality.
    D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS msaa_quality_levels;
    msaa_quality_levels.Format = m_backbuffer_format;                           // in
    msaa_quality_levels.SampleCount = m_msaa_samples;                           // in
    msaa_quality_levels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;     // in
    msaa_quality_levels.NumQualityLevels = 0;                                   // out

    THROW_ON_FAIL( m_device->CheckFeatureSupport( D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &msaa_quality_levels, sizeof( msaa_quality_levels )))

    m_msaa_quality = msaa_quality_levels.NumQualityLevels;
    assert( m_msaa_quality > 0 && "Unexpected MSAA quality level" );

    CreateCommandObjects();
    return true;
}


bool DX12_App::Init_ResourceManager()
{
    auto swapchain = CreateSwapChain( m_window_manager->Width(), m_window_manager->Height() );

    auto ds_buffer = CreateMainDepthStencilBuffer( m_window_manager->Width(), m_window_manager->Height() );

    if( !swapchain || !ds_buffer ) {
        return false;
    }

    bool res = m_resource_manager->Initialize( m_device.Get(), m_cmd_list.Get(), swapchain, ds_buffer, m_depthstencil_format );

    // Execute the initialization commands.
    THROW_ON_FAIL( m_cmd_list->Close());
    ID3D12CommandList *cmdsLists[] = { m_cmd_list.Get() };
    m_cmd_queue->ExecuteCommandLists( _countof( cmdsLists ), cmdsLists );
    
    // Wait until initialization is complete.
    FlushCommandQueue();

    return res;
}


bool DX12_App::Deinit_MemoryManager()
{
    return MemoryManager::Deinitialize();
}


bool DX12_App::Deinit_WindowManager()
{
    return m_window_manager->Deinitialize();
}


bool DX12_App::Deinit_Device()
{
    return true;
}


bool DX12_App::Deinit_ResourceManager()
{
    return m_resource_manager->Deinitialize();
}


void DX12_App::CreateCommandObjects()
{
    D3D12_COMMAND_QUEUE_DESC cmd_queue_desc{};
    cmd_queue_desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    cmd_queue_desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

    THROW_ON_FAIL( m_device->CreateCommandQueue( &cmd_queue_desc, IID_PPV_ARGS( &m_cmd_queue )));
    THROW_ON_FAIL( m_device->CreateCommandAllocator( D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS( m_cmd_list_alloc.GetAddressOf())));
    THROW_ON_FAIL( m_device->CreateCommandList( 0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_cmd_list_alloc.Get(), nullptr, IID_PPV_ARGS( m_cmd_list.GetAddressOf())));

    m_cmd_list->Close();            // Сразу закрываем cmd_list, т.к. при первом к нему обращении будет вызван его Reset. Это требует, чтобы cmd_list находился в закрытом состоянии.
}


CP_IDXGISwapChain DX12_App::CreateSwapChain( size_t bb_width, size_t bb_height )
{
    CP_IDXGISwapChain swap_chain;
    DXGI_SWAP_CHAIN_DESC sw_desc{};
    sw_desc.BufferDesc.Width = static_cast<UINT>( bb_width );
    sw_desc.BufferDesc.Height = static_cast<UINT>( bb_height );
    sw_desc.BufferDesc.RefreshRate.Numerator = 60;
    sw_desc.BufferDesc.RefreshRate.Denominator = 1;
    sw_desc.BufferDesc.Format = m_backbuffer_format;
    sw_desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    sw_desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    sw_desc.SampleDesc.Count = m_msaa_enabled ? m_msaa_samples : 1;
    sw_desc.SampleDesc.Quality = m_msaa_enabled ? ( m_msaa_quality - 1 ) : 0;
    sw_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sw_desc.BufferCount = m_buffering_level;
    sw_desc.OutputWindow = m_window_manager->GetWindowHandler();
    sw_desc.Windowed = m_window_manager->IsWindowed();
    sw_desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    sw_desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

    THROW_ON_FAIL( m_dxgi_factory4->CreateSwapChain( m_cmd_queue.Get(), &sw_desc, swap_chain.GetAddressOf()));

    return swap_chain;
}


CP_ID3D12Resource DX12_App::CreateMainDepthStencilBuffer( size_t ds_width, size_t ds_height )
{
    D3D12_RESOURCE_DESC ds_desc{};
    ds_desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    ds_desc.Alignment = 0;
    ds_desc.Width = static_cast<UINT>( ds_width );
    ds_desc.Height = static_cast<UINT>( ds_height );
    ds_desc.DepthOrArraySize = 1;
    ds_desc.MipLevels = 1;
    ds_desc.Format = DXGI_FORMAT_R24G8_TYPELESS;
    ds_desc.SampleDesc.Count = m_msaa_enabled ? m_msaa_samples : 1;
    ds_desc.SampleDesc.Quality = m_msaa_enabled ? (m_msaa_quality - 1) : 0;
    ds_desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    ds_desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

    D3D12_CLEAR_VALUE opt_clear{};
    opt_clear.Format = m_depthstencil_format;
    opt_clear.DepthStencil.Depth = 1.0f;
    opt_clear.DepthStencil.Stencil = 0;

    CP_ID3D12Resource ret;
    THROW_ON_FAIL( m_device->CreateCommittedResource( &CD3DX12_HEAP_PROPERTIES( D3D12_HEAP_TYPE_DEFAULT ), D3D12_HEAP_FLAG_NONE, &ds_desc, D3D12_RESOURCE_STATE_COMMON, &opt_clear, IID_PPV_ARGS( ret.GetAddressOf())));
    return ret;
}


void DX12_App::MakeDepthBufferReady()
{
    FlushCommandQueue();

    THROW_ON_FAIL( m_cmd_list->Reset( m_cmd_list_alloc.Get(), nullptr ));
    m_cmd_list->ResourceBarrier( 1, &CD3DX12_RESOURCE_BARRIER::Transition( m_resource_manager->DepthStencilBuffer(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_DEPTH_WRITE ));

    THROW_ON_FAIL( m_cmd_list->Close());

    ID3D12CommandList* cmd_lists[] = { m_cmd_list.Get() };
    m_cmd_queue->ExecuteCommandLists( _countof( cmd_lists ), cmd_lists );

    FlushCommandQueue();
}


void DX12_App::SetupViewPort( size_t x, size_t y, size_t w, size_t h, float depth_min, float depth_max )
{
    m_screen_viewport.TopLeftX = static_cast<float>( x );
    m_screen_viewport.TopLeftY = static_cast<float>( y );
    m_screen_viewport.Width = static_cast<float>( w );
    m_screen_viewport.Height = static_cast<float>( h );
    m_screen_viewport.MinDepth = depth_min;
    m_screen_viewport.MaxDepth = depth_max;
}


void DX12_App::SetupScissorRect( size_t x, size_t y, size_t w, size_t h )
{
    m_scissor_rect = { static_cast<LONG>(x), static_cast<LONG>( y ), static_cast<LONG>( w ), static_cast<LONG>( h ) };
}


void DX12_App::FlushCommandQueue()
{
    m_current_fence++;
    THROW_ON_FAIL( m_cmd_queue->Signal( m_fence.Get(), m_current_fence ));

    if( m_fence->GetCompletedValue() < m_current_fence )
    {
        HANDLE eventHandle = CreateEventEx( nullptr, nullptr, false, EVENT_ALL_ACCESS );
        THROW_ON_FAIL( m_fence->SetEventOnCompletion( m_current_fence, eventHandle ));

        WaitForSingleObject( eventHandle, INFINITE );
        CloseHandle( eventHandle );
    }
}


void DX12_App::Resize( size_t new_width, size_t new_height )
{
    if( !m_device || !m_cmd_list_alloc )
    {
        return;
    }
    try {
        FlushCommandQueue();

        auto ds_buffer = CreateMainDepthStencilBuffer( new_width, new_height );

        THROW_ON_FALSE( m_resource_manager->Resize( m_device.Get(), new_width, new_height, ds_buffer, m_depthstencil_format ));

        MakeDepthBufferReady();

        SetupViewPort( 0, 0, new_width, new_height, 0.0, 1.0 );

        SetupScissorRect( 0, 0, new_width, new_height );
    }
    catch( DxException & e )
    {
        MessageBox( 0, e.ToString().c_str(), L"HR Failed", MB_OK );
    }
}


void DX12_App::ToggleMSAA()
{
    m_msaa_enabled = !m_msaa_enabled;
}


void DX12_App::CalculateFrameStats()
{
    // Code computes the average frames per second, and also the average time it takes to render one frame. These stats are appended to the window caption bar.
    static int frameCnt = 0;
    static float timeElapsed = 0.0f;

    frameCnt++;

    // Compute averages over one second period.
    if( ( m_timer.TotalTime() - timeElapsed ) >= 1.0f )
    {
        float fps = (float)frameCnt; // fps = frameCnt / 1
        float mspf = 1000.0f / fps;

        std::string fpsStr = std::to_string( fps );
        std::string mspfStr = std::to_string( mspf );
        std::string windowText = "    FPS: " + fpsStr + "   msec/frame : " + mspfStr;

        m_window_manager->SetMainWindowText( windowText.c_str() );

        // Reset for next average.
        frameCnt = 0;
        timeElapsed += 1.0f;
    }
}


void DX12_App::OnWindowActivation( bool active )
{
    m_dx12_app->m_app_paused = !active;
    if( active ) {
        m_dx12_app->m_timer.Start();
    }
    else {
        m_dx12_app->m_timer.Stop();
    }
}


void DX12_App::OnWindowStartResizing( size_t curr_width, size_t curr_height )
{
    m_dx12_app->m_app_paused = true;
    m_dx12_app->m_timer.Stop();
}


void DX12_App::OnWindowStopResizing( size_t new_width, size_t new_height )
{
    m_dx12_app->m_app_paused = false;
    m_dx12_app->Resize( new_width, new_height );
    m_dx12_app->m_timer.Start();
}


void DX12_App::OnWindowResized( size_t new_width, size_t new_height, bool active )
{
    m_dx12_app->m_app_paused = !active;

    if( active )
    {
        m_dx12_app->Resize( new_width, new_height );
    }
}


void DX12_App::OnWindowResizedByAPI( size_t new_width, size_t new_height )
{
    m_dx12_app->Resize( new_width, new_height );
}


void DX12_App::OnMouseToggle( bool pushed, size_t pos_x, size_t pos_y )
{}


void DX12_App::OnMouseMove( size_t pos_x, size_t pos_y )
{}


void DX12_App::OnKeyUp( size_t key )
{
    switch( key )
    {
        case VK_F2: {
            m_dx12_app->ToggleMSAA();
            break;
        }
        case VK_HOME: {
            m_dx12_app->TestUpdateMaterial();
            break;
        }
    }
}


void DX12_App::TestUpdateMaterial()
{
    m_resource_manager->m_materials["M_Green"].SetColor( XMFLOAT4{ 0.7f, 0.0f, 1.0f, 1.0f } );
    m_updater->AddItemToUpdate( &m_resource_manager->m_materials["M_Green"] );
}

