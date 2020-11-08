#pragma once

#include "Error_Base.h"


template<class T>
class IRegionsBasedAllocator
{
public:
    virtual T *Allocate( size_t size ) = 0;

    virtual Error_BasePtr Deallocate( void *start, size_t size ) = 0;

    virtual ~IRegionsBasedAllocator() {}
};