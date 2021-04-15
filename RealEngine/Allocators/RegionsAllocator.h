#pragma once

#include "IRegionsAllocator.h"
#include "IAllocatorCore.h"

#include <memory>

template<class T>
class RegionsList;
class RegionsAllocator_Tester;
class ManagedBlockRB_Impl;


class RegionsAllocator : public IAllocatorCore, public IRegionsAllocator
{
public:
    friend class RegionsAllocator_Tester;

    // Конструируется "сырой" аллокатор, использовать который можно будет только после инициализации управляемого блока одной из версий SetupManagedBlock().
    RegionsAllocator( uint8_t mode, ManagedBlockRB_Impl *mb_impl = nullptr );

    // Конструктор создаёт управляемый блок памяти размером 'mem_size' байт
    RegionsAllocator( size_t mem_size, uint8_t mode, ManagedBlockRB_Impl *mb_impl = nullptr );

    // Конструктор создаёт управляемый блок памяти размером 'mem_size' байт, запрашивая его у внешнего аллокатора 'parent_alloc'
    RegionsAllocator( size_t mem_size, RegionsAllocator *parent_alloc, bool clean_when_dealloc, ManagedBlockRB_Impl *mb_impl = nullptr );

    // Конструктору передаётся управляемый блок памяти с началом в 'mem_start', размером 'mem_size' байт, созданный заранее
    RegionsAllocator( uint8_t *mem_start, size_t mem_size, uint8_t mode, ManagedBlockRB_Impl *mb_impl = nullptr );

    // Запрашивает кусок памяти размером 'size' байт
    virtual uint8_t *Allocate( size_t size ) override;

    // Релизит кусок памяти с началом в 'start' и размером 'size' байт
    virtual Error_BasePtr Deallocate( void *start, size_t size ) override;

private:
    ManagedBlockRB_Impl *m_casted_mb;                                           // Просто кастованный указатель на реализацию IManagedBlock, чтобы не кастовать каждый раз
};


// Реализация управляемого блока памяти
class ManagedBlockRB_Impl : public IManagedBlock
{
    friend class RegionsAllocator;

protected:
    // Инициализация блока
    virtual void SetupManagedBlockImpl( uint8_t *mem_start, size_t mem_size ) override;

    // Сброс блока
    virtual void ResetManagedBlockImpl() override;

    // Размер ячейки памяти, байт, которыми оперирует аллокатор
    virtual size_t CellSizeImpl() override;

private:
    std::unique_ptr<RegionsList<uint8_t>>   m_reg_list = nullptr;               // Трэйсер используемых регионов памяти
};
