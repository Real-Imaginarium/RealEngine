#pragma once

#include "Error_Allocator.h"

#include "IAllocatorCore.h"
#include "IPullAllocator.h"

#include "Utilites.hpp"

template<class T>
class PullAllocator;

// Реализация управляемого блока памяти
template<class T>
class ManagedBlockPB_Impl : public IManagedBlock
{
    friend class PullAllocator<T>;

protected:
    // Инициализация блока
    virtual void SetupManagedBlockImpl( uint8_t *mem_start, size_t mem_size ) override;

    // Сброс блока
    virtual void ResetManagedBlockImpl() override;

    // Размер ячейки памяти, байт, которыми оперирует аллокатор
    virtual size_t CellSizeImpl() override { return sizeof( T ); }

private:
    T **m_pull = nullptr;
    T **m_top = nullptr;
};


template<class T>
class PullAllocator : public IAllocatorCore, public IPullAllocator<T>
{
public:
    // Конструируется "сырой" аллокатор, использовать который можно будет только после инициализации управляемого блока одной из версий SetupManagedBlock().
    PullAllocator( uint8_t mode, IManagedBlock *mb_impl = nullptr );

    // Конструктор создаёт управляемый блок памяти размером sizeof(T) * 'pull_size' байт
    PullAllocator( size_t pull_size, uint8_t mode, IManagedBlock *mb_impl = nullptr );

    // Конструктор создаёт управляемый блок памяти размером sizeof(T) * 'pull_size' байт, запрашивая его у внешнего аллокатора 'parent_alloc'
    PullAllocator( size_t pull_size, RegionsAllocator *parent_alloc, bool clean_when_dealloc, IManagedBlock *mb_impl = nullptr );

    // Конструктору передаётся управляемый блок памяти с началом в 'pull_start', размером sizeof(T) * 'pull_size' байт, созданный заранее
    PullAllocator( uint8_t *pull_start, size_t pull_size, uint8_t mode, IManagedBlock *mb_impl = nullptr );

    // Запрашивает элемент из пула
    virtual T *Allocate() override;

    // Возвращает элемент в пул
    virtual Error_BasePtr Deallocate( T *elem ) override;

private:
    ManagedBlockPB_Impl<T> *m_casted_mb;                    // Просто кастованный указатель на реализацию IManagedBlock, чтобы не кастовать каждый раз
};


// Конструируется "сырой" аллокатор, использовать который можно будет только после инициализации управляемого блока одной из версий SetupManagedBlock().
template<class T>
PullAllocator<T>::PullAllocator( uint8_t mode, IManagedBlock *mb_impl )
    : IAllocatorCore( mode, mb_impl ? mb_impl : new ManagedBlockPB_Impl<T> )
    , m_casted_mb( dynamic_cast<ManagedBlockPB_Impl<T>*>( m_mb_impl.get()))
{}

// Конструктор создаёт управляемый блок памяти размером sizeof(T) * 'pull_size' байт
template<class T>
PullAllocator<T>::PullAllocator( size_t pull_size, uint8_t mode, IManagedBlock *mb_impl )
    : IAllocatorCore( pull_size * sizeof(T), mode, mb_impl ? mb_impl : new ManagedBlockPB_Impl<T> )
    , m_casted_mb( dynamic_cast<ManagedBlockPB_Impl<T> *>( m_mb_impl.get()))
{}

// Конструктор создаёт управляемый блок памяти размером sizeof(T) * 'pull_size' байт, запрашивая его у внешнего аллокатора 'parent_alloc'
template<class T>
PullAllocator<T>::PullAllocator( size_t pull_size, RegionsAllocator *parent_alloc, bool clean_when_dealloc, IManagedBlock *mb_impl )
    : IAllocatorCore( pull_size, parent_alloc, clean_when_dealloc, mb_impl ? mb_impl : new ManagedBlockPB_Impl<T> )
    , m_casted_mb( dynamic_cast<ManagedBlockPB_Impl<T> *>( m_mb_impl.get()))
{}

// Конструктору передаётся управляемый блок памяти с началом в 'pull_start', размером sizeof(T) * 'pull_size' байт, созданный заранее
template<class T>
PullAllocator<T>::PullAllocator( uint8_t *pull_start, size_t pull_size, uint8_t mode, IManagedBlock *mb_impl )
    : IAllocatorCore( pull_start, pull_size * sizeof(T), mode, mb_impl ? mb_impl : new ManagedBlockPB_Impl<T> )
    , m_casted_mb( dynamic_cast<ManagedBlockPB_Impl<T> *>( m_mb_impl.get()))
{}


// Запрашивает элемент из пула
template<class T>
T *PullAllocator<T>::Allocate()
{
    if( m_casted_mb->m_top == m_casted_mb->m_pull )
    {
        auto err = ERR_ALLOCATOR( EA_Type::ALLOC_SIZE_TOO_BIG, 1 );
        throw err;
    }
    (m_casted_mb->m_top)--;
    T *temp = *(m_casted_mb->m_top);
    *(m_casted_mb->m_top) = nullptr;
    return temp;
}


// Возвращает элемент в пул
template<class T>
Error_BasePtr PullAllocator<T>::Deallocate( T *elem )
{
    // Если деаллоцируемый эл-т не принадлежит управляемому блоку памяти
    if( (uint8_t*)elem < ManagedBlockStart() || (uint8_t*)elem >= ManagedBlockEnd() )
    {
        return ERR_ALLOCATOR( EA_Type::DEALLOC_REGION_OUT_OF_BOUNDS, elem, 1, ManagedBlockStart(), ManagedBlockSize() );
    }

    // Если адрес деаллоцируемого эл-та не кратен адресам элементов в пуле
    if( ((uint8_t*)elem - ManagedBlockStart()) % sizeof(T))
    {
        return ERR_ALLOCATOR( EA_Type::DEALLOC_REGION_OUT_OF_BOUNDS, elem, 1, ManagedBlockStart(), ManagedBlockSize() );
    }

    // Если в пуле нет места - причина одна: один и тот же элемент когда-то был деаллоцирован дважды.
    if( (size_t)(m_casted_mb->m_top - m_casted_mb->m_pull) >= ManagedBlockSize() / sizeof(T) )
    {
        return ERR_ALLOCATOR( EA_Type::DEALLOC_REGION_OUT_OF_BOUNDS, elem, 1, ManagedBlockStart(), ManagedBlockSize() );
    }
    if( m_clean_when_dealloc )
    {
        memset( elem, 0, sizeof(T) );
    }
    *(m_casted_mb->m_top) = elem;
    (m_casted_mb->m_top)++;
    return nullptr;
}


// Инициализация блока
template<class T>
void ManagedBlockPB_Impl<T>::SetupManagedBlockImpl( uint8_t *mem_start, size_t mem_size )
{
    size_t elements_in_pull = mem_size / sizeof( T );
    m_start = mem_start;
    m_size = mem_size;

    utils::Attempt_calloc<T *>( 20, 100, elements_in_pull, m_pull );
    m_top = m_pull + elements_in_pull;
    *m_top = nullptr;

    for( size_t i = 0; i < elements_in_pull; ++i )
    {
        m_pull[i] = reinterpret_cast<T *>( mem_start + sizeof( T ) * i );
    }
}


// Сброс блока
template<class T>
void ManagedBlockPB_Impl<T>::ResetManagedBlockImpl()
{
    if( m_start )
    {
        free( m_start );
    }
}