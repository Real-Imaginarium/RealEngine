#include "IAllocatorCore.h"

#include "Error_Allocator.h"
#include "RegionsAllocator.h"
#include "Utilites.hpp"


// Конструируется "сырой" аллокатор, использовать который можно будет только после инициализации управляемого блока одной из версий SetupManagedBlock().
IAllocatorCore::IAllocatorCore( uint8_t mode, IManagedBlock *mb_impl )
    : m_mb_impl( mb_impl )
    , m_parent_alloc( nullptr )
    , m_free_when_destruct( mode & Mode::FREE_WHEN_DESTRUCT )
    , m_clean_when_dealloc( mode & Mode::CLEAN_WHEN_DEALLOC )
    , m_managed_block_ready( false )
{}


// Конструктор создаёт управляемый блок памяти размером 'mem_size' байт
IAllocatorCore::IAllocatorCore( size_t mem_size, uint8_t mode, IManagedBlock *mb_impl ) : IAllocatorCore( mode, mb_impl )
{
    SetupManagedBlock( mem_size );
}


// Конструктор создаёт управляемый блок памяти размером 'mem_size' байт, запрашивая его у внешнего аллокатора 'parent_alloc'
IAllocatorCore::IAllocatorCore( size_t mem_size, RegionsAllocator *parent_alloc, bool clean_when_dealloc, IManagedBlock *mb_impl )
    : m_mb_impl( mb_impl )
    , m_parent_alloc( parent_alloc )
    , m_free_when_destruct( false )
    , m_clean_when_dealloc( clean_when_dealloc )
    , m_managed_block_ready( false )
{
    SetupManagedBlock( mem_size, m_parent_alloc );
}


// Конструктору передаётся управляемый блок памяти с началом в 'mem_start', размером 'mem_size' байт, созданный заранее
IAllocatorCore::IAllocatorCore( uint8_t *mem_start, size_t mem_size, uint8_t mode, IManagedBlock *mb_impl ) : IAllocatorCore( mode, mb_impl )
{
    SetupManagedBlock( mem_start, mem_size );
}


// Деструктор, релизит управляемый блок памяти при определённых условиях (см. ResetManagedBlock())
IAllocatorCore::~IAllocatorCore()
{
    ResetManagedBlock();
}


// Создать управляемый блок памяти размером 'mem_size' байт
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


// Запросить у внешнего аллокатора 'parent_alloc' блок размером, достаточным для размещения 'cell_num' ячеек дочернего аллокатора. Это значит, что если
// размер ячейки 'parent_alloc' = 5 байт, а дочернего - 20 байт, то для размещения 3 таких ячеек понадобится 12 ячеек родительского аллокатора.  Отсюда
// требование: размер ячейки дочернего аллокатора должен быть равен или кратен родительскому (в примере - кратен)
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


// Установить созданный заранее управляемый блок памяти с началом в 'mem_start', размером 'mem_size' байт
void IAllocatorCore::SetupManagedBlock( uint8_t *mem_start, size_t mem_size )
{
    if( !m_managed_block_ready )
    {
        m_mb_impl->SetupManagedBlockImpl( mem_start, mem_size );

        m_managed_block_ready = true;
    }
}


// Релизит управляемый блок памяти
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


// Указатель на начало управляемого блока памяти
uint8_t* IAllocatorCore::ManagedBlockStart()
{
    return m_mb_impl->m_start;
}


// Указатель на конец + 1 управляемого блока памяти
uint8_t *IAllocatorCore::ManagedBlockEnd()
{
    return m_mb_impl->m_start + m_mb_impl->m_size;
}


// Размер управляемого блока памяти, байт
size_t IAllocatorCore::ManagedBlockSize()
{
    return m_mb_impl->m_size;
}
