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


// Пока используем один-единственный меш - коробка - чисто затестить. Полноценные буферы мешей будут позже.
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
    static constexpr uint8_t max_buffering_level = 4;           // Максимальный уровень буферизации

    // Конструктор.
    ResourceManager();

    // Деструктор.
    ~ResourceManager();

    // Инициализирует ResourceManager.
    //      dsv_format  - формат дескриптора на буфер глубины
    bool Initialize( ID3D12Device *device, ID3D12GraphicsCommandList *cmd_list, CP_IDXGISwapChain swapchain, CP_ID3D12Resource ds_buff, DXGI_FORMAT dsv_format );

    // Выполняет необходимые операции перед разрушением ResourceManager.
    bool Deinitialize();

    // Задаёт новые размеры свапчейна и устанавливает новый основной буфер глубины, его дименшоны должны совпадать с новыми размерами свапчейна
    //      dsv_format  - формат дескриптора на буфер глубины
    bool Resize( ID3D12Device *device, size_t new_width, size_t new_height, CP_ID3D12Resource ds_buff, DXGI_FORMAT dsv_format );

    // Циклично инкрементит индекс основного буфера отрисовки
    void AdvanceBackBufferIndex();

    // Свапит буфер отрисовки для вывода его содержимого на экран
    void SwapBackBuffers();

    // Ресурс основного буфера глубины
    ID3D12Resource* DepthStencilBuffer() const { return m_main_ds.Get(); }

    // Ресурс текущего основного буфера отрисовки
    ID3D12Resource* CurrentBackBuffer() const { return m_main_bb[m_curr_bb_index].Get(); }

    // Дескриптор основного буфера глубины (CPU_HANDLE)
    CD3DX12_CPU_DESCRIPTOR_HANDLE DepthStencilView() const { return m_main_ds_views[0].CPU_Handle(); }

    // Дескриптор текущего основного буфера отрисовки (CPU_HANDLE)
    CD3DX12_CPU_DESCRIPTOR_HANDLE CurrentBackBufferView() const { return m_main_bb_views[m_curr_bb_index].CPU_Handle(); }

public:
    // Настраивает аллокаторы дескрипторов
    void SetupAllocators( ID3D12Device *device );

    // Устанавливает новый свапчейн. Основные буферы отрисовки и их дескрипторы также обновляются - см. ReleaseBackBuffers() / SetupBackBuffers().
    bool ResetSwapchain( CP_IDXGISwapChain swapchain, ID3D12Device *device );

    // Устанавливает новый основной буфер глубины (его дескриптор при этом обновляется).
    bool ResetDepthStencil( ID3D12Device *device, CP_ID3D12Resource ds_buff, DXGI_FORMAT dsv_format );

    // Задаёт новые дименшоны свапчейна. Основные буферы отрисовки и их дескрипторы также обновляются - см. ReleaseBackBuffers() / SetupBackBuffers().
    bool ResizeSwapChain( ID3D12Device *device, size_t width, size_t height );
    
    // Вызывается при любом изменении свапчейна (Reset или Resize). Обновляет Основные буферы отрисовки и их дескрипторы.
    bool SetupBackBuffers( ID3D12Device *device );
    
    // Релизит ресурсы и дескрипторы основных буферов отрисовки.
    void ReleaseBackBuffers();

    // Релизит ресурс и дескриптор основного буфера глубины.
    void ReleaseDepthStencilBuffer();

    // Строит по схеме вектор материалов и вектор констант, используемых материалами
    void BuildMaterials( ID3D12Device *dev /*NamedVector карт нормалей/цветов..., Schema*/ );

    // Строит по схеме вектор RenderItem'ов
    void BuildRenderItems( ID3D12Device *dev /*NamedVector материалов, Schema*/ );

    void BuildShadersAndInputLayout();                                                      // TODO: Переместить в DX12_App
    void BuildRootSignature( ID3D12Device *dev );                                           // TODO: Переместить в DX12_App
    void BuildPSOs( ID3D12Device *dev );                                                    // TODO: Переместить в DX12_App

    static DescriptorsAllocator rtv_alloc;                                                  // Аллокатор RTV
    static DescriptorsAllocator dsv_alloc;                                                  // Аллокатор DSV
    static DescriptorsAllocator cbv_alloc;                                                  // Аллокатор CBV общий
    static DescriptorsAllocator srv_alloc;                                                  // Аллокатор SRV

    static AtomicConstantsAllocator constants_alloc;                                        // Аллокатор элементарных констант (256 байт) общий
    static GPURegionsAllocator gpu_generic_alloc;                                           // Аллокатор памяти на GPU общий
    static GPURegionsAllocator gpu_generic_alloc_test;                                           // Аллокатор памяти на GPU общий



    ConstantsPull<ObjectConstant>                       m_object_constants_pull;            // Пулл констант, каждая используется определённым RenderItem'ом
    GPUStructersVector<MaterialConstant>                m_material_structs;                 // Вектор констант, используемых материалами - будет подключаться целиком (StructuredBuffer<>)
    NamedVector<BasicMaterial>                          m_materials;                        // Материалы, используются RenderItem'ами
    NamedVector<RenderItem>                             m_render_items;                     // RenderItem'ы, используют материалы, пулл констант

    // Материалы строит билдер или просто метод. На вход принимает:
    //      NamedVector с дескрипторами карт нормалей, цветов, шероховатостей, металлика и прочих.
    //      Схему (имя материала, имена дескрипторов, используемых материалом; отдельные скаляры - всё для формирования константы материала).
    //   По этим данным формирует вектор констант, используемых материалами и вектор самих материалов. Прописывает в константах соотв. значения для каждого материала.
    //   Возвращает:
    //      ConstantsVector - константы, используемые материалами.
    //      NamedVector - собственно, материалы.
    // RenderItem'ы строит билдер или метод. На вход принимает:
    //      NamedVector с материалами
    //      Какой-то контейнер (ещё не решено) с мешами.
    //      Схему (имя RenderItem'а,
    //             имя материала(ов) - по нему получаем указатель на индекс материала в векторе. Из указателя на индекс материал при отрисовке выдираем индекс константы, используемой материалом,
    //             идентификатор мешей (?),
    //             положение в пространстве)
    //   По этим данным строит вектор RenderItem'ов, прописывает Object-константах соотв. начальные значения.
    //   Возвращает:
    //      NamedVector - собственно, RenderItem'ы

    std::unique_ptr<ConstantCPU<PassConstant>>          m_main_pass_constant;               // TODO: CameraManager

    MeshGeometry                                        m_geometry;                         // TODO: переместить в DX12_App. Пока все RenderItem'ы используют одинаковые меши - коробки

    std::unordered_map<std::string, CP_ID3DBlob>        m_shaders;                          // TODO: возможно, нужен ShaderManager или что-то подобное
    std::vector<D3D12_INPUT_ELEMENT_DESC>               m_input_layout;                     // TODO: переместить в DX12_App.
    Microsoft::WRL::ComPtr<ID3D12RootSignature>         m_root_signature = nullptr;         // TODO: переместить в DX12_App.
    std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D12PipelineState>> mPSOs;     // TODO: переместить в DX12_App.

    uint8_t                                             m_back_buffers_num = 0;             // Уровень буферизации
    uint8_t                                             m_curr_bb_index = 0;                // Текущий индекс основного буфера отрисовки

    Vector<Descriptor, DescriptorsAllocator>            m_main_bb_views;                    // Дескрипторы основных буферов отрисовки
    Vector<Descriptor, DescriptorsAllocator>            m_main_ds_views;                    // Дескриптор(ы) основного буфера глубины

    CP_IDXGISwapChain                                   m_swapchain = nullptr;              // Собственно свапчейн
    CP_ID3D12Resource                                   m_main_bb[max_buffering_level];     // Основные буферы отрисовки (из свапчейна)
    CP_ID3D12Resource                                   m_main_ds = nullptr;                // Основной буфер глубины
};