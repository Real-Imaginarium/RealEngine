#pragma once

#include "Error_Base.h"


class IRegionsAllocator
{
public:
    // ����������� ����� ������ �������� 'size' ����
    virtual uint8_t *Allocate( size_t size ) = 0;

    // ������� ����� ������ � ������� � 'start' � �������� 'size' ����
    virtual Error_BasePtr Deallocate( void *start, size_t size ) = 0;
};