#pragma once

#include "Descriptor.h"
#include "RegionsAllocator.h"

#include <wrl.h>


using CP_ID3D12DescriptorHeap = Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>;
using CP_ID3D12Device = Microsoft::WRL::ComPtr<ID3D12Device>;

class ManagedBlockD_Impl;


class DescriptorsAllocator : public RegionsAllocator
{
public:
    // Конструируется "сырой" аллокатор, использовать который можно будет только после инициализации управляемого блока одной из версий SetupManagedBlock().
    DescriptorsAllocator( DescriptorType d_type, uint8_t mode, ManagedBlockD_Impl *mb_impl = nullptr );

    // Конструктор создаёт управляемый блок памяти размером, достаточным для размещения 'num_descriptors' дескрипторов.
    DescriptorsAllocator( ID3D12Device *dev, DescriptorType d_type, size_t num_descriptors, uint8_t mode, ManagedBlockD_Impl *mb_impl = nullptr );

    // Конструктор создаёт управляемый блок памяти размером, достаточным для размещения 'num_descriptors' дескрипторов, запрашивая его у внешнего аллокатора 'parent_alloc'
    DescriptorsAllocator( ID3D12Device *dev, DescriptorType d_type, size_t num_descriptors, RegionsAllocator *parent_alloc, bool clean_when_dealloc, ManagedBlockD_Impl *mb_impl = nullptr );

    // Конструктору передаётся управляемый блок памяти с началом в 'mem_start' и размером, достаточным для размещения 'num_descriptors' дескрипторов, созданный заранее
    DescriptorsAllocator( ID3D12Device *dev, DescriptorType d_type, uint8_t *mem_start, size_t num_descriptors, uint8_t mode, ManagedBlockD_Impl *mb_impl = nullptr );

    // Создать управляемый блок памяти для размещения 'num_descriptors' дескрипторов
    void SetupManagedBlock( ID3D12Device *dev, size_t num_descriptors );

    // Запросить у внешнего аллокатора 'parent_alloc' блок памяти для размещения 'num_descriptors' дескрипторов
    void SetupManagedBlock( ID3D12Device *dev, size_t num_descriptors, RegionsAllocator *parent_alloc );

    // Установить созданный заранее управляемый блок памяти с началом в 'mem_start', размером 'num_descriptors' дескрипторов
    void SetupManagedBlock( ID3D12Device *dev, uint8_t *mem_start, size_t num_descriptors );

    // Запрашивает пачку дескрипторов количеством 'size' (возвращаемый указатель предполагает приведение к Descriptor*)
    virtual uint8_t *Allocate( size_t size ) override;

    // Релизит пачку дескрипторов с началом в 'start' и количеством 'size' дескрипторов
    virtual Error_BasePtr Deallocate( void *start, size_t size ) override;

private:
    using RegionsAllocator::SetupManagedBlock;

    ManagedBlockD_Impl *m_casted_mb;                                            // Просто кастованный указатель на реализацию IManagedBlock, чтобы не кастовать каждый раз
};


// Реализация управляемого блока памяти
class ManagedBlockD_Impl : public ManagedBlockRB_Impl
{
    friend class DescriptorsAllocator;

protected:
    // Инициализация блока
    virtual void SetupManagedBlockImpl( uint8_t *mem_start, size_t mem_size ) override;

    // Размер ячейки памяти, байт, которыми оперирует аллокатор
    virtual size_t CellSizeImpl() override;

private:
    // К-тор блока
    ManagedBlockD_Impl( ID3D12Device *dev, DescriptorType m_descr_type );

    // Установка m_device, если в к-тор был передан nullptr
    void SetDevice( ID3D12Device *dev );

    CP_ID3D12Device         m_device;                                           // Девайс (с его помощью создаётся куча дескрипторов и сами дескрипторы)
    CP_ID3D12DescriptorHeap m_descr_heap;                                       // Куча дескрипторов
    DescriptorType          m_descr_type;                                       // Тип дескриптора
    uint16_t                m_descr_size;                                       // Размер дескриптора в куче (на GPU), байт
};

