#pragma once

#include "d3dx12.h"
#include "GPURegionsAllocator.h"
#include "RegionsAllocator.h"

#include <wrl.h>

struct ID3D12Device;
struct ID3D12Resource;
class ManagedBlockC_Impl;

using CP_ID3D12Device = Microsoft::WRL::ComPtr<ID3D12Device>;
using CP_ID3D12Resource = Microsoft::WRL::ComPtr<ID3D12Resource>;


class AtomicConstantsAllocator : public GPURegionsAllocator//RegionsAllocator
{
public:
    //  онструируетс€ "сырой" аллокатор, использовать который можно будет только после инициализации управл€емого блока в SetupManagedBlock()
    AtomicConstantsAllocator( uint8_t mode, ManagedBlockC_Impl *mb_impl = nullptr );

    //  онструктор создаЄт управл€емый блок пам€ти размером, достаточным дл€ размещени€ 'num_constants' элементарных (256 байт) констант
    AtomicConstantsAllocator( ID3D12Device *dev, size_t num_constants, uint8_t mode, ManagedBlockC_Impl *mb_impl = nullptr );

    //  онструктор создаЄт управл€емый блок пам€ти размером 'num_constants' элементарных (256 байт) констант, запрашива€ его у родительского аллокатора 'parent_alloc'
    AtomicConstantsAllocator( ID3D12Device *dev, size_t num_constants, GPURegionsAllocator *parent_alloc, bool clean_when_dealloc, ManagedBlockC_Impl *mb_impl = nullptr );

    //  онструктору передаЄтс€ управл€емый блок пам€ти на GPU с началом в 'mem_start', размером 'num_constants' элементарных (256 байт) констант, созданный заранее
    AtomicConstantsAllocator( ID3D12Device *dev, CP_ID3D12Resource buffer, uint8_t *mem_start, size_t num_constants, uint8_t mode, ManagedBlockC_Impl *mb_impl = nullptr );

    // —оздать управл€емый блок пам€ти дл€ размещени€ 'num_constants' элементарных (256 байт) констант
    void SetupManagedBlock( ID3D12Device *dev, size_t num_constants );

    // —оздать управл€емый блок пам€ти дл€ размещени€ 'num_constants' элементарных (256 байт) констант, запрашива€ его у родительского аллокатора 'parent_alloc'
    void SetupManagedBlock( ID3D12Device *dev, size_t num_constants, GPURegionsAllocator *parent_alloc );

    // —оздать управл€емый блок пам€ти дл€ размещени€ 'num_constants' элементарных (256 байт) констант, запрашива€ его у родительского аллокатора 'parent_alloc'
    void SetupManagedBlock( ID3D12Device *dev, CP_ID3D12Resource buffer, uint8_t *mem_start, size_t num_constants );

    // «апрашивает пачку элементарных (256 байт) констант количеством 'size' (возвращаемый указатель предполагает приведение к константе)
    virtual uint8_t *Allocate( size_t size ) override;

    // –елизит пачку констант минимального размера (256 байт) с началом в 'start' и количеством 'size' констант
    virtual Error_BasePtr Deallocate( void *start, size_t size ) override;

private:
    ManagedBlockC_Impl *m_casted_mb;                                            // ѕросто кастованный указатель на реализацию IManagedBlock, чтобы не кастовать каждый раз
};


// –еализаци€ управл€емого блока пам€ти
class ManagedBlockC_Impl : public ManagedBlockRWS_Impl
{
    friend class AtomicConstantsAllocator;

protected:
    // –азмер €чейки пам€ти, байт, которыми оперирует аллокатор
    virtual size_t CellSizeImpl() override;

private:
    static const size_t g_minimum_size_of_constant;                             // ћинимальный размер константы в терминах DirectX'12, байт

    //  -тор блока
    ManagedBlockC_Impl( ID3D12Device *dev );
};