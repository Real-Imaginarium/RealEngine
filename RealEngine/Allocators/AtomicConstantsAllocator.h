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
    // �������������� "�����" ���������, ������������ ������� ����� ����� ������ ����� ������������� ������������ ����� � SetupManagedBlock()
    AtomicConstantsAllocator( uint8_t mode, ManagedBlockC_Impl *mb_impl = nullptr );

    // ����������� ������ ����������� ���� ������ ��������, ����������� ��� ���������� 'num_constants' ������������ (256 ����) ��������
    AtomicConstantsAllocator( ID3D12Device *dev, size_t num_constants, uint8_t mode, ManagedBlockC_Impl *mb_impl = nullptr );

    // ������� ����������� ���� ������ ��� ���������� 'num_constants' ������������ (256 ����) ��������
    void SetupManagedBlock( ID3D12Device *dev, size_t num_constants );

    // ����������� ����� ������������ (256 ����) �������� ����������� 'size' (������������ ��������� ������������ ���������� � ���������)
    virtual uint8_t *Allocate( size_t size ) override;

    // ������� ����� �������� ������������ ������� (256 ����) � ������� � 'start' � ����������� 'size' ��������
    virtual Error_BasePtr Deallocate( void *start, size_t size ) override;

    // ��������� ����������� ����� ������� � GPU ��� ������������ �� ���� ������ ��������� � CPU
    D3D12_GPU_VIRTUAL_ADDRESS CalculateVirtualAddress( void *ptr );

private:
    using RegionsAllocator::SetupManagedBlock;

    ManagedBlockC_Impl *m_casted_mb;                                            // ������ ����������� ��������� �� ���������� IManagedBlock, ����� �� ��������� ������ ���
};


// ���������� ������������ ����� ������
class ManagedBlockC_Impl : public ManagedBlockRB_Impl
{
    friend class AtomicConstantsAllocator;

protected:
    // ������������� �����
    virtual void SetupManagedBlockImpl( uint8_t *mem_start, size_t mem_size ) override;

    // ����� �����
    virtual void ResetManagedBlockImpl() override;

    // ������ ������ ������, ����, �������� ��������� ���������
    virtual size_t CellSizeImpl() override;

private:
    static const size_t g_minimum_size_of_constant;                             // ����������� ������ ��������� � �������� DirectX'12, ����

    // �-��� �����
    ManagedBlockC_Impl( ID3D12Device *dev );

    // ��������� m_device, ���� � �-��� ��� ������� nullptr
    void SetDevice( ID3D12Device *dev );

    CP_ID3D12Device         m_device;                                           // ������ (� ��� ������� �������� ������ ������ ��������)
    CP_ID3D12Resource       m_constants_buffer;                                 // ����� ��������
};