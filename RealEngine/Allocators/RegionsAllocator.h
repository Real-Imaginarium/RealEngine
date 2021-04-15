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

    // �������������� "�����" ���������, ������������ ������� ����� ����� ������ ����� ������������� ������������ ����� ����� �� ������ SetupManagedBlock().
    RegionsAllocator( uint8_t mode, ManagedBlockRB_Impl *mb_impl = nullptr );

    // ����������� ������ ����������� ���� ������ �������� 'mem_size' ����
    RegionsAllocator( size_t mem_size, uint8_t mode, ManagedBlockRB_Impl *mb_impl = nullptr );

    // ����������� ������ ����������� ���� ������ �������� 'mem_size' ����, ���������� ��� � �������� ���������� 'parent_alloc'
    RegionsAllocator( size_t mem_size, RegionsAllocator *parent_alloc, bool clean_when_dealloc, ManagedBlockRB_Impl *mb_impl = nullptr );

    // ������������ ��������� ����������� ���� ������ � ������� � 'mem_start', �������� 'mem_size' ����, ��������� �������
    RegionsAllocator( uint8_t *mem_start, size_t mem_size, uint8_t mode, ManagedBlockRB_Impl *mb_impl = nullptr );

    // ����������� ����� ������ �������� 'size' ����
    virtual uint8_t *Allocate( size_t size ) override;

    // ������� ����� ������ � ������� � 'start' � �������� 'size' ����
    virtual Error_BasePtr Deallocate( void *start, size_t size ) override;

private:
    ManagedBlockRB_Impl *m_casted_mb;                                           // ������ ����������� ��������� �� ���������� IManagedBlock, ����� �� ��������� ������ ���
};


// ���������� ������������ ����� ������
class ManagedBlockRB_Impl : public IManagedBlock
{
    friend class RegionsAllocator;

protected:
    // ������������� �����
    virtual void SetupManagedBlockImpl( uint8_t *mem_start, size_t mem_size ) override;

    // ����� �����
    virtual void ResetManagedBlockImpl() override;

    // ������ ������ ������, ����, �������� ��������� ���������
    virtual size_t CellSizeImpl() override;

private:
    std::unique_ptr<RegionsList<uint8_t>>   m_reg_list = nullptr;               // ������� ������������ �������� ������
};
