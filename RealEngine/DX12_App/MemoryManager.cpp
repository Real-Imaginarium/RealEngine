#include "MemoryManager.h"
#include "RegionsAllocator.h"


RegionsAllocator MemoryManager::generic_allocator = RegionsAllocator( Mode::CLEAN_WHEN_DEALLOC | Mode::FREE_WHEN_DESTRUCT );


bool MemoryManager::Initialize()
{
    generic_allocator.SetupManagedBlock( 10 * 1024 * 1024 );                // 10ћб аллокатору общего назначени€
    return true;
}


bool MemoryManager::Deinitialize()
{
    generic_allocator.ResetManagedBlock();
    return true;
}