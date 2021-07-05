#pragma once

#include "GameTimer.h"
#include "ResourceManager.h"
#include "Updater.h"
#include "WinWorkflowManager.h"

#include <memory>


using CP_ID3D12GraphicsCommandList = Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>;
using CP_ID3D12CommandAllocator = Microsoft::WRL::ComPtr<ID3D12CommandAllocator>;
using CP_ID3D12CommandQueue = Microsoft::WRL::ComPtr<ID3D12CommandQueue>;
using CP_ID3D12Resource = Microsoft::WRL::ComPtr<ID3D12Resource>;
using CP_IDXGIFactory4 = Microsoft::WRL::ComPtr<IDXGIFactory4>;
using CP_ID3D12Device = Microsoft::WRL::ComPtr<ID3D12Device>;
using CP_ID3D12Fence = Microsoft::WRL::ComPtr<ID3D12Fence>;


class DX12_App
{
public:
    static const uint8_t back_buffers_count = 2;

    DX12_App( HINSTANCE app_inst );

    bool Initialize();

    bool Deinitialize();

    int Run();

    void Update( GameTimer *gt );

private:
    void Draw( GameTimer *gt );

    void DrawRenderItems();

    bool Init_MemoryManager();

    bool Init_WindowManager();

    bool Init_Device();

    bool Init_ResourceManager();

    bool Deinit_MemoryManager();

    bool Deinit_WindowManager();

    bool Deinit_Device();

    bool Deinit_ResourceManager();

    void CreateCommandObjects();

    CP_IDXGISwapChain CreateSwapChain( size_t bb_width, size_t bb_height );

    CP_ID3D12Resource CreateMainDepthStencilBuffer( size_t ds_width, size_t ds_height );

    void MakeDepthBufferReady();

    void SetupViewPort( size_t x, size_t y, size_t w, size_t h, float depth_min, float depth_max );

    void SetupScissorRect( size_t x, size_t y, size_t w, size_t h );

    void FlushCommandQueue();

    void Resize( size_t new_width, size_t new_height );

    void ToggleMSAA();

    void CalculateFrameStats();

    static void OnWindowActivation( bool active );
    static void OnWindowStartResizing( size_t curr_width, size_t curr_height );
    static void OnWindowStopResizing( size_t new_width, size_t new_height );
    static void OnWindowResized( size_t new_width, size_t new_height, bool active );
    static void OnWindowResizedByAPI( size_t new_width, size_t new_height );
    static void OnMouseToggle( bool pushed, size_t pos_x, size_t pos_y );
    static void OnMouseMove( size_t pos_x, size_t pos_y );
    static void OnKeyUp( size_t key );


    void TestUpdateMaterial();

    static DX12_App                     *m_dx12_app;

    std::unique_ptr<WinWorkflowManager> m_window_manager;
    std::unique_ptr<ResourceManager>    m_resource_manager;

    CP_ID3D12GraphicsCommandList        m_cmd_list;
    CP_ID3D12CommandAllocator           m_cmd_list_alloc;
    CP_ID3D12CommandQueue               m_cmd_queue;

    CP_IDXGIFactory4                    m_dxgi_factory4;
    CP_ID3D12Device                     m_device;
    CP_ID3D12Fence                      m_fence;

    DXGI_FORMAT                         m_backbuffer_format;
    DXGI_FORMAT                         m_depthstencil_format;

    D3D12_VIEWPORT                      m_screen_viewport;
    D3D12_RECT                          m_scissor_rect;

    GameTimer                           m_timer;

    uint64_t                            m_current_fence;
    uint8_t                             m_buffering_level;
    uint8_t                             m_msaa_samples;
    uint8_t                             m_msaa_quality;
    bool                                m_msaa_enabled;
    bool                                m_app_paused;

    std::unique_ptr<Updater>            m_updater;
};