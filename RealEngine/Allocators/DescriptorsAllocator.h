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
    // �������������� "�����" ���������, ������������ ������� ����� ����� ������ ����� ������������� ������������ ����� ����� �� ������ SetupManagedBlock().
    DescriptorsAllocator( DescriptorType d_type, uint8_t mode, ManagedBlockD_Impl *mb_impl = nullptr );

    // ����������� ������ ����������� ���� ������ ��������, ����������� ��� ���������� 'num_descriptors' ������������.
    DescriptorsAllocator( ID3D12Device *dev, DescriptorType d_type, size_t num_descriptors, uint8_t mode, ManagedBlockD_Impl *mb_impl = nullptr );

    // ����������� ������ ����������� ���� ������ ��������, ����������� ��� ���������� 'num_descriptors' ������������, ���������� ��� � �������� ���������� 'parent_alloc'
    DescriptorsAllocator( ID3D12Device *dev, DescriptorType d_type, size_t num_descriptors, RegionsAllocator *parent_alloc, bool clean_when_dealloc, ManagedBlockD_Impl *mb_impl = nullptr );

    // ������������ ��������� ����������� ���� ������ � ������� � 'mem_start' � ��������, ����������� ��� ���������� 'num_descriptors' ������������, ��������� �������
    DescriptorsAllocator( ID3D12Device *dev, DescriptorType d_type, uint8_t *mem_start, size_t num_descriptors, uint8_t mode, ManagedBlockD_Impl *mb_impl = nullptr );

    // ������� ����������� ���� ������ ��� ���������� 'num_descriptors' ������������
    void SetupManagedBlock( ID3D12Device *dev, size_t num_descriptors );

    // ��������� � �������� ���������� 'parent_alloc' ���� ������ ��� ���������� 'num_descriptors' ������������
    void SetupManagedBlock( ID3D12Device *dev, size_t num_descriptors, RegionsAllocator *parent_alloc );

    // ���������� ��������� ������� ����������� ���� ������ � ������� � 'mem_start', �������� 'num_descriptors' ������������
    void SetupManagedBlock( ID3D12Device *dev, uint8_t *mem_start, size_t num_descriptors );

    // ����������� ����� ������������ ����������� 'size' (������������ ��������� ������������ ���������� � Descriptor*)
    virtual uint8_t *Allocate( size_t size ) override;

    // ������� ����� ������������ � ������� � 'start' � ����������� 'size' ������������
    virtual Error_BasePtr Deallocate( void *start, size_t size ) override;

private:
    using RegionsAllocator::SetupManagedBlock;

    ManagedBlockD_Impl *m_casted_mb;                                            // ������ ����������� ��������� �� ���������� IManagedBlock, ����� �� ��������� ������ ���
};


// ���������� ������������ ����� ������
class ManagedBlockD_Impl : public ManagedBlockRB_Impl
{
    friend class DescriptorsAllocator;

protected:
    // ������������� �����
    virtual void SetupManagedBlockImpl( uint8_t *mem_start, size_t mem_size ) override;

    // ������ ������ ������, ����, �������� ��������� ���������
    virtual size_t CellSizeImpl() override;

private:
    // �-��� �����
    ManagedBlockD_Impl( ID3D12Device *dev, DescriptorType m_descr_type );

    // ��������� m_device, ���� � �-��� ��� ������� nullptr
    void SetDevice( ID3D12Device *dev );

    CP_ID3D12Device         m_device;                                           // ������ (� ��� ������� �������� ���� ������������ � ���� �����������)
    CP_ID3D12DescriptorHeap m_descr_heap;                                       // ���� ������������
    DescriptorType          m_descr_type;                                       // ��� �����������
    uint16_t                m_descr_size;                                       // ������ ����������� � ���� (�� GPU), ����
};

