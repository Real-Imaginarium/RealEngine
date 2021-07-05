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
    // �������������� "�����" ���������, ������������ ������� ����� ����� ������ ����� ������������� ������������ ����� � SetupManagedBlock()
    AtomicConstantsAllocator( uint8_t mode, ManagedBlockC_Impl *mb_impl = nullptr );

    // ����������� ������ ����������� ���� ������ ��������, ����������� ��� ���������� 'num_constants' ������������ (256 ����) ��������
    AtomicConstantsAllocator( ID3D12Device *dev, size_t num_constants, uint8_t mode, ManagedBlockC_Impl *mb_impl = nullptr );

    // ����������� ������ ����������� ���� ������ �������� 'num_constants' ������������ (256 ����) ��������, ���������� ��� � ������������� ���������� 'parent_alloc'
    AtomicConstantsAllocator( ID3D12Device *dev, size_t num_constants, GPURegionsAllocator *parent_alloc, bool clean_when_dealloc, ManagedBlockC_Impl *mb_impl = nullptr );

    // ������������ ��������� ����������� ���� ������ �� GPU � ������� � 'mem_start', �������� 'num_constants' ������������ (256 ����) ��������, ��������� �������
    AtomicConstantsAllocator( ID3D12Device *dev, CP_ID3D12Resource buffer, uint8_t *mem_start, size_t num_constants, uint8_t mode, ManagedBlockC_Impl *mb_impl = nullptr );

    // ������� ����������� ���� ������ ��� ���������� 'num_constants' ������������ (256 ����) ��������
    void SetupManagedBlock( ID3D12Device *dev, size_t num_constants );

    // ������� ����������� ���� ������ ��� ���������� 'num_constants' ������������ (256 ����) ��������, ���������� ��� � ������������� ���������� 'parent_alloc'
    void SetupManagedBlock( ID3D12Device *dev, size_t num_constants, GPURegionsAllocator *parent_alloc );

    // ������� ����������� ���� ������ ��� ���������� 'num_constants' ������������ (256 ����) ��������, ���������� ��� � ������������� ���������� 'parent_alloc'
    void SetupManagedBlock( ID3D12Device *dev, CP_ID3D12Resource buffer, uint8_t *mem_start, size_t num_constants );

    // ����������� ����� ������������ (256 ����) �������� ����������� 'size' (������������ ��������� ������������ ���������� � ���������)
    virtual uint8_t *Allocate( size_t size ) override;

    // ������� ����� �������� ������������ ������� (256 ����) � ������� � 'start' � ����������� 'size' ��������
    virtual Error_BasePtr Deallocate( void *start, size_t size ) override;

private:
    ManagedBlockC_Impl *m_casted_mb;                                            // ������ ����������� ��������� �� ���������� IManagedBlock, ����� �� ��������� ������ ���
};


// ���������� ������������ ����� ������
class ManagedBlockC_Impl : public ManagedBlockRWS_Impl
{
    friend class AtomicConstantsAllocator;

protected:
    // ������ ������ ������, ����, �������� ��������� ���������
    virtual size_t CellSizeImpl() override;

private:
    static const size_t g_minimum_size_of_constant;                             // ����������� ������ ��������� � �������� DirectX'12, ����

    // �-��� �����
    ManagedBlockC_Impl( ID3D12Device *dev );
};