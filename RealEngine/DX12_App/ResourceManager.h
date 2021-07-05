#pragma once

#include "AtomicConstantsAllocator.h"
#include "BasicMaterial.h"
#include "Constant.hpp"
#include "ConstantsVector.hpp"
#include "ConstantsPull.hpp"
#include "DescriptorsAllocator.h"
#include "DataTypes.h"
#include "GPURegionsAllocator.h"
#include "NamedVector.hpp"
#include "PullAllocator.h"
#include "RenderItem.h"

#include <dxgi1_4.h>


using CP_IDXGISwapChain = Microsoft::WRL::ComPtr<IDXGISwapChain>;
using CP_ID3D12Resource = Microsoft::WRL::ComPtr<ID3D12Resource>;
using CP_ID3DBlob = Microsoft::WRL::ComPtr<ID3DBlob>;


class DescriptorsAllocator;
struct ID3D12Device;
struct IDXGISwapChain;


// ���� ���������� ����-������������ ��� - ������� - ����� ���������. ����������� ������ ����� ����� �����.
struct MeshGeometry
{
    CP_ID3D12Resource VertexBufferGPU = nullptr;
    CP_ID3D12Resource IndexBufferGPU = nullptr;

    CP_ID3DBlob VertexBufferCPU = nullptr;
    CP_ID3DBlob IndexBufferCPU = nullptr;

    CP_ID3D12Resource VertexBufferUploader = nullptr;
    CP_ID3D12Resource IndexBufferUploader = nullptr;

    // Data about the buffers.
    UINT VertexByteStride = 0;
    UINT VertexBufferByteSize = 0;
    DXGI_FORMAT IndexFormat = DXGI_FORMAT_R16_UINT;
    UINT IndexBufferByteSize = 0;

    D3D12_VERTEX_BUFFER_VIEW VertexBufferView()const
    {
        D3D12_VERTEX_BUFFER_VIEW vbv;
        vbv.BufferLocation = VertexBufferGPU->GetGPUVirtualAddress();
        vbv.StrideInBytes = VertexByteStride;
        vbv.SizeInBytes = VertexBufferByteSize;

        return vbv;
    }

    D3D12_INDEX_BUFFER_VIEW IndexBufferView()const
    {
        D3D12_INDEX_BUFFER_VIEW ibv;
        ibv.BufferLocation = IndexBufferGPU->GetGPUVirtualAddress();
        ibv.Format = IndexFormat;
        ibv.SizeInBytes = IndexBufferByteSize;

        return ibv;
    }
};


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
    bool Initialize( ID3D12Device *device, ID3D12GraphicsCommandList *cmd_list, CP_IDXGISwapChain swapchain, CP_ID3D12Resource ds_buff, DXGI_FORMAT dsv_format );

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

public:
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

    // ������ �� ����� ������ ���������� � ������ ��������, ������������ �����������
    void BuildMaterials( ID3D12Device *dev /*NamedVector ���� ��������/������..., Schema*/ );

    // ������ �� ����� ������ RenderItem'��
    void BuildRenderItems( ID3D12Device *dev /*NamedVector ����������, Schema*/ );

    void BuildShadersAndInputLayout();                                                      // TODO: ����������� � DX12_App
    void BuildRootSignature( ID3D12Device *dev );                                           // TODO: ����������� � DX12_App
    void BuildPSOs( ID3D12Device *dev );                                                    // TODO: ����������� � DX12_App

    static DescriptorsAllocator rtv_alloc;                                                  // ��������� RTV
    static DescriptorsAllocator dsv_alloc;                                                  // ��������� DSV
    static DescriptorsAllocator cbv_alloc;                                                  // ��������� CBV �����
    static DescriptorsAllocator srv_alloc;                                                  // ��������� SRV

    static AtomicConstantsAllocator constants_alloc;                                        // ��������� ������������ �������� (256 ����) �����
    static GPURegionsAllocator gpu_generic_alloc;                                           // ��������� ������ �� GPU �����
    static GPURegionsAllocator gpu_generic_alloc_test;                                           // ��������� ������ �� GPU �����



    ConstantsPull<ObjectConstant>                       m_object_constants_pull;            // ���� ��������, ������ ������������ ����������� RenderItem'��
    GPUStructersVector<MaterialConstant>                m_material_structs;                 // ������ ��������, ������������ ����������� - ����� ������������ ������� (StructuredBuffer<>)
    NamedVector<BasicMaterial>                          m_materials;                        // ���������, ������������ RenderItem'���
    NamedVector<RenderItem>                             m_render_items;                     // RenderItem'�, ���������� ���������, ���� ��������

    // ��������� ������ ������ ��� ������ �����. �� ���� ���������:
    //      NamedVector � ������������� ���� ��������, ������, ��������������, ��������� � ������.
    //      ����� (��� ���������, ����� ������������, ������������ ����������; ��������� ������� - �� ��� ������������ ��������� ���������).
    //   �� ���� ������ ��������� ������ ��������, ������������ ����������� � ������ ����� ����������. ����������� � ���������� �����. �������� ��� ������� ���������.
    //   ����������:
    //      ConstantsVector - ���������, ������������ �����������.
    //      NamedVector - ����������, ���������.
    // RenderItem'� ������ ������ ��� �����. �� ���� ���������:
    //      NamedVector � �����������
    //      �����-�� ��������� (��� �� ������) � ������.
    //      ����� (��� RenderItem'�,
    //             ��� ���������(��) - �� ���� �������� ��������� �� ������ ��������� � �������. �� ��������� �� ������ �������� ��� ��������� �������� ������ ���������, ������������ ����������,
    //             ������������� ����� (?),
    //             ��������� � ������������)
    //   �� ���� ������ ������ ������ RenderItem'��, ����������� Object-���������� �����. ��������� ��������.
    //   ����������:
    //      NamedVector - ����������, RenderItem'�

    std::unique_ptr<ConstantCPU<PassConstant>>          m_main_pass_constant;               // TODO: CameraManager

    MeshGeometry                                        m_geometry;                         // TODO: ����������� � DX12_App. ���� ��� RenderItem'� ���������� ���������� ���� - �������

    std::unordered_map<std::string, CP_ID3DBlob>        m_shaders;                          // TODO: ��������, ����� ShaderManager ��� ���-�� ��������
    std::vector<D3D12_INPUT_ELEMENT_DESC>               m_input_layout;                     // TODO: ����������� � DX12_App.
    Microsoft::WRL::ComPtr<ID3D12RootSignature>         m_root_signature = nullptr;         // TODO: ����������� � DX12_App.
    std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D12PipelineState>> mPSOs;     // TODO: ����������� � DX12_App.

    uint8_t                                             m_back_buffers_num = 0;             // ������� �����������
    uint8_t                                             m_curr_bb_index = 0;                // ������� ������ ��������� ������ ���������

    Vector<Descriptor, DescriptorsAllocator>            m_main_bb_views;                    // ����������� �������� ������� ���������
    Vector<Descriptor, DescriptorsAllocator>            m_main_ds_views;                    // ����������(�) ��������� ������ �������

    CP_IDXGISwapChain                                   m_swapchain = nullptr;              // ���������� ��������
    CP_ID3D12Resource                                   m_main_bb[max_buffering_level];     // �������� ������ ��������� (�� ���������)
    CP_ID3D12Resource                                   m_main_ds = nullptr;                // �������� ����� �������
};