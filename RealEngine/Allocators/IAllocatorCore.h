#pragma once

#include <stdint.h>
#include <memory>

enum class Mode : uint8_t
{
    NO_FREE_NO_CLEANUP = 1,         // Ничего не делать
    FREE_WHEN_DESTRUCT = 2,         // Релизить управляемый блок при разрушении аллокатора
    CLEAN_WHEN_DEALLOC = 4          // Заполнять нулями деаллоцируемый элемент
};
inline uint8_t operator|( Mode L, Mode R ) { return static_cast<uint8_t>( L ) | static_cast<uint8_t>( R ); }
inline    bool operator&( uint8_t val, Mode flag ) { return static_cast<bool>( val & static_cast<uint8_t>( flag ) ); }


class RegionsAllocator;
class IManagedBlock;

class IAllocatorCore
{
public:
    // Конструируется "сырой" аллокатор, использовать который можно будет только после инициализации управляемого блока одной из версий SetupManagedBlock().
    IAllocatorCore( uint8_t mode, IManagedBlock *mb_impl );

    // Конструктор создаёт управляемый блок памяти размером 'mem_size' байт
    IAllocatorCore( size_t mem_size, uint8_t mode, IManagedBlock *mb_impl );

    // Конструктор создаёт управляемый блок памяти размером 'mem_size' байт, запрашивая его у внешнего аллокатора 'parent_alloc'
    IAllocatorCore( size_t mem_size, RegionsAllocator *parent_alloc, bool clean_when_dealloc, IManagedBlock *mb_impl );

    // Конструктору передаётся управляемый блок памяти с началом в 'mem_start', размером 'mem_size' байт, созданный заранее
    IAllocatorCore( uint8_t *mem_start, size_t mem_size, uint8_t mode, IManagedBlock *mb_impl );

    // Деструктор, релизит управляемый блок памяти при определённых условиях (см. ResetManagedBlock())
    virtual ~IAllocatorCore();

    // Создать управляемый блок памяти размером 'mem_size' байт
    void SetupManagedBlock( size_t mem_size );

    // Запросить у внешнего аллокатора 'parent_alloc' управляемый блок памяти размером 'mem_size' байт
    void SetupManagedBlock( size_t cell_num, RegionsAllocator *parent_alloc );

    // Установить созданный заранее управляемый блок памяти с началом в 'mem_start', размером 'mem_size' байт
    void SetupManagedBlock( uint8_t *mem_start, size_t mem_size );

    // Релизит управляемый блок памяти
    void ResetManagedBlock();

    // Размер ячейки памяти, байт, которыми оперирует аллокатор
    size_t CellSize();

protected:
    // Указатель на начало управляемого блока памяти
    uint8_t *ManagedBlockStart();

    // Указатель на конец + 1 управляемого блока памяти
    uint8_t *ManagedBlockEnd();

    // Размер управляемого блока памяти, байт
    size_t ManagedBlockSize();

    std::unique_ptr<IManagedBlock>  m_mb_impl;                          // Управляемый блок
    RegionsAllocator                *m_parent_alloc;                    // Родительский аллокатор (опционально)
    bool                            m_clean_when_dealloc;               // Обнулять память при деаллокации
    bool                            m_free_when_destruct;               // Освобождать память управляемого блока при разрушении аллокатора
    bool                            m_managed_block_ready;              // Управляемый блок готов
};


// Управляемый блок имплементится наследниками
class IManagedBlock
{
    friend class IAllocatorCore;

protected:
    // Инициализация блока, начало в 'mem_start', размер 'mem_size' байт
    virtual void SetupManagedBlockImpl( uint8_t *mem_start, size_t mem_size ) = 0;

    // Сброс блока
    virtual void ResetManagedBlockImpl() = 0;

    // Размер ячейки памяти, байт, которыми оперирует аллокатор
    virtual size_t CellSizeImpl() = 0;

    uint8_t *m_start;                                                   // Начало управляемого блока
    size_t  m_size;                                                     // Размер управляемого блока
};