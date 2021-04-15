#pragma once

#include "Error_Base.h"


template<class T>
class IPullAllocator
{
public:
    // Запрашивает элемент из пула
    virtual T *Allocate() = 0;

    // Возвращает элемент в пул
    virtual Error_BasePtr Deallocate( T *elem ) = 0;
};