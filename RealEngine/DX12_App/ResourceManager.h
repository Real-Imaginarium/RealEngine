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
    static constexpr uint8_t max_buffering_level = 4;           // Максимальный уровень буферизации

    // Конструктор.
    ResourceManager();

    // Деструктор.
    ~ResourceManager();

    // Инициализирует ResourceManager.
    //      dsv_format  - формат дескриптора на буфер глубины
    bool Initialize( ID3D12Device *device, CP_IDXGISwapChain swapchain, CP_ID3D12Resource ds_buff, DXGI_FORMAT dsv_format );

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

private:
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

    static DescriptorsAllocator rtv_alloc;                                      // Аллокатор RTV
    static DescriptorsAllocator dsv_alloc;                                      // Аллокатор DSV
    static DescriptorsAllocator cbv_alloc;                                      // Аллокатор CBV общий
    static AtomicConstantsAllocator constants_alloc;                            // Аллокатор элементарных констант (256 байт) общий
    
    //PullAllocator<Descriptor> m_cbv_broker_alloc;                               // Аллокатор-брокер CBV - получает память от общего CBV аллокатора и раздаёт её по 1 дескриптору


    uint8_t                                  m_back_buffers_num = 0;            // Уровень буферизации
    uint8_t                                  m_curr_bb_index = 0;               // Текущий индекс основного буфера отрисовки

    Vector<Descriptor, DescriptorsAllocator> m_main_bb_views;                   // Дескрипторы основных буферов отрисовки
    Vector<Descriptor, DescriptorsAllocator> m_main_ds_views;                   // Дескриптор(ы) основного буфера глубины

    CP_IDXGISwapChain                        m_swapchain = nullptr;             // Собственно свапчейн
    CP_ID3D12Resource                        m_main_bb[max_buffering_level];    // Основные буферы отрисовки (из свапчейна)
    CP_ID3D12Resource                        m_main_ds = nullptr;               // Основной буфер глубины
};