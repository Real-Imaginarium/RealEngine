#include "AtomicConstantsAllocator.h"

#include "DxException.h"
#include "RegionsList.hpp"


const size_t ManagedBlockC_Impl::g_minimum_size_of_constant = 256;

// �������������� "�����" ���������, ������������ ������� ����� ����� ������ ����� ������������� ������������ ����� ����� �� ������ SetupManagedBlock().
AtomicConstantsAllocator::AtomicConstantsAllocator( uint8_t mode, ManagedBlockC_Impl *mb_impl )
    : RegionsAllocator( mode, mb_impl ? mb_impl : new ManagedBlockC_Impl( nullptr ))
    , m_casted_mb( dynamic_cast<ManagedBlockC_Impl*>( m_mb_impl.get()))
{}


// ����������� ������ ����������� ���� ������ ��������, ����������� ��� ���������� 'num_constants' ������������ (256 ����) ��������
AtomicConstantsAllocator::AtomicConstantsAllocator( ID3D12Device *dev, size_t num_constants, uint8_t mode, ManagedBlockC_Impl *mb_impl )
    : RegionsAllocator( mode, mb_impl ? mb_impl : new ManagedBlockC_Impl( nullptr ) )
    , m_casted_mb( dynamic_cast<ManagedBlockC_Impl *>( m_mb_impl.get() ) )
{
    SetupManagedBlock( dev, num_constants * ManagedBlockC_Impl::g_minimum_size_of_constant );
}


// ������� ����������� ���� ������ ��� ���������� 'num_constants' ������������ (256 ����) ��������
void AtomicConstantsAllocator::SetupManagedBlock( ID3D12Device *dev, size_t num_constants )
{
    m_casted_mb->SetDevice( dev );
    IAllocatorCore::SetupManagedBlock( nullptr, num_constants * ManagedBlockC_Impl::g_minimum_size_of_constant );
}


// ����������� ����� ������������ (256 ����) �������� ����������� 'size' (������������ ��������� ������������ ���������� � ���������)
uint8_t *AtomicConstantsAllocator::Allocate( size_t size )
{
    return RegionsAllocator::Allocate( size * ManagedBlockC_Impl::g_minimum_size_of_constant );
}


// ������� ����� �������� ������������ ������� (256 ����) � ������� � 'start' � ����������� 'size' ��������
Error_BasePtr AtomicConstantsAllocator::Deallocate( void *start, size_t size )
{
    return RegionsAllocator::Deallocate( start, size * ManagedBlockC_Impl::g_minimum_size_of_constant );
}


// ��������� ����������� ����� ������� � GPU ��� ������������ �� ���� ������ ��������� � CPU
D3D12_GPU_VIRTUAL_ADDRESS AtomicConstantsAllocator::CalculateVirtualAddress( void *ptr )
{
    if( !ptr || ptr > ManagedBlockStart() || ptr < ManagedBlockStart() )
    {
        return 0;
    }

    // ��������� ������ ��������� ������������ ������ ������������ �����, ����� ���������� ��� � ������������ ������ �� ������ ������� � GPU
    return m_casted_mb->m_constants_buffer->GetGPUVirtualAddress() + ( ManagedBlockStart() - (uint8_t*)ptr );
}


// �-��� �����
ManagedBlockC_Impl::ManagedBlockC_Impl( ID3D12Device *dev )
    : m_device( dev )
    , m_constants_buffer( nullptr )
{}


// ������������� �����
void ManagedBlockC_Impl::SetupManagedBlockImpl( uint8_t *mem_start, size_t mem_size )
{
    mem_start = nullptr;    // �������� �������� ���������, �.�. � ������ ���������� ��� �� ������ ����, ��������� ������������ ����� ID3D12Resource::map() (��. �����)

    // ���� m_device � ������������ ��� ���������� � nullptr - ����� ������� ������� SetDevice() 
    if( !m_device )
    {
        return;
    }

    // ������ ����� �� GPU �������� 'mem_size' ��� �������� ������� ���������� �������� (������������ �-����� ����������)
    THROW_ON_FAIL( m_device->CreateCommittedResource( &CD3DX12_HEAP_PROPERTIES( D3D12_HEAP_TYPE_UPLOAD ),
                                                      D3D12_HEAP_FLAG_NONE,
                                                      &CD3DX12_RESOURCE_DESC::Buffer( mem_size ),
                                                      D3D12_RESOURCE_STATE_GENERIC_READ,
                                                      nullptr,
                                                      IID_PPV_ARGS( &m_constants_buffer )));

    // ������� �� ��������� ������ � ��������� ������������� �����
    THROW_ON_FAIL( m_constants_buffer->Map( 0, nullptr, reinterpret_cast<void**>( &mem_start )));

    ManagedBlockRB_Impl::SetupManagedBlockImpl( mem_start, mem_size );
}


// ����� �����
void ManagedBlockC_Impl::ResetManagedBlockImpl()
{
    m_constants_buffer->Unmap( 0, nullptr );
}


// ������ ������ ������, ����, �������� ��������� ���������
size_t ManagedBlockC_Impl::CellSizeImpl()
{
    return g_minimum_size_of_constant;
}


// ��������� m_device, ���� � �-��� ��� ������� nullptr
void ManagedBlockC_Impl::SetDevice( ID3D12Device *dev )
{
    if( !m_device )
    {
        m_device = dev;
    }
}