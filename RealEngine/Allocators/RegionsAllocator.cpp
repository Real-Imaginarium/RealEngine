#include "RegionsAllocator.h"
#include "RegionsList.hpp"
#include "Error_Allocator.h"


// �������������� "�����" ���������, ������������ ������� ����� ����� ������ ����� ������������� ������������ ����� ����� �� ������ SetupManagedBlock().
RegionsAllocator::RegionsAllocator( uint8_t mode, ManagedBlockRB_Impl *mb_impl )
    : IAllocatorCore( mode, mb_impl ? mb_impl : new ManagedBlockRB_Impl )
    , m_casted_mb( mb_impl ? mb_impl : dynamic_cast<ManagedBlockRB_Impl*>( m_mb_impl.get()))
{}

// ����������� ������ ����������� ���� ������ �������� 'mem_size' ����
RegionsAllocator::RegionsAllocator( size_t mem_size, uint8_t mode, ManagedBlockRB_Impl *mb_impl )
    : IAllocatorCore( mem_size, mode, mb_impl ? mb_impl : new ManagedBlockRB_Impl )
    , m_casted_mb( mb_impl ? mb_impl : dynamic_cast<ManagedBlockRB_Impl*>( m_mb_impl.get()))
{}

// ����������� ������ ����������� ���� ������ �������� 'mem_size' ����, ���������� ��� � �������� ���������� 'parent_alloc'
RegionsAllocator::RegionsAllocator( size_t mem_size, RegionsAllocator *parent_alloc, bool clean_when_dealloc, ManagedBlockRB_Impl *mb_impl )
    : IAllocatorCore( mem_size, parent_alloc, clean_when_dealloc, mb_impl ? mb_impl : new ManagedBlockRB_Impl )
    , m_casted_mb( mb_impl ? mb_impl : dynamic_cast<ManagedBlockRB_Impl*>( m_mb_impl.get()))
{}

// ������������ ��������� ����������� ���� ������ � ������� � 'mem_start', �������� 'mem_size' ����, ��������� �������
RegionsAllocator::RegionsAllocator( uint8_t *mem_start, size_t mem_size, uint8_t mode, ManagedBlockRB_Impl *mb_impl )
    : IAllocatorCore( mem_start, mem_size, mode, mb_impl ? mb_impl : new ManagedBlockRB_Impl )
    , m_casted_mb( mb_impl ? mb_impl : dynamic_cast<ManagedBlockRB_Impl*>( m_mb_impl.get()))
{}


// ����������� ����� ������ �������� 'size' ����
uint8_t *RegionsAllocator::Allocate( size_t size )
{
    uint8_t *p = nullptr;

    auto err = m_casted_mb->m_reg_list->GrabRegion( size, &p );

    if( err )
    {
        auto err_rl = ErrorCast<Error_RegionsList>( err );

        if( err_rl && ( err_rl->Elem() == ERL_Type::GRAB_FROM_EMPTY_LIST || err_rl->Elem() == ERL_Type::CONSISTENT_REG_NOTFOUND ) )
        {
            err = ERR_ALLOCATOR( EA_Type::ALLOC_SIZE_TOO_BIG, size );
            throw err;
        }
        else {
            TRACE_CUSTOM_THR_ERR( err, "Can't allocate the memory block with size: " + std::to_string( size ) + " by some reasons (see trace)" );
        }
    }
    return p;
}


// ������� ����� ������ � ������� � 'start' � �������� 'size' ����
Error_BasePtr RegionsAllocator::Deallocate( void *start, size_t size )
{
    // ���������, ��� �������������� ������ ��������� � �������� ������������ �����
    if( start < ManagedBlockStart() || start >= ManagedBlockEnd()  || (uint8_t *)start + size - 1 >= ManagedBlockEnd())
    {
        return ERR_ALLOCATOR( EA_Type::DEALLOC_REGION_OUT_OF_BOUNDS, start, size, ManagedBlockStart(), ManagedBlockSize() );
    }
    if( m_clean_when_dealloc ) {
        memset( start, 0, size );
    }
    return m_casted_mb->m_reg_list->ReleaseRegion( { (uint8_t *)start, size } );
}


// ������������� �����
void ManagedBlockRB_Impl::SetupManagedBlockImpl( uint8_t *mem_start, size_t mem_size )
{
    m_start = mem_start;
    m_size = mem_size;

    try {
        m_reg_list = std::make_unique<RegionsList<uint8_t>>( 3, RegionP<uint8_t>{ m_start, m_size } );
    }
    catch( Error_BasePtr e )
    {
        TRACE_ALLOCATOR_THR_ERR( e, EA_Type::REGIONS_LIST_CREATION_ERROR, m_start, m_size );
    }
}


// ����� �����
void ManagedBlockRB_Impl::ResetManagedBlockImpl()
{
    if( m_start )
    {
        free( m_start );
    }
}


// ������ ������ ������, ����, �������� ��������� ���������
size_t ManagedBlockRB_Impl::CellSizeImpl()
{
    return 1;
}