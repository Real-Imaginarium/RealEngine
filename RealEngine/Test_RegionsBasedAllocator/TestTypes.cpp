#include "TestTypes.h"


RegionsAllocator SomeObject::alloc( 1000000 * sizeof( SomeObject ), Mode::FREE_WHEN_DESTRUCT | Mode::CLEAN_WHEN_DEALLOC );


void* SomeObject::operator new( size_t size )
{
    return ::new( (void *)alloc.Allocate( size ) ) SomeObject();
}

void* SomeObject::operator new[]( size_t size )
{
    return ::operator new[]( size, (void *)alloc.Allocate( size ) );
}

void SomeObject::operator delete( void *start )
{
    alloc.Deallocate( start, sizeof( SomeObject ) );
}

void SomeObject::operator delete[]( void *start )
{
    alloc.Deallocate( start, *(size_t *)start * sizeof( SomeObject ) + sizeof( size_t ) );
}