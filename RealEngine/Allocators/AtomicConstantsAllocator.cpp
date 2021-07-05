#include "AtomicConstantsAllocator.h"

#include "DxException.h"
#include "RegionsList.hpp"


const size_t ManagedBlockC_Impl::g_minimum_size_of_constant = 256;

// �������������� "�����" ���������, ������������ ������� ����� ����� ������ ����� ������������� ������������ ����� ����� �� ������ SetupManagedBlock().
AtomicConstantsAllocator::AtomicConstantsAllocator( uint8_t mode, ManagedBlockC_Impl *mb_impl )
    : GPURegionsAllocator( mode, 
                           mb_impl ? mb_impl : new ManagedBlockC_Impl( nullptr ))
    , m_casted_mb( dynamic_cast<ManagedBlockC_Impl*>( m_mb_impl.get()))
{}


// ����������� ������ ����������� ���� ������ ��������, ����������� ��� ���������� 'num_constants' ������������ (256 ����) ��������
AtomicConstantsAllocator::AtomicConstantsAllocator( ID3D12Device *dev, size_t num_constants, uint8_t mode, ManagedBlockC_Impl *mb_impl )
    : GPURegionsAllocator( mode, 
                           mb_impl ? mb_impl : new ManagedBlockC_Impl( nullptr ))
    , m_casted_mb( dynamic_cast<ManagedBlockC_Impl*>( m_mb_impl.get()))
{
    SetupManagedBlock( dev, num_constants );
}


// ����������� ������ ����������� ���� ������ �������� 'num_constants' ������������ (256 ����) ��������, ���������� ��� � ������������� ���������� 'parent_alloc'
AtomicConstantsAllocator::AtomicConstantsAllocator( ID3D12Device *dev, size_t num_constants, GPURegionsAllocator *parent_alloc, bool clean_when_dealloc, ManagedBlockC_Impl *mb_impl )
    : GPURegionsAllocator( static_cast<uint8_t>( clean_when_dealloc ? Mode::CLEAN_WHEN_DEALLOC : Mode::NO_FREE_NO_CLEANUP ),
                           mb_impl ? mb_impl : new ManagedBlockC_Impl( nullptr ))
    , m_casted_mb( dynamic_cast<ManagedBlockC_Impl*>( m_mb_impl.get()))
{
    SetupManagedBlock( dev, num_constants, parent_alloc );
}


// ������������ ��������� ����������� ���� ������ �� GPU � ������� � 'mem_start', �������� 'num_constants' ������������ (256 ����) ��������, ��������� �������
AtomicConstantsAllocator::AtomicConstantsAllocator( ID3D12Device *dev, CP_ID3D12Resource buffer, uint8_t *mem_start, size_t num_constants, uint8_t mode, ManagedBlockC_Impl *mb_impl )
    : GPURegionsAllocator( mode,
                           mb_impl ? mb_impl : new ManagedBlockC_Impl( nullptr ))
    , m_casted_mb( dynamic_cast<ManagedBlockC_Impl*>( m_mb_impl.get()))
{
    SetupManagedBlock( dev, buffer, mem_start, num_constants );
}


// ������� ����������� ���� ������ ��� ���������� 'num_constants' ������������ (256 ����) ��������
void AtomicConstantsAllocator::SetupManagedBlock( ID3D12Device *dev, size_t num_constants )
{
    GPURegionsAllocator::SetupManagedBlock( dev, num_constants * ManagedBlockC_Impl::g_minimum_size_of_constant );
}


// ������� ����������� ���� ������ ��� ���������� 'num_constants' ������������ (256 ����) ��������, ���������� ��� � ������������� ���������� 'parent_alloc'
void AtomicConstantsAllocator::SetupManagedBlock( ID3D12Device *dev, size_t num_constants, GPURegionsAllocator *parent_alloc )
{
    GPURegionsAllocator::SetupManagedBlock( dev, num_constants, parent_alloc );
}


// ������� ����������� ���� ������ ��� ���������� 'num_constants' ������������ (256 ����) ��������, ���������� ��� � ������������� ���������� 'parent_alloc'
void AtomicConstantsAllocator::SetupManagedBlock( ID3D12Device *dev, CP_ID3D12Resource buffer, uint8_t *mem_start, size_t num_constants )
{
    GPURegionsAllocator::SetupManagedBlock( dev, buffer, mem_start, num_constants * ManagedBlockC_Impl::g_minimum_size_of_constant );
}


// ����������� ����� ������������ (256 ����) �������� ����������� 'size' (������������ ��������� ������������ ���������� � ���������)
uint8_t *AtomicConstantsAllocator::Allocate( size_t size )
{
    return GPURegionsAllocator::Allocate( size * ManagedBlockC_Impl::g_minimum_size_of_constant );
}


// ������� ����� �������� ������������ ������� (256 ����) � ������� � 'start' � ����������� 'size' ��������
Error_BasePtr AtomicConstantsAllocator::Deallocate( void *start, size_t size )
{
    return GPURegionsAllocator::Deallocate( start, size * ManagedBlockC_Impl::g_minimum_size_of_constant );
}

// �-��� �����
ManagedBlockC_Impl::ManagedBlockC_Impl( ID3D12Device *dev ) : ManagedBlockRWS_Impl( dev )
{}


// ������ ������ ������, ����, �������� ��������� ���������
size_t ManagedBlockC_Impl::CellSizeImpl()
{
    return g_minimum_size_of_constant;
}