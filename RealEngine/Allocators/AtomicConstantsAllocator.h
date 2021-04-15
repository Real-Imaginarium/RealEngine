#pragma once

#include "d3dx12.h"
#include "RegionsAllocator.h"

#include <wrl.h>

struct ID3D12Device;
struct ID3D12Resource;
class ManagedBlockC_Impl;

using CP_ID3D12Device = Microsoft::WRL::ComPtr<ID3D12Device>;
using CP_ID3D12Resource = Microsoft::WRL::ComPtr<ID3D12Resource>;


class AtomicConstantsAllocator : public RegionsAllocator
{
public:
    // Конструируется "сырой" аллокатор, использовать который можно будет только после инициализации управляемого блока в SetupManagedBlock()
    AtomicConstantsAllocator( uint8_t mode, ManagedBlockC_Impl *mb_impl = nullptr );

    // Конструктор создаёт управляемый блок памяти размером, достаточным для размещения 'num_constants' элементарных (256 байт) констант
    AtomicConstantsAllocator( ID3D12Device *dev, size_t num_constants, uint8_t mode, ManagedBlockC_Impl *mb_impl = nullptr );

    // Создать управляемый блок памяти для размещения 'num_constants' элементарных (256 байт) констант
    void SetupManagedBlock( ID3D12Device *dev, size_t num_constants );

    // Запрашивает пачку элементарных (256 байт) констант количеством 'size' (возвращаемый указатель предполагает приведение к константе)
    virtual uint8_t *Allocate( size_t size ) override;

    // Релизит пачку констант минимального размера (256 байт) с началом в 'start' и количеством 'size' констант
    virtual Error_BasePtr Deallocate( void *start, size_t size ) override;

    // Вычисляет виртуальный адрес ресурса в GPU для отображённого на этот ресурс указателя в CPU
    D3D12_GPU_VIRTUAL_ADDRESS CalculateVirtualAddress( void *ptr );

private:
    using RegionsAllocator::SetupManagedBlock;

    ManagedBlockC_Impl *m_casted_mb;                                            // Просто кастованный указатель на реализацию IManagedBlock, чтобы не кастовать каждый раз
};


// Реализация управляемого блока памяти
class ManagedBlockC_Impl : public ManagedBlockRB_Impl
{
    friend class AtomicConstantsAllocator;

protected:
    // Инициализация блока
    virtual void SetupManagedBlockImpl( uint8_t *mem_start, size_t mem_size ) override;

    // Сброс блока
    virtual void ResetManagedBlockImpl() override;

    // Размер ячейки памяти, байт, которыми оперирует аллокатор
    virtual size_t CellSizeImpl() override;

private:
    static const size_t g_minimum_size_of_constant;                             // Минимальный размер константы в терминах DirectX'12, байт

    // К-тор блока
    ManagedBlockC_Impl( ID3D12Device *dev );

    // Установка m_device, если в к-тор был передан nullptr
    void SetDevice( ID3D12Device *dev );

    CP_ID3D12Device         m_device;                                           // Девайс (с его помощью создаётся ресурс буфера констант)
    CP_ID3D12Resource       m_constants_buffer;                                 // Буфер констант
};