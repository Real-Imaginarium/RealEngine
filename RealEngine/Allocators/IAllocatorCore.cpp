#include "IAllocatorCore.h"

#include "Error_Allocator.h"
#include "RegionsAllocator.h"
#include "Utilites.hpp"


// �������������� "�����" ���������, ������������ ������� ����� ����� ������ ����� ������������� ������������ ����� ����� �� ������ SetupManagedBlock().
IAllocatorCore::IAllocatorCore( uint8_t mode, IManagedBlock *mb_impl )
    : m_mb_impl( mb_impl )
    , m_parent_alloc( nullptr )
    , m_free_when_destruct( mode & Mode::FREE_WHEN_DESTRUCT )
    , m_clean_when_dealloc( mode & Mode::CLEAN_WHEN_DEALLOC )
    , m_managed_block_ready( false )
{}


// ����������� ������ ����������� ���� ������ �������� 'mem_size' ����
IAllocatorCore::IAllocatorCore( size_t mem_size, uint8_t mode, IManagedBlock *mb_impl ) : IAllocatorCore( mode, mb_impl )
{
    SetupManagedBlock( mem_size );
}


// ����������� ������ ����������� ���� ������ �������� 'mem_size' ����, ���������� ��� � �������� ���������� 'parent_alloc'
IAllocatorCore::IAllocatorCore( size_t mem_size, RegionsAllocator *parent_alloc, bool clean_when_dealloc, IManagedBlock *mb_impl )
    : m_mb_impl( mb_impl )
    , m_parent_alloc( parent_alloc )
    , m_free_when_destruct( false )
    , m_clean_when_dealloc( clean_when_dealloc )
    , m_managed_block_ready( false )
{
    SetupManagedBlock( mem_size, m_parent_alloc );
}


// ������������ ��������� ����������� ���� ������ � ������� � 'mem_start', �������� 'mem_size' ����, ��������� �������
IAllocatorCore::IAllocatorCore( uint8_t *mem_start, size_t mem_size, uint8_t mode, IManagedBlock *mb_impl ) : IAllocatorCore( mode, mb_impl )
{
    SetupManagedBlock( mem_start, mem_size );
}


// ����������, ������� ����������� ���� ������ ��� ����������� �������� (��. ResetManagedBlock())
IAllocatorCore::~IAllocatorCore()
{
    ResetManagedBlock();
}


// ������� ����������� ���� ������ �������� 'mem_size' ����
void IAllocatorCore::SetupManagedBlock( size_t mem_size )
{
    if( !m_managed_block_ready )
    {
        uint8_t *mem_start = nullptr;

        auto err = utils::Attempt_calloc( 20, 100, mem_size, mem_start );            TRACE_ALLOCATOR_THR_ERR( err, EA_Type::MANAGED_BLOCK_CREATION_ERROR, mem_size );

        m_mb_impl->SetupManagedBlockImpl( mem_start, mem_size );

        m_managed_block_ready = true;
    }
}


// ��������� � �������� ���������� 'parent_alloc' ���� ��������, ����������� ��� ���������� 'cell_num' ����� ��������� ����������. ��� ������, ��� ����
// ������ ������ 'parent_alloc' = 5 ����, � ��������� - 20 ����, �� ��� ���������� 3 ����� ����� ����������� 12 ����� ������������� ����������.  ������
// ����������: ������ ������ ��������� ���������� ������ ���� ����� ��� ������ ������������� (� ������� - ������)
void IAllocatorCore::SetupManagedBlock( size_t cell_num, RegionsAllocator *parent_alloc )
{
    if( !m_managed_block_ready )
    {
        m_parent_alloc = parent_alloc;

        size_t cells_to_aquire = cell_num * CellSize() / m_parent_alloc->CellSize();

        m_mb_impl->SetupManagedBlockImpl( m_parent_alloc->Allocate( cells_to_aquire ), cells_to_aquire );

        m_managed_block_ready = true;
    }
}


// ���������� ��������� ������� ����������� ���� ������ � ������� � 'mem_start', �������� 'mem_size' ����
void IAllocatorCore::SetupManagedBlock( uint8_t *mem_start, size_t mem_size )
{
    if( !m_managed_block_ready )
    {
        m_mb_impl->SetupManagedBlockImpl( mem_start, mem_size );

        m_managed_block_ready = true;
    }
}


// ������� ����������� ���� ������
void IAllocatorCore::ResetManagedBlock()
{
    if( m_managed_block_ready )
    {
        m_managed_block_ready = false;
        if( m_parent_alloc ) {
            m_parent_alloc->Deallocate( m_mb_impl->m_start, m_mb_impl->m_size );
            m_parent_alloc = nullptr;
            return;
        }
        if( m_free_when_destruct )
        {
            m_mb_impl->ResetManagedBlockImpl();
        }
    }
}


size_t IAllocatorCore::CellSize()
{
    return m_mb_impl->CellSizeImpl();
}


// ��������� �� ������ ������������ ����� ������
uint8_t* IAllocatorCore::ManagedBlockStart()
{
    return m_mb_impl->m_start;
}


// ��������� �� ����� + 1 ������������ ����� ������
uint8_t *IAllocatorCore::ManagedBlockEnd()
{
    return m_mb_impl->m_start + m_mb_impl->m_size;
}


// ������ ������������ ����� ������, ����
size_t IAllocatorCore::ManagedBlockSize()
{
    return m_mb_impl->m_size;
}
