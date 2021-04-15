#pragma once

#include "Error_Custom.h"
#include "TestTypes.h"
#include "LogTest.h"

#include <typeinfo>


class RegionsAllocator_Tester
{
public:
    struct All_Perf_Test_Config
    {
        size_t objects_num;
        size_t holes;
        size_t min_initial_obj_size;
        size_t max_initial_obj_size;
        size_t min_ins_size;
        size_t max_ins_size;
        std::string logFile;
    };

    friend class RegionsAllocator;

    RegionsAllocator_Tester();
    
    ~RegionsAllocator_Tester();

    // Проверяет создание управляемого аллокатором блока памяти
    void Test_ManagedBlockCreation();

    // Проверяет аллокацию максимально возможного и слишком большого блока памяти
    void Test_Allocate_Most_Possible_And_Too_Big_Block();

    // Проверяет рандомное выделение/освобождение памяти, управляемой аллокатором
    void Test_Alloc_Dealloc_Random();

    void Test_Allocator_Performance( const All_Perf_Test_Config &conf );

private:
    template<class T>
    void AllocateSomething( std::shared_ptr<RegionsAllocator> alloc, size_t size_alloc, std::vector<std::tuple<void*, size_t, int>> &allocations );

    Error_BasePtr DeallocateSomething( std::shared_ptr<RegionsAllocator> alloc, std::vector<std::tuple<void*, size_t, int>> &allocations, size_t index );

    template<class T>
    bool ValidateMemory( T *start, size_t size, T &&expected );

    bool ValidateAllocations( std::vector<std::tuple<void *, size_t, int>> &allocations );

    static const uint32_t m_mem_size;
    uint8_t *m_mem;
};


template<class T>
void RegionsAllocator_Tester::AllocateSomething( std::shared_ptr<RegionsAllocator> alloc, size_t size_alloc, std::vector<std::tuple<void *, size_t, int>> &allocations )
{
    static_assert( sizeof( T ) >= sizeof( uint8_t ), "Size of allocated element type < size of Allocator's cells type!" );
    static_assert( !( sizeof( T ) % sizeof( uint8_t ) ), "Size of allocated element type isn't multiple of Allocator's cell type size!" );

    uint8_t type;
    if( typeid( T ) == typeid( Type_1 ) )
        type = 1;
    else if( typeid( T ) == typeid( Type_2 ) )
        type = 2;
    else if( typeid( T ) == typeid( Type_3 ) )
        type = 3;
    else if( typeid( T ) == typeid( Type_4 ) )
        type = 4;
    else if( typeid( T ) == typeid( Type_5 ) )
        type = 5;
    else if( typeid( T ) == typeid( Type_6 ) )
        type = 6;
    else
        type = 0;

    size_t factor = sizeof( T ) / sizeof( uint8_t );
    T *p = (T *)alloc->Allocate( size_alloc * factor );
    if( !p ) {
        Error_BasePtr err = ERR_CUSTOM( "Allocate() returned nullptr" );
        throw err;
    }
    for( size_t i = 0; i < size_alloc; ++i ) {
        ::new( (void *)(p+i) ) T();
    }
    allocations.push_back( std::make_tuple( p, size_alloc, type ) );
}


template<class T>
bool RegionsAllocator_Tester::ValidateMemory( T *start, size_t size, T &&expected )
{
    bool ok = true;
    for( size_t i = 0; i < size; ++i )
    {
        if( !(start[i] == expected) ) {
            ok = false;
            break;
        }
    }
    return ok;
}
