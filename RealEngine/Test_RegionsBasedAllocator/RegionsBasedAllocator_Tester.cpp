#include "RegionsBasedAllocator_Tester.h"
#include "Error_Allocator.h"
#include "LogInfo.h"
#include "Utilites.hpp"

const uint32_t RegionsAllocator_Tester::m_mem_size = 1024;


RegionsAllocator_Tester::RegionsAllocator_Tester()
{
    auto err = utils::Attempt_calloc( 10, 100, m_mem_size, m_mem );         TRACE_CUSTOM_THR_ERR( err, "Can't aquire the memory for testing" );
}


RegionsAllocator_Tester::~RegionsAllocator_Tester()
{
    memset( m_mem, -1, m_mem_size * sizeof( uint8_t ) );
    free( m_mem );
    memset( (void *)this, -1, sizeof( RegionsAllocator_Tester ) );
}


void RegionsAllocator_Tester::Test_Alloc_Dealloc_Random()
{
    Error_BasePtr err = nullptr;
    size_t mem_size = 100000;
    size_t num_transactions = 1000;
    Log::test( "Log.txt" ) << LogTest::Start{ "Test Random Allocation/Deallocation for type: " + std::string( typeid( uint8_t ).name() ), num_transactions };
    do {
        try {
            auto alloc = std::make_shared<RegionsAllocator>( mem_size, Mode::FREE_WHEN_DESTRUCT | Mode::CLEAN_WHEN_DEALLOC );
            std::vector<std::tuple<void *, size_t, int>> allocations;
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

                    uint8_t max = std::max( { type_1_num_alloc, type_2_num_alloc, type_3_num_alloc, type_4_num_alloc, type_5_num_alloc, type_6_num_alloc } );

                    // Аллоцируем и конструируем произвольное число объектов разных типов
                    for( size_t i = 0; i < max; i++ ) {
                        if( type_1_num_alloc >= i ) {
                            AllocateSomething<Type_1>( alloc, utils::random_int( 1, 10 ), allocations );
                        }
                        if( type_2_num_alloc >= i ) {
                            AllocateSomething<Type_2>( alloc, utils::random_int( 1, 10 ), allocations );
                        }
                        if( type_3_num_alloc >= i ) {
                            AllocateSomething<Type_3>( alloc, utils::random_int( 1, 10 ), allocations );
                        }
                        if( type_4_num_alloc >= i ) {
                            AllocateSomething<Type_4>( alloc, utils::random_int( 1, 10 ), allocations );
                        }
                        if( type_5_num_alloc >= i ) {
                            AllocateSomething<Type_5>( alloc, utils::random_int( 1, 10 ), allocations );
                        }
                        if( type_6_num_alloc >= i ) {
                            AllocateSomething<Type_6>( alloc, utils::random_int( 1, 10 ), allocations );
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
                    if( err = DeallocateSomething( alloc, allocations, utils::random_int( 0, (int)allocations.size() - 1 ) ) ) {
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
                if( err = DeallocateSomething( alloc, allocations, i ) ) {
                    break;
                }
            }
            TRACE_CUSTOM_PRNT_CNT( err, "Failed to deallocate the region remained" );

            // Проверяем память (все ячейки должны быть = 0)
            for( size_t i = 0; i < alloc->ManagedBlockSize(); ++i )
            {
                if( alloc->ManagedBlockStart()[i] != 0 ) {
                    err = ERR_CUSTOM( "Allocated memory has unexpected value(s) after all objects has been deallocated" );
                    err->Print();
                    break;
                }
            }
        }
        catch( Error_BasePtr e ) {
            err = e;                                                                                    TRACE_CUSTOM_PRNT_CNT( err, "Can't create allocator to test" );
        }
    } while( 0 );
    Log::test( "Log.txt" ) << LogTest::Finished{ err == nullptr };
}


void RegionsAllocator_Tester::Test_ManagedBlockCreation()
{
    Log::test( "Log.txt" ) << LogTest::Start{ "Test Managed Block Creation for type: " + std::string( typeid( uint8_t ).name() ), 1 };
    Error_BasePtr err = nullptr;
    uint8_t *mem = nullptr;
    bool ok = true;
    do {
        try {
            auto alloc = std::make_shared<RegionsAllocator>( m_mem_size, Mode::FREE_WHEN_DESTRUCT | Mode::CLEAN_WHEN_DEALLOC );
            mem = alloc->ManagedBlockStart();
            for( size_t i = 0; i < m_mem_size; ++i ) {
                ok &= mem[i] == 0x00;
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
            auto alloc = std::make_shared<RegionsAllocator>( mem, m_mem_size, Mode::FREE_WHEN_DESTRUCT | Mode::CLEAN_WHEN_DEALLOC );
            for( size_t i = 0; i < m_mem_size; ++i ) {
                ok &= mem[i] == 0x00;
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


void RegionsAllocator_Tester::Test_Allocate_Most_Possible_And_Too_Big_Block()
{
    Log::test( "Log.txt" ) << LogTest::Start{ "Test Allocate Most Possible And Too Big Block for type: " + std::string( typeid( uint8_t ).name() ), 1 };
    Error_BasePtr err = nullptr;
    size_t mem_size = 1024;
    do {
        try {
            auto alloc = std::make_shared<RegionsAllocator>( mem_size, Mode::FREE_WHEN_DESTRUCT | Mode::CLEAN_WHEN_DEALLOC );
            uint8_t *p = nullptr;
            try {
                if( !( p = alloc->Allocate( mem_size ) ) ) {
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


void RegionsAllocator_Tester::Test_Allocator_Performance( const All_Perf_Test_Config &conf )
{
    Log::test( "Log.txt" ) << LogTest::Start{ "Test Allocator Performance", 1 };

    size_t trans_num = 500;
    size_t del_num = 20;
    size_t ins_num = 20;

    size_t custom_del_time, global_del_time, custom_ins_time, global_ins_time, release_beg_adj, release_beg, release_end_adj, release_end, release_mid_L_adj, release_mid_R_adj,
        release_mid_LRadj, release_mid_Noadj, release_in_single, release_in_empty, grab_full_region, grab_part_region, grab_from_single, summ_ins_counts, summ_grab_counts;
    
    custom_del_time = global_del_time = custom_ins_time = global_ins_time = release_beg_adj = release_beg = release_end_adj = release_end = release_mid_L_adj = release_mid_R_adj =
        release_mid_LRadj = release_mid_Noadj = release_in_single = release_in_empty = grab_full_region = grab_part_region = grab_from_single = summ_ins_counts = summ_grab_counts = 0;

    for( size_t i = 0; i < trans_num; ++i )
    {
        // Создаём массив объектов с помощью кастомного и стандартного new
        std::vector<std::tuple<SomeObject *, SomeObject *>> allocated_objects;
        for( size_t i = 0; i < conf.objects_num; ++i )
        {
            int arr_size = utils::random_int( (int)conf.min_initial_obj_size, (int)conf.max_initial_obj_size );
            allocated_objects.push_back( std::make_tuple( new SomeObject[arr_size], ::new SomeObject[arr_size] ) );
        }

        // Делаем дырки в памяти, управляемой кастомной и стандартной системой аллокации
        for( size_t i = 1; i <= conf.holes; ++i )
        {
            size_t delete_index = ( ( conf.objects_num / conf.holes ) - 1 ) * i;
            delete[] std::get<0>( allocated_objects[delete_index] );
            ::delete[] std::get<1>( allocated_objects[delete_index] );
            allocated_objects.erase( allocated_objects.begin() + delete_index );
        }

        //alloc.m_reg_list->ResetStat();

        // Удаляем N объектов
        for( size_t k = 0; k < del_num; ++k )
        {
            size_t deletion_index = utils::random_int( 0, (int)(allocated_objects.size() - 1) );

            auto start = std::chrono::high_resolution_clock::now();
            delete[] std::get<0>( allocated_objects[deletion_index] );
            auto stop = std::chrono::high_resolution_clock::now();
            custom_del_time += ( stop - start ).count();

            start = std::chrono::high_resolution_clock::now();
            ::delete[] std::get<1>( allocated_objects[deletion_index] );
            stop = std::chrono::high_resolution_clock::now();
            global_del_time += ( stop - start ).count();

            allocated_objects.erase( allocated_objects.begin() + deletion_index );
            //std::cout << "Del: " << alloc.m_reg_list->m_p_list_size << std::endl;
        }

        // Создаём N объектов
        for( size_t k = 0; k < ins_num; ++k )
        {
            size_t insertion_size = utils::random_int( (int)conf.min_ins_size, (int)conf.max_ins_size );

            auto start = std::chrono::high_resolution_clock::now();
            SomeObject *custom_p = new SomeObject[insertion_size];
            auto stop = std::chrono::high_resolution_clock::now();
            custom_ins_time += ( stop - start ).count();

            start = std::chrono::high_resolution_clock::now();
            SomeObject *global_p = ::new SomeObject[insertion_size];
            stop = std::chrono::high_resolution_clock::now();
            global_ins_time += ( stop - start ).count();

            allocated_objects.push_back( std::make_tuple( custom_p, global_p ) );
            //std::cout << "Ins: " << alloc.m_reg_list->m_p_list_size << std::endl;
        }

        // Снимаем статистику
        //auto stat = alloc.m_reg_list->GetStat();
        //release_beg_adj += std::get<0>( stat ); release_beg += std::get<1>( stat ); release_end_adj += std::get<2>( stat ); release_end += std::get<3>( stat ); release_mid_LRadj += std::get<4>( stat );
        //release_mid_R_adj += std::get<5>( stat ); release_mid_L_adj += std::get<6>( stat ); release_mid_Noadj += std::get<7>( stat ); release_in_single += std::get<8>( stat );
        //release_in_empty += std::get<9>( stat ); grab_full_region += std::get<10>( stat ); grab_part_region += std::get<11>( stat ); grab_from_single += std::get<12>( stat );
        //summ_ins_counts += alloc.m_reg_list->GetCountInsSumm(); summ_grab_counts += alloc.m_reg_list->GetCountGrabsSumm();

        // Удаляем оставшиеся объекты
        for( size_t i = 0; i < allocated_objects.size(); ++i )
        {
            delete[] std::get<0>( allocated_objects[i] );
            ::delete[] std::get<1>( allocated_objects[i] );
        }
    }
    float custom_avg_ins_time =  (float)custom_ins_time / (float)( ins_num * trans_num );
    float global_avg_ins_time =  (float)global_ins_time / (float)( ins_num * trans_num );
    float custom_avg_del_time =  (float)custom_del_time / (float)( ins_num * trans_num );
    float global_avg_del_time =  (float)global_del_time / (float)( ins_num * trans_num );
    float percentage_beg_adj =   ( (float)release_beg_adj / (float)summ_ins_counts ) * 100;
    float percentage_beg =       ( (float)release_beg / (float)summ_ins_counts ) * 100;
    float percentage_end_adj =   ( (float)release_end_adj / (float)summ_ins_counts ) * 100;
    float percentage_end =       ( (float)release_end / (float)summ_ins_counts ) * 100;
    float percentage_mid_LRadj = ( (float)release_mid_LRadj / (float)summ_ins_counts ) * 100;
    float percentage_mid_R_adj = ( (float)release_mid_R_adj / (float)summ_ins_counts ) * 100;
    float percentage_mid_L_adj = ( (float)release_mid_L_adj / (float)summ_ins_counts ) * 100;
    float percentage_mid_Noadj = ( (float)release_mid_Noadj / (float)summ_ins_counts ) * 100;
    float percentage_in_single = ( (float)release_in_single / (float)summ_ins_counts ) * 100;
    float percentage_in_empty =  ( (float)release_in_empty / (float)summ_ins_counts ) * 100;
    float percentage_full_reg =  ( (float)grab_full_region / (float)summ_grab_counts ) * 100;
    float percentage_part_reg =  ( (float)grab_part_region / (float)summ_grab_counts ) * 100;
    float percentage_single =    ( (float)grab_from_single / (float)summ_grab_counts ) * 100;

    Log::info( "Log.txt" ) <<
        "Custom ins average time: " << custom_avg_ins_time << " nsec\nGlobal ins average time: " << global_avg_ins_time << " nsec\nCustom del average time: " << custom_avg_del_time << 
        " nsec\nGlobal del average time: " << global_avg_del_time << " nsec\nPercentage Begin no-adj insertions:  " << percentage_beg_adj << "\nPercentage Begin adj insertions:     " <<
        percentage_beg << "\nPercentage End no-adj insertions:    " << percentage_end_adj << "\nPercentage End adj insertions:       " << percentage_end << "\nPercentage Middle LR-adj insertions: " <<
        percentage_mid_LRadj << "\nPercentage Middle R-adj insertions:  " << percentage_mid_R_adj << "\nPercentage Middle L-adj insertions:  " << percentage_mid_L_adj <<
        "\nPercentage Middle No-adj insertions: " << percentage_mid_Noadj << "\nPercentage single list insertions:   " << percentage_in_single << "\nPercentage empty list insertions:    " << 
        percentage_in_empty << "\nPercentage Grab Full Region:         " << percentage_full_reg << "\nPercentage Grab Part Region:         " << percentage_part_reg << 
        "\nPercentage Grab from single sized:   " << percentage_single << Log::endlog{};

    Log::info( conf.logFile ) << custom_avg_ins_time << "\t" << global_avg_ins_time << "\t" << custom_avg_del_time << "\t" << global_avg_del_time << "\t" << conf.holes << Log::endlog{};

    Log::test( "Log.txt" ) << LogTest::Finished{ true };
    //system( "pause" );
}


Error_BasePtr RegionsAllocator_Tester::DeallocateSomething( std::shared_ptr<RegionsAllocator> alloc, std::vector<std::tuple<void *, size_t, int>> &allocations, size_t index )
{
    static_assert( sizeof( Type_1 ) >= sizeof( uint8_t ) || sizeof( Type_2 ) >= sizeof( uint8_t ) || sizeof( Type_3 ) >= sizeof( uint8_t ) ||
                   sizeof( Type_4 ) >= sizeof( uint8_t ) || sizeof( Type_5 ) >= sizeof( uint8_t ) || sizeof( Type_6 ) >= sizeof( uint8_t )
                   , "Size of allocated element type < size of Allocator's cells type!" );
    static_assert( !( sizeof( Type_1 ) % sizeof( uint8_t ) || sizeof( Type_2 ) % sizeof( uint8_t ) || sizeof( Type_3 ) % sizeof( uint8_t ) ||
                      sizeof( Type_4 ) % sizeof( uint8_t ) || sizeof( Type_5 ) % sizeof( uint8_t ) || sizeof( Type_6 ) % sizeof( uint8_t ) ),
                   "Size of allocated element type isn't multiple of Allocator's cell type size!" );

    if( index >= allocations.size() ) {
        return ERR_PARAM( 2, "size_t index", "< " + std::to_string( allocations.size() ), std::to_string( index ) );
    }
    auto elem = allocations[index];
    size_t factor = 0;
    switch( std::get<2>( elem ) ) {
    case 1: { factor = sizeof( Type_1 ) / sizeof( uint8_t ); break; }
    case 2: { factor = sizeof( Type_2 ) / sizeof( uint8_t ); break; }
    case 3: { factor = sizeof( Type_3 ) / sizeof( uint8_t ); break; }
    case 4: { factor = sizeof( Type_4 ) / sizeof( uint8_t ); break; }
    case 5: { factor = sizeof( Type_5 ) / sizeof( uint8_t ); break; }
    case 6: { factor = sizeof( Type_6 ) / sizeof( uint8_t ); break; }
    default: factor = 0;
    }
    allocations.erase( allocations.begin() + index );
    return alloc->Deallocate( std::get<0>( elem ), std::get<1>( elem ) * factor );
}


bool RegionsAllocator_Tester::ValidateAllocations( std::vector<std::tuple<void *, size_t, int>> &allocations )
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
