#pragma once

#include "AtomicConstantsAllocator.h"
#include "DescriptorsAllocator.h"
#include "PullAllocator.h"
#include "Vector.hpp"

#include <dxgi1_4.h>


using CP_IDXGISwapChain = Microsoft::WRL::ComPtr<IDXGISwapChain>;
using CP_ID3D12Resource = Microsoft::WRL::ComPtr<ID3D12Resource>;

class DescriptorsAllocator;
struct ID3D12Device;
struct IDXGISwapChain;

class ResourceManager
{
public:
    static constexpr uint8_t max_buffering_level = 4;           // ������������ ������� �����������

    // �����������.
    ResourceManager();

    // ����������.
    ~ResourceManager();

    // �������������� ResourceManager.
    //      dsv_format  - ������ ����������� �� ����� �������
    bool Initialize( ID3D12Device *device, CP_IDXGISwapChain swapchain, CP_ID3D12Resource ds_buff, DXGI_FORMAT dsv_format );

    // ��������� ����������� �������� ����� ����������� ResourceManager.
    bool Deinitialize();

    // ����� ����� ������� ��������� � ������������� ����� �������� ����� �������, ��� ��������� ������ ��������� � ������ ��������� ���������
    //      dsv_format  - ������ ����������� �� ����� �������
    bool Resize( ID3D12Device *device, size_t new_width, size_t new_height, CP_ID3D12Resource ds_buff, DXGI_FORMAT dsv_format );

    // �������� ����������� ������ ��������� ������ ���������
    void AdvanceBackBufferIndex();

    // ������ ����� ��������� ��� ������ ��� ����������� �� �����
    void SwapBackBuffers();

    // ������ ��������� ������ �������
    ID3D12Resource* DepthStencilBuffer() const { return m_main_ds.Get(); }

    // ������ �������� ��������� ������ ���������
    ID3D12Resource* CurrentBackBuffer() const { return m_main_bb[m_curr_bb_index].Get(); }

    // ���������� ��������� ������ ������� (CPU_HANDLE)
    CD3DX12_CPU_DESCRIPTOR_HANDLE DepthStencilView() const { return m_main_ds_views[0].CPU_Handle(); }

    // ���������� �������� ��������� ������ ��������� (CPU_HANDLE)
    CD3DX12_CPU_DESCRIPTOR_HANDLE CurrentBackBufferView() const { return m_main_bb_views[m_curr_bb_index].CPU_Handle(); }

private:
    // ����������� ���������� ������������
    void SetupAllocators( ID3D12Device *device );

    // ������������� ����� ��������. �������� ������ ��������� � �� ����������� ����� ����������� - ��. ReleaseBackBuffers() / SetupBackBuffers().
    bool ResetSwapchain( CP_IDXGISwapChain swapchain, ID3D12Device *device );

    // ������������� ����� �������� ����� ������� (��� ���������� ��� ���� �����������).
    bool ResetDepthStencil( ID3D12Device *device, CP_ID3D12Resource ds_buff, DXGI_FORMAT dsv_format );

    // ����� ����� ��������� ���������. �������� ������ ��������� � �� ����������� ����� ����������� - ��. ReleaseBackBuffers() / SetupBackBuffers().
    bool ResizeSwapChain( ID3D12Device *device, size_t width, size_t height );
    
    // ���������� ��� ����� ��������� ��������� (Reset ��� Resize). ��������� �������� ������ ��������� � �� �����������.
    bool SetupBackBuffers( ID3D12Device *device );
    
    // ������� ������� � ����������� �������� ������� ���������.
    void ReleaseBackBuffers();

    // ������� ������ � ���������� ��������� ������ �������.
    void ReleaseDepthStencilBuffer();

    static DescriptorsAllocator rtv_alloc;                                      // ��������� RTV
    static DescriptorsAllocator dsv_alloc;                                      // ��������� DSV
    static DescriptorsAllocator cbv_alloc;                                      // ��������� CBV �����
    static AtomicConstantsAllocator constants_alloc;                            // ��������� ������������ �������� (256 ����) �����
    
    //PullAllocator<Descriptor> m_cbv_broker_alloc;                               // ���������-������ CBV - �������� ������ �� ������ CBV ���������� � ������ � �� 1 �����������


    uint8_t                                  m_back_buffers_num = 0;            // ������� �����������
    uint8_t                                  m_curr_bb_index = 0;               // ������� ������ ��������� ������ ���������

    Vector<Descriptor, DescriptorsAllocator> m_main_bb_views;                   // ����������� �������� ������� ���������
    Vector<Descriptor, DescriptorsAllocator> m_main_ds_views;                   // ����������(�) ��������� ������ �������

    CP_IDXGISwapChain                        m_swapchain = nullptr;             // ���������� ��������
    CP_ID3D12Resource                        m_main_bb[max_buffering_level];    // �������� ������ ��������� (�� ���������)
    CP_ID3D12Resource                        m_main_ds = nullptr;               // �������� ����� �������
};