#pragma once

#include "Error_Base.h"


class IRegionsAllocator
{
public:
    // «апрашивает кусок пам€ти размером 'size' байт
    virtual uint8_t *Allocate( size_t size ) = 0;

    // –елизит кусок пам€ти с началом в 'start' и размером 'size' байт
    virtual Error_BasePtr Deallocate( void *start, size_t size ) = 0;
};