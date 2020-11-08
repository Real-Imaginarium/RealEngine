#pragma once

#include "RegionsBasedAllocator.hpp"
#include "TestTypes.h"
#include "LogTest.h"

#include <typeinfo>


class RegionsBasedAllocator_Tester
{
public:
    friend class RegionsBasedAllocator<uint8_t>;

    RegionsBasedAllocator_Tester();
    
    ~RegionsBasedAllocator_Tester();

    // Проверяет создание управляемого аллокатором блока памяти
    template<class T>
    void Test_ManagedBlockCreation();

    // Проверяет аллокацию максимально возможного и слишком большого блока памяти
    template<class T>
    void Test_Allocate_Most_Possible_And_Too_Big_Block();

    // Проверяет рандомное выделение/освобождение памяти, управляемой аллокатором
    template<class T>
    void Test_Alloc_Dealloc_Random();

    void Test_Allocator_Performance();

private:
    template<class T, class Cell, bool cleanup = true>
    void AllocateSomething( std::shared_ptr<RegionsBasedAllocator<Cell, cleanup>> alloc, size_t size_alloc, std::vector<std::tuple<void*, size_t, int>> &allocations );

    template<class Cell, bool cleanup = true>
    Error_BasePtr DeallocateSomething( std::shared_ptr<RegionsBasedAllocator<Cell, cleanup>> alloc, std::vector<std::tuple<void*, size_t, int>> &allocations, size_t index );

    template<class T>
    bool ValidateMemory( T *start, size_t size, T &&expected );

    bool ValidateAllocations( std::vector<std::tuple<void *, size_t, int>> &allocations );

    static const uint32_t m_mem_size;
    uint8_t *m_mem;
};


template<class T>
void RegionsBasedAllocator_Tester::Test_ManagedBlockCreation()
{
    Log::test( "Log.txt" ) << LogTest::Start{ "Test Managed Block Creation for type: " + std::string(typeid(T).name()), 1 };
    Error_BasePtr err = nullptr;
    T *mem = nullptr;
    bool ok = true;
    do {
        try {
            auto alloc = std::make_shared<RegionsBasedAllocator<T>>( m_mem_size );
            mem = alloc->m_managed_block.start;
            for( size_t i = 0; i < m_mem_size * sizeof( T ); ++i ) {
                ok &= ( (uint8_t*)mem )[i] == 0x00;
            }
            if( !ok ) {
                err = ERR_CUSTOM( "Wrong value in memory" );                                            TRACE_CUSTOM_PRNT_CNT( err, "Managed block isn't clean after creation" );
            }
        }
        catch( Error_BasePtr e ) {
            err = e;                                                                                    TRACE_CUSTOM_PRNT_CNT( err, "Can't create allocator to test" );
        }
        try {
            err = utils::Attempt_calloc( 20, 100, m_mem_size, mem );                                    TRACE_CUSTOM_PRNT_CNT( err, "Can't aquire the memory to pass it to allocator for managing" );
            auto alloc = std::make_shared<RegionsBasedAllocator<T>>( mem, m_mem_size );
            for( size_t i = 0; i < m_mem_size * sizeof( T ); ++i ) {
                ok &= ( (uint8_t *)mem )[i] == 0x00;
            }
            if( !ok ) {
                err = ERR_CUSTOM( "Wrong value in memory" );                                            TRACE_CUSTOM_PRNT_CNT( err, "Managed block isn't clean after initialization" );
            }
        }
        catch( Error_BasePtr e ) {
            err = e;                                                                                    TRACE_CUSTOM_PRNT_CNT( err, "Can't create allocator to test" );
        }
    } while( 0 );
    Log::test( "Log.txt" ) << LogTest::Finished{ err == nullptr };
}


template<class T>
void RegionsBasedAllocator_Tester::Test_Allocate_Most_Possible_And_Too_Big_Block()
{
    Log::test( "Log.txt" ) << LogTest::Start{ "Test Allocate Most Possible And Too Big Block for type: " + std::string( typeid( T ).name() ), 1 };
    Error_BasePtr err = nullptr;
    size_t mem_size = 1024;
    do {
        try {
            auto alloc = std::make_shared<RegionsBasedAllocator<T>>( mem_size );
            T *p = nullptr;
            try {
                if( !(p = alloc->Allocate( mem_size ))) {
                    err = ERR_CUSTOM( "Allocate() returned nullptr when trying to allocate most wide memory block" );
                    err->Print();
                    continue;
                }
            }
            catch( Error_BasePtr e ) {
                err = e;                                                                                TRACE_CUSTOM_PRNT_CNT( err, "Can't allocate the most wide possible memory block" );
            }
            err = alloc->Deallocate( p, mem_size );                                                     TRACE_CUSTOM_PRNT_CNT( err, "Can't deallocate the memory block to continue test" );
            try {
                alloc->Allocate( mem_size + 1 );
                err = ERR_CUSTOM( "Allocate() must throw exception when trying to allocate too big memory block" );
                err->Print();
                continue;
            }
            catch( Error_BasePtr e ) {
                auto err_al = ErrorCast<Error_Allocator>( e );
                if( !err_al || err_al->Elem() != EA_Type::ALLOC_SIZE_TOO_BIG ) {
                    err = e;
                    TRACE_CUSTOM_PRNT_CNT( e, "Allocate() must throw \"EA_Type::ALLOC_SIZE_TOO_BIG\" when trying to allocate too big block" );
                }
                continue;
            }
        }
        catch( Error_BasePtr e ) {
            err = e;                                                                                    TRACE_CUSTOM_PRNT_CNT( err, "Can't create allocator to test" );
        }
    } while( 0 );
    Log::test( "Log.txt" ) << LogTest::Finished{ err == nullptr };
}


template<class Cell>
void RegionsBasedAllocator_Tester::Test_Alloc_Dealloc_Random()
{
    Error_BasePtr err = nullptr;
    size_t mem_size = 100000;
    size_t num_transactions = 1000;
    Log::test( "Log.txt" ) << LogTest::Start{ "Test Random Allocation/Deallocation for type: " + std::string( typeid( Cell ).name() ), num_transactions };
    do {
        try {
            auto alloc = std::make_shared<RegionsBasedAllocator<Cell, true>>( mem_size );
            std::vector<std::tuple<void*, size_t, int>> allocations;
            for( size_t t = 0; t < num_transactions; ++t )
            {
                Log::test() << LogTest::Progress{};
                try {
                    // Число аллокаций для разных типов
                    uint8_t type_1_num_alloc = utils::random_int( 1, 10 );
                    uint8_t type_2_num_alloc = utils::random_int( 1, 10 );
                    uint8_t type_3_num_alloc = utils::random_int( 1, 10 );
                    uint8_t type_4_num_alloc = utils::random_int( 1, 10 );
                    uint8_t type_5_num_alloc = utils::random_int( 1, 10 );
                    uint8_t type_6_num_alloc = utils::random_int( 1, 10 );

                    uint8_t max = std::max({ type_1_num_alloc, type_2_num_alloc, type_3_num_alloc, type_4_num_alloc, type_5_num_alloc, type_6_num_alloc });
                    
                    // Аллоцируем и конструируем произвольное число объектов разных типов
                    for( size_t i = 0; i < max; i++ ) {
                        if( type_1_num_alloc >= i ) {
                            AllocateSomething<Type_1, Cell>( alloc, utils::random_int( 1, 10 ), allocations );
                        }
                        if( type_2_num_alloc >= i ) {
                            AllocateSomething<Type_2, Cell>( alloc, utils::random_int( 1, 10 ), allocations );
                        }
                        if( type_3_num_alloc >= i ) {
                            AllocateSomething<Type_3, Cell>( alloc, utils::random_int( 1, 10 ), allocations );
                        }
                        if( type_4_num_alloc >= i ) {
                            AllocateSomething<Type_4, Cell>( alloc, utils::random_int( 1, 10 ), allocations );
                        }
                        if( type_5_num_alloc >= i ) {
                            AllocateSomething<Type_5, Cell>( alloc, utils::random_int( 1, 10 ), allocations );
                        }
                        if( type_6_num_alloc >= i ) {
                            AllocateSomething<Type_6, Cell>( alloc, utils::random_int( 1, 10 ), allocations );
                        }
                    }
                }
                catch( Error_BasePtr e ) {
                    auto err_al = ErrorCast<Error_Allocator>( e );
                    if( !err_al || err_al->Elem() != EA_Type::ALLOC_SIZE_TOO_BIG ) {
                        err = e;
                        TRACE_CUSTOM_PRNT_BRK( e, "Unexpected error during allocation" );
                    }
                }
                // Проверяем сконструированные объекты
                if( !ValidateAllocations( allocations ) ) {
                    err = ERR_CUSTOM( "Allocated memory has unexpected value(s) after construction the objects" );
                    err->Print();
                    break;
                }
                // Деаллоцируем 20% произвольных объектов
                for( int64_t i = allocations.size() / 5; i > 0; --i )
                {
                    if( err = DeallocateSomething<Cell>( alloc, allocations, utils::random_int( 0, (int)allocations.size() - 1 ))) {
                        break;
                    }
                }
                TRACE_CUSTOM_PRNT_BRK( err, "Failed to deallocate random region" );
            }
            if( err ) {
                continue;
            }
            // Проверяем оставшиеся объекты
            if( !ValidateAllocations( allocations ) ) {
                err = ERR_CUSTOM( "Allocated memory has unexpected value(s) after random allocations/deallocations" );
                err->Print();
                continue;
            }
            // Деаллоцируем оставшиеся объекты
            for( int64_t i = allocations.size() - 1; i >= 0; --i )
            {
                if( err = DeallocateSomething<Cell>( alloc, allocations, i )) {
                    break;
                }
            }
            TRACE_CUSTOM_PRNT_CNT( err, "Failed to deallocate the region remained" );
            
            // Проверяем память (все ячейки должны быть = 0)
            for( size_t i = 0; i < alloc->m_managed_block.size; ++i )
            {
                if( alloc->m_managed_block.start[i] != 0 ) {
                    err = ERR_CUSTOM( "Allocated memory has unexpected value(s) after all objects has been deallocated" );
                    err->Print();
                    break;
                }
            }
        }
        catch( Error_BasePtr e ) {
            err = e;                                                                                    TRACE_CUSTOM_PRNT_CNT( err, "Can't create allocator to test" );
        }
    } while(0);
    Log::test( "Log.txt" ) << LogTest::Finished{ err == nullptr };
}

template<class T, class Cell, bool cleanup>
void RegionsBasedAllocator_Tester::AllocateSomething( std::shared_ptr<RegionsBasedAllocator<Cell, cleanup>> alloc, size_t size_alloc, std::vector<std::tuple<void *, size_t, int>> &allocations )
{
    static_assert( sizeof( T ) >= sizeof( Cell ), "Size of allocated element type < size of Allocator's cells type!" );
    static_assert( !( sizeof( T ) % sizeof( Cell ) ), "Size of allocated element type isn't multiple of Allocator's cell type size!" );

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

    size_t factor = sizeof( T ) / sizeof( Cell );
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


template<class Cell, bool cleanup>
Error_BasePtr RegionsBasedAllocator_Tester::DeallocateSomething( std::shared_ptr<RegionsBasedAllocator<Cell, cleanup>> alloc, std::vector<std::tuple<void *, size_t, int>> &allocations, size_t index )
{
    static_assert( sizeof( Type_1 ) >= sizeof( Cell ) || sizeof( Type_2 ) >= sizeof( Cell ) || sizeof( Type_3 ) >= sizeof( Cell ) ||
                   sizeof( Type_4 ) >= sizeof( Cell ) || sizeof( Type_5 ) >= sizeof( Cell ) || sizeof( Type_6 ) >= sizeof( Cell )
                   , "Size of allocated element type < size of Allocator's cells type!" );
    static_assert( !( sizeof( Type_1 ) % sizeof( Cell ) || sizeof( Type_2 ) % sizeof( Cell ) || sizeof( Type_3 ) % sizeof( Cell ) || 
                      sizeof( Type_4 ) % sizeof( Cell ) || sizeof( Type_5 ) % sizeof( Cell ) || sizeof( Type_6 ) % sizeof( Cell )),
                   "Size of allocated element type isn't multiple of Allocator's cell type size!" );

    if( index >= allocations.size() ) {
        return ERR_PARAM( 2, "size_t index", "< " + std::to_string( allocations.size() ), std::to_string( index ) );
    }
    auto elem = allocations[index];
    size_t factor = 0;
    switch( std::get<2>( elem ) ) {
        case 1: { factor = sizeof( Type_1 ) / sizeof( Cell ); break; }
        case 2: { factor = sizeof( Type_2 ) / sizeof( Cell ); break; }
        case 3: { factor = sizeof( Type_3 ) / sizeof( Cell ); break; }
        case 4: { factor = sizeof( Type_4 ) / sizeof( Cell ); break; }
        case 5: { factor = sizeof( Type_5 ) / sizeof( Cell ); break; }
        case 6: { factor = sizeof( Type_6 ) / sizeof( Cell ); break; }
        default: factor = 0;
    }
    allocations.erase( allocations.begin() + index );
    return alloc->Deallocate( std::get<0>( elem ), std::get<1>( elem ) * factor );
}


template<class T>
bool RegionsBasedAllocator_Tester::ValidateMemory( T *start, size_t size, T &&expected )
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
