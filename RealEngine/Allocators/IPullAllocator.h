#pragma once

#include "Error_Base.h"


template<class T>
class IPullAllocator
{
public:
    // ����������� ������� �� ����
    virtual T *Allocate() = 0;

    // ���������� ������� � ���
    virtual Error_BasePtr Deallocate( T *elem ) = 0;
};