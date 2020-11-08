#include "RegionsBasedAllocator_Tester.h"


const uint32_t RegionsBasedAllocator_Tester::m_mem_size = 1024;


RegionsBasedAllocator_Tester::RegionsBasedAllocator_Tester()
{
    auto err = utils::Attempt_calloc( 10, 100, m_mem_size, m_mem );         TRACE_CUSTOM_THR_ERR( err, "Can't aquire the memory for testing" );
}


RegionsBasedAllocator_Tester::~RegionsBasedAllocator_Tester()
{
    memset( m_mem, -1, m_mem_size * sizeof( uint8_t ) );
    free( m_mem );
    memset( (void *)this, -1, sizeof( RegionsBasedAllocator_Tester ) );
}


void RegionsBasedAllocator_Tester::Test_Allocator_Performance()
{
    Log::test( "Log.txt" ) << LogTest::Start{ "Test Allocator Performance", 1 };

    size_t objects_regions_num = 1000;
    size_t transactions_num = 200;
    size_t transaction_steps_num = objects_regions_num / 10;    // Каждая транзакция состоит из n-го числа поочерёдных delete[]/new[]

    std::vector<std::vector<std::tuple<size_t, size_t>>> transactions;
    for( size_t i = 0; i < transactions_num; ++i )
    {
        std::vector<std::tuple<size_t, size_t>> transaction;
        for( size_t i = 0; i < transaction_steps_num; ++i )
        {
            size_t deletion_index = utils::random_int( 0, (int)objects_regions_num - 1 );
            size_t insertion_size = utils::random_int( 1, 20 );
            std::tuple<size_t, size_t> transaction_step = std::make_tuple( deletion_index, insertion_size );
            transaction.push_back( transaction_step );
        }
        transactions.push_back( transaction );
    }
    std::vector<SomeObject *> polygon_for_custom;
    std::vector<SomeObject *> polygon_for_global;

    for( size_t i = 0; i < objects_regions_num; ++i )
    {
        int arr_size = utils::random_int( 1, 20 );
        polygon_for_custom.push_back( new SomeObject[arr_size] );
        polygon_for_global.push_back( ::new SomeObject[arr_size] );
    }
    size_t time_for_custom_operators = 0;
    size_t time_for_global_operators = 0;

    for( auto &transaction : transactions )
    {
        // Поочерёдно удаляем/создаём новые объекты на полигоне
        for( auto const &step : transaction )
        {
            size_t del_index = std::get<0>( step );
            size_t ins_size = std::get<1>( step );

            auto start = std::chrono::high_resolution_clock::now();
            delete[] polygon_for_custom[del_index];
            auto stop = std::chrono::high_resolution_clock::now();

            SomeObject *p = new SomeObject[ins_size];
            time_for_custom_operators += ( stop - start ).count();

            polygon_for_custom.erase( polygon_for_custom.begin() + del_index );
            polygon_for_custom.push_back( p );
        }
    }
    for( auto &transaction : transactions )
    {
        // Поочерёдно удаляем/создаём новые объекты на полигоне
        for( auto const &step : transaction )
        {
            size_t del_index = std::get<0>( step );
            size_t ins_size = std::get<1>( step );

            auto start = std::chrono::high_resolution_clock::now();
            ::delete[] polygon_for_global[del_index];
            auto stop = std::chrono::high_resolution_clock::now();

            SomeObject *p = ::new SomeObject[ins_size];
            time_for_global_operators += ( stop - start ).count();

            polygon_for_global.erase( polygon_for_global.begin() + del_index );
            polygon_for_global.push_back( p );
        }
    }
    std::cout << "Custom: " << time_for_custom_operators << " ns\n" << "Global: " << time_for_global_operators << " ns\n"
        << "Ration: " << (float)time_for_custom_operators / (float)time_for_global_operators << std::endl;

    Log::test( "Log.txt" ) << LogTest::Finished{ true };
}


bool RegionsBasedAllocator_Tester::ValidateAllocations( std::vector<std::tuple<void *, size_t, int>> &allocations )
{
    bool ok = true;
    for( auto const elem : allocations ) {
        switch( std::get<2>( elem ) ) {
        case 1: { ok &= ValidateMemory<Type_1>( (Type_1 *)std::get<0>( elem ), std::get<1>( elem ), Type_1() ); break; }
        case 2: { ok &= ValidateMemory<Type_2>( (Type_2 *)std::get<0>( elem ), std::get<1>( elem ), Type_2() ); break; }
        case 3: { ok &= ValidateMemory<Type_3>( (Type_3 *)std::get<0>( elem ), std::get<1>( elem ), Type_3() ); break; }
        case 4: { ok &= ValidateMemory<Type_4>( (Type_4 *)std::get<0>( elem ), std::get<1>( elem ), Type_4() ); break; }
        case 5: { ok &= ValidateMemory<Type_5>( (Type_5 *)std::get<0>( elem ), std::get<1>( elem ), Type_5() ); break; }
        case 6: { ok &= ValidateMemory<Type_6>( (Type_6 *)std::get<0>( elem ), std::get<1>( elem ), Type_6() ); break; }
        }
    }
    return ok;
}
