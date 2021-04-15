#pragma once

#include <stdint.h>
#include <memory>

enum class Mode : uint8_t
{
    NO_FREE_NO_CLEANUP = 1,         // ������ �� ������
    FREE_WHEN_DESTRUCT = 2,         // �������� ����������� ���� ��� ���������� ����������
    CLEAN_WHEN_DEALLOC = 4          // ��������� ������ �������������� �������
};
inline uint8_t operator|( Mode L, Mode R ) { return static_cast<uint8_t>( L ) | static_cast<uint8_t>( R ); }
inline    bool operator&( uint8_t val, Mode flag ) { return static_cast<bool>( val & static_cast<uint8_t>( flag ) ); }


class RegionsAllocator;
class IManagedBlock;

class IAllocatorCore
{
public:
    // �������������� "�����" ���������, ������������ ������� ����� ����� ������ ����� ������������� ������������ ����� ����� �� ������ SetupManagedBlock().
    IAllocatorCore( uint8_t mode, IManagedBlock *mb_impl );

    // ����������� ������ ����������� ���� ������ �������� 'mem_size' ����
    IAllocatorCore( size_t mem_size, uint8_t mode, IManagedBlock *mb_impl );

    // ����������� ������ ����������� ���� ������ �������� 'mem_size' ����, ���������� ��� � �������� ���������� 'parent_alloc'
    IAllocatorCore( size_t mem_size, RegionsAllocator *parent_alloc, bool clean_when_dealloc, IManagedBlock *mb_impl );

    // ������������ ��������� ����������� ���� ������ � ������� � 'mem_start', �������� 'mem_size' ����, ��������� �������
    IAllocatorCore( uint8_t *mem_start, size_t mem_size, uint8_t mode, IManagedBlock *mb_impl );

    // ����������, ������� ����������� ���� ������ ��� ����������� �������� (��. ResetManagedBlock())
    virtual ~IAllocatorCore();

    // ������� ����������� ���� ������ �������� 'mem_size' ����
    void SetupManagedBlock( size_t mem_size );

    // ��������� � �������� ���������� 'parent_alloc' ����������� ���� ������ �������� 'mem_size' ����
    void SetupManagedBlock( size_t cell_num, RegionsAllocator *parent_alloc );

    // ���������� ��������� ������� ����������� ���� ������ � ������� � 'mem_start', �������� 'mem_size' ����
    void SetupManagedBlock( uint8_t *mem_start, size_t mem_size );

    // ������� ����������� ���� ������
    void ResetManagedBlock();

    // ������ ������ ������, ����, �������� ��������� ���������
    size_t CellSize();

protected:
    // ��������� �� ������ ������������ ����� ������
    uint8_t *ManagedBlockStart();

    // ��������� �� ����� + 1 ������������ ����� ������
    uint8_t *ManagedBlockEnd();

    // ������ ������������ ����� ������, ����
    size_t ManagedBlockSize();

    std::unique_ptr<IManagedBlock>  m_mb_impl;                          // ����������� ����
    RegionsAllocator                *m_parent_alloc;                    // ������������ ��������� (�����������)
    bool                            m_clean_when_dealloc;               // �������� ������ ��� �����������
    bool                            m_free_when_destruct;               // ����������� ������ ������������ ����� ��� ���������� ����������
    bool                            m_managed_block_ready;              // ����������� ���� �����
};


// ����������� ���� ������������� ������������
class IManagedBlock
{
    friend class IAllocatorCore;

protected:
    // ������������� �����, ������ � 'mem_start', ������ 'mem_size' ����
    virtual void SetupManagedBlockImpl( uint8_t *mem_start, size_t mem_size ) = 0;

    // ����� �����
    virtual void ResetManagedBlockImpl() = 0;

    // ������ ������ ������, ����, �������� ��������� ���������
    virtual size_t CellSizeImpl() = 0;

    uint8_t *m_start;                                                   // ������ ������������ �����
    size_t  m_size;                                                     // ������ ������������ �����
};