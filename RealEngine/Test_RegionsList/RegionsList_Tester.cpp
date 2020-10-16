#include "RegionsList_Tester.h"
#include "Error_Custom.h"
#include "Error_RegionsList.h"
#include "Globals.h"
#include "LogInfo.h"
#include "LogError.h"
#include "LogDebug.h"
#include "LogTest.h"

#include <array>
#include <chrono>
#include <fstream>
#include <iostream>
#include <list>
#include <mutex>
#include <ppl.h>
#include <ppltasks.h>
#include <random>
#include <string>
#include <thread>
#include <time.h>
#include <tuple>
#include <typeinfo>


using InsertionsComplex = Globals::InsertionsComplex;
using ReorganizeComplex = Globals::ReorganizeComplex;
using InsertionPlace =    Globals::InsertionsComplex::InsertionPlace;
using FieldState =        Globals::InsertionsComplex::FieldState;

using namespace std::filesystem;


// Проверяет вставку уже существующего региона в RegionsList
void RegionsList_Tester::Test_DoubleInserion()
{
    Log::test( "Log.txt" ) << LogTest::Start{ "Test Existing Region Insertions", 1 };
    Error_BasePtr err = nullptr;
    size_t regList_initCap = 0;
    size_t pitch_size = 4000;
    CELL *memoryPitch = new CELL[pitch_size];
    Region_P bounds = { memoryPitch, pitch_size };

    auto regList = std::make_shared<RegionsList<CELL>>( regList_initCap );

    // Тестовые вставки
    Region_P initial_region = { memoryPitch + 100, 100 };
    Region_P second_region  = { memoryPitch + 300, 100 };

    do {
        // Проверяем RegionsList в процессе инициализации
        std::tie( err, std::ignore ) = rl_check::Check_RegList_Init( regList, regList_initCap, initial_region, bounds );    TRACE_CUSTOM_PRNT_CNT( err, "RegionsList initialization error" );

        // Пытаемся заинсертить существующий регион, следим за ошибкой
        std::string err_mess = "Error isn't detected (Trying to insert existing region)";
        auto errRegs = ErrorCast<Error_RegionsList>( regList->ReleaseRegion( initial_region ) );

        if (!errRegs || errRegs->Type() != ERL_Type::EXISTING_REG_INSERTION) {
            err = ERR_CUSTOM( err_mess );                                                                                   TRACE_CUSTOM_PRNT_CNT( err, "Wrong behaviour after initial insertion" );
        }
        // Делаем вторую вставку
        err = regList->ReleaseRegion( second_region );                                                                      TRACE_CUSTOM_PRNT_CNT( err, "Error during RegionsList second insertion" );

        // Пытаемся заинсертить существующий регион, следим за ошибкой
        errRegs = ErrorCast<Error_RegionsList>(regList->ReleaseRegion( second_region ));

        if (!errRegs || errRegs->Type() != ERL_Type::EXISTING_REG_INSERTION) {
            err = ERR_CUSTOM( err_mess );                                                                                   TRACE_CUSTOM_PRNT_CNT( err, "Wrong behaviour after second insertion" );
        }
        delete[] memoryPitch;
        Log::test( "Log.txt" ) << LogTest::Finished{ true };
        return;

    } while (0);

    delete[] memoryPitch;
    Log::test( "Log.txt" ) << LogTest::Finished{ false };
}


// Проверяет вставку региона, который перекрывается с существующим(и) регионом(ами) в PList
void RegionsList_Tester::Test_OverlappedInsertion()
{
    Log::test( "Log.txt" ) << LogTest::Start{ "Test Overlapping Region Insertions", 1 };
    Error_BasePtr err = nullptr;
    size_t regList_initCap = 0;
    size_t pitch_size = 4000;
    CELL* memoryPitch = new CELL[pitch_size];
    Region_P bounds = { memoryPitch, pitch_size };

    auto regList = std::make_shared<RegionsList<CELL>>( regList_initCap );

    // Тестовые вставки
    Region_P initial_region              = { memoryPitch + 100, 50 };
    Region_P second_region               = { memoryPitch + 200, 50 };
    Region_P left_overlapped             = { memoryPitch + 50,  51 };
    Region_P right_overlapped            = { memoryPitch + 149, 50 };
    Region_P medium_left_overlapped      = { memoryPitch + 149, 50 };
    Region_P medium_left_overlapped_adj  = { memoryPitch + 149, 51 };
    Region_P medium_right_overlapped     = { memoryPitch + 151, 50 };
    Region_P medium_right_overlapped_adj = { memoryPitch + 150, 51 };
    Region_P medium_left_right           = { memoryPitch + 149, 52 };

    do {
        // Проверяем RegionsList в процессе инициализации
        std::tie(err, std::ignore) = rl_check::Check_RegList_Init( regList, regList_initCap, initial_region, bounds );  TRACE_CUSTOM_PRNT_CNT( err, "RegionsList initialization error" );

        std::string errMess = "Error isn't detected: Trying to";

        // Пытаемся заинсертить перекрывающийся слева регион, следим за ошибкой
        auto errRegs = ErrorCast<Error_RegionsList>( regList->ReleaseRegion( left_overlapped ) );
        if (!errRegs || errRegs->Type() != ERL_Type::OVERLAPPED_REG_INSERTION)
        {
            err = ERR_CUSTOM( errMess + " insert left overlapped region)" );                                            TRACE_CUSTOM_PRNT_CNT( err, "ReleaseRegion() Wrong behavior after initial insertion" );
        }
        // Пытаемся заинсертить перекрывающийся справа регион, следим за ошибкой
        errRegs = ErrorCast<Error_RegionsList>( regList->ReleaseRegion( right_overlapped ) );
        if (!errRegs || errRegs->Type() != ERL_Type::OVERLAPPED_REG_INSERTION)
        {
            err = ERR_CUSTOM( errMess + " insert right overlapped region)" );                                           TRACE_CUSTOM_PRNT_CNT( err, "ReleaseRegion() Wrong behavior after initial insertion" );
        }
        // Делаем вторую вставку
        err = regList->ReleaseRegion( second_region );                                                                  TRACE_CUSTOM_PRNT_CNT( err, "Error during RegionsList second insertion" );
        right_overlapped = { memoryPitch + 249, 50 };

        // Пытаемся заинсертить посередине перекрывающийся слева регион
        errRegs = ErrorCast<Error_RegionsList>( regList->ReleaseRegion( medium_left_overlapped ) );
        if (!errRegs || errRegs->Type() != ERL_Type::OVERLAPPED_REG_INSERTION)
        {
            err = ERR_CUSTOM( errMess + " insert medium left overlapped region)" );                                     TRACE_CUSTOM_PRNT_CNT( err, "ReleaseRegion() Wrong behavior after second insertion" );
        }
        // Пытаемся заинсертить посередине перекрывающийся слева и смежный справа регион
        errRegs = ErrorCast<Error_RegionsList>(regList->ReleaseRegion( medium_left_overlapped_adj ));
        if (!errRegs || errRegs->Type() != ERL_Type::OVERLAPPED_REG_INSERTION)
        {
            err = ERR_CUSTOM( errMess + " insert medium left overlapped right-adjacent region)" );                      TRACE_CUSTOM_PRNT_CNT( err, "ReleaseRegion() Wrong behavior after second insertion" );
        }
        // Пытаемся заинсертить посередине перекрывающийся справа регион
        errRegs = ErrorCast<Error_RegionsList>(regList->ReleaseRegion( medium_right_overlapped ));
        if (!errRegs || errRegs->Type() != ERL_Type::OVERLAPPED_REG_INSERTION)
        {
            err = ERR_CUSTOM( errMess + " insert medium right overlapped region)" );                                    TRACE_CUSTOM_PRNT_CNT( err, "ReleaseRegion() Wrong behavior after second insertion" );
        }
        // Пытаемся заинсертить посередине перекрывающийся справа и смежный слева регион
        errRegs = ErrorCast<Error_RegionsList>(regList->ReleaseRegion( medium_right_overlapped_adj ));
        if (!errRegs || errRegs->Type() != ERL_Type::OVERLAPPED_REG_INSERTION)
        {
        	err = ERR_CUSTOM( errMess + " insert medium right overlapped left-adjacent region)" );                      TRACE_CUSTOM_PRNT_CNT( err, "ReleaseRegion() Wrong behavior after second insertion" );
        }
        // Пытаемся заинсертить посередине перекрывающийся слева и справа регион
        errRegs = ErrorCast<Error_RegionsList>(regList->ReleaseRegion( medium_left_right ));
        if (!errRegs || errRegs->Type() != ERL_Type::OVERLAPPED_REG_INSERTION)
        {
            err = ERR_CUSTOM( errMess + " insert medium right and left overlapped region)" );                           TRACE_CUSTOM_PRNT_CNT( err, "ReleaseRegion() Wrong behavior after second insertion" );
        }
        // Пытаемся заинсертить крайний левый перекрывающийся регион
        errRegs = ErrorCast<Error_RegionsList>(regList->ReleaseRegion( left_overlapped ));
        if (!errRegs || errRegs->Type() != ERL_Type::OVERLAPPED_REG_INSERTION)
        {
        	err = ERR_CUSTOM( errMess + " to insert left overlapped region)" );                                         TRACE_CUSTOM_PRNT_CNT( err, "ReleaseRegion() Wrong behavior after second insertion" );
        }
        // Пытаемся заинсертить крайний правый перекрывающийся регион
        errRegs = ErrorCast<Error_RegionsList>(regList->ReleaseRegion( right_overlapped ));
        if (!errRegs || errRegs->Type() != ERL_Type::OVERLAPPED_REG_INSERTION)
        {
            err = ERR_CUSTOM( errMess + " insert right overlapped region)" );                                           TRACE_CUSTOM_PRNT_CNT( err, "ReleaseRegion() Wrong behavior after second insertion" );
        }
        delete[] memoryPitch;
        Log::test( "Log.txt" ) << LogTest::Finished{ true };
        return;

    } while (0);

    delete[] memoryPitch;
    Log::test( "Log.txt" ) << LogTest::Finished{ false };
}


// Проверяет захват, когда RegionnsList пуст
void RegionsList_Tester::Test_GrabbingFromEmptyList()
{
    Log::test( "Log.txt" ) << LogTest::Start{ "Test Grabbing From Empty List", 1 };
    Error_BasePtr err = nullptr;
    CELL* marker = new CELL( utils::random_int( 0, 100 ) );
    CELL* grabbedReg = marker;
    auto regList = std::make_shared<RegionsList<CELL>>( 0 );

    do {
        // Проверяем корректность RegionsList после создания
        TestData td = { 0 };
        td.p_listState_resulted = { 3, 0, 1, 2, 1, 1 };
        td.s_listState_resulted = { 3, 0, 1, 2, 1, 1 };
        err = rl_check::CheckRegionsList( regList, td );                                                                TRACE_CUSTOM_PRNT_CNT( err, "RegionsList is broken after creation" );

        // Пытаемся захватить регион из пустого списка, следим за ошибкой
        auto errRegs = ErrorCast<Error_RegionsList>( regList->GrabRegion( 1, &grabbedReg ) );

        if (!errRegs || errRegs->Type() != ERL_Type::GRAB_FROM_EMPTY_LIST)
        {
            err = ERR_CUSTOM( "Error isn't detected (Trying to grab region from empty S-List)" );                       TRACE_CUSTOM_PRNT_CNT( err, "GrabRegion() Wrong behavior after RegionsList creation" );
        }
        if (grabbedReg != marker)
        {
            err = ERR_CUSTOM( "Output pointer changed during grabing the region from empty S-List" );                   TRACE_CUSTOM_PRNT_CNT( err, "GrabRegion() Wrong behavior after RegionsList creation" );
        }
        // Проверяем корректность RegionsList после попытки захвата
        err = rl_check::CheckRegionsList( regList, td );                                                                TRACE_CUSTOM_PRNT_CNT( err, "RegionsList is broken after attempt to grab" );

        delete marker;
        Log::test( "Log.txt" ) << LogTest::Finished{ true };
        return;

    } while (0);

    delete marker;
    Log::test( "Log.txt" ) << LogTest::Finished{ false };
}


// Проверяет захват региона, большего, чем есть в RegionsList
void RegionsList_Tester::Test_GrabbingTooBigRegion()
{
    Log::test( "Log.txt" ) << LogTest::Start{ "Test Grabbing Too Big Region", 1 };

    Error_BasePtr err = nullptr;
    size_t regList_initCap = 0;
    size_t pitch_size = 20000;
    CELL* memoryPitch = new CELL[pitch_size];
    Region_P bounds = { memoryPitch, pitch_size };
    CELL* marker = memoryPitch + utils::random_int( 0, (int)pitch_size - 1 );
    CELL* grabbedReg = marker;

    auto regList = std::make_shared<RegionsList<CELL>>( regList_initCap );

    Region_P initial_insertion = { memoryPitch,      10 };
    Region_P second_insertion  = { memoryPitch + 20, 11 };

    do {
        // Проверяем RegionsList в процессе инициализации
        TestData td;
        std::tie( err, td ) = rl_check::Check_RegList_Init( regList, regList_initCap, initial_insertion, bounds );      TRACE_CUSTOM_PRNT_CNT( err, "RegionsList initialization error" );

        // Пытаемся захватить слишком большой регион, следим за ошибкой
        auto errRegs = ErrorCast<Error_RegionsList>(regList->GrabRegion( 11, &grabbedReg ));

        if (!errRegs || errRegs->Type() != ERL_Type::CONSISTENT_REG_NOTFOUND)
        {
            err = ERR_CUSTOM( "Error isn't detected (Trying to grab region with size greater than S-List has)" );       TRACE_CUSTOM_PRNT_CNT( err, "GrabRegion() Wrong behavior after initial insertion" );
        }
        if (grabbedReg != marker)
        {
            err = ERR_CUSTOM( "Output pointer has been changed during attempt to grab region with too big size" );      TRACE_CUSTOM_PRNT_CNT( err, "GrabRegion() Wrong behavior after initial insertion" );
        }

        // Проверяем корректность RegionsList после попытки захвата слишком большого региона
        err = rl_check::CheckRegionsList( regList, td );                                                                TRACE_CUSTOM_PRNT_CNT( err, "RegionsList is broken after initial insertion and trying to grab too big region" );

        // Делаем вторую вставку
        err = regList->ReleaseRegion( second_insertion );                                                               TRACE_CUSTOM_PRNT_CNT( err, "Error during RegionsList second insertion" );

        // Проверяем корректность RegionsList после второй вставки
        td.p_listState_resulted = { 3, 2, 1, 0, 1, 3 };
        td.s_listState_resulted = { 3, 2, 1, 0, 1, 3 };
        td.p_listContent_resulted = { initial_insertion, second_insertion };
        td.s_listContent_resulted = utils::SListFromPList( td.p_listContent_resulted );

        err = rl_check::CheckRegionsList( regList, td );                                                                TRACE_CUSTOM_PRNT_CNT( err, "RegionsList is broken after second insertion" );

        // Пытаемся захватить слишком большой регион после второй вставки, следим за ошибкой
        errRegs = ErrorCast<Error_RegionsList>(regList->GrabRegion( 12, &grabbedReg ));
        if (!errRegs || errRegs->Type() != ERL_Type::CONSISTENT_REG_NOTFOUND)
        {
            err = ERR_CUSTOM( "Error isn't detected (Trying to grab the region with size greater than S-List has)" );   TRACE_CUSTOM_PRNT_CNT( err, "GrabRegion() Wrong behavior after second insertion" );
        }
        if (grabbedReg != marker)
        {
            err = ERR_CUSTOM( "Output pointer has been changed during attempt to grab region with too big size" );      TRACE_CUSTOM_PRNT_CNT( err, "GrabRegion() Wrong behavior after second insertion" );
        }

        // Проверяем корректность RegionsList после попытки захвата слишком большого региона
        err = rl_check::CheckRegionsList( regList, td );                                                                TRACE_CUSTOM_PRNT_CNT( err, "RegionsList is broken after second insertion and trying to grab too big region" );

        delete[] memoryPitch;
        Log::test( "Log.txt" ) << LogTest::Finished{ true };
        return;

    } while (0);

    delete[] memoryPitch;
    Log::test( "Log.txt" ) << LogTest::Finished{ false };
}


// Проверяет работу RegionsList при расширении списков (ReorganizeList)
void RegionsList_Tester::Test_ListsReorganise()
{
    Log::test( "Log.txt" ) << LogTest::Start{ "Test Lists Reorganization", 1 };

    Error_BasePtr err = nullptr;
    Region_P bounds = { ReorganizeComplex::m, ReorganizeComplex::mem_size };
    auto regList = std::make_shared<RegionsList<CELL>>( 0 );
    TestData td;

    std::vector<std::pair<TestData, bool>> states;
    do {
        // Проверяем одиночные случаи реорганизации RegionsList
        for( auto test_case : ReorganizeComplex::test_cases )
        {
            std::tie( td.p_listState_initial, td.p_listState_resulted, td.p_listContent_initial, td.p_listContent_resulted, td.intermediate_reg ) = test_case;
            td.s_listState_initial = td.p_listState_initial;
            td.s_listState_resulted = td.p_listState_resulted;
            td.s_listContent_initial = utils::SListFromPList( td.p_listContent_initial );
            td.s_listContent_resulted = utils::SListFromPList( td.p_listContent_resulted );

            // Устанавливаем RegionsList в исходное состояние, проверяем
            err = rl_manip::SetupRegionsList( regList, td );                                            TRACE_CUSTOM_BRK( err, "RegionsList setting-up error" );
            err = rl_check::CheckRegionsList( regList, td, true, true, false, false );                  TRACE_CUSTOM_BRK( err, "RegionsList is broken after setting-up" );

            // Производим реорганизующую вставку
            err = regList->ReleaseRegion( td.intermediate_reg );                                        TRACE_CUSTOM_BRK( err, "Error during reorganizing ReleaseRegion()" );

            // Проверяем реорганизованный RegionsList
            td.intermediate_reg = bounds;
            err = rl_check::CheckRegionsList( regList, td );                                            TRACE_CUSTOM_BRK( err, "RegionsList is broken after reorganization" );
        }
        TRACE_CUSTOM_CNT( err, "Single reorganization test-case failed" );


        // Проверяем последовательность реорганизаций RegionsList
        for( auto test_seq : ReorganizeComplex::test_sequences )
        {
            std::vector<std::tuple<Region_P, bool>> manipulations;
            std::tie( td.p_listState_initial, td.p_listState_resulted, manipulations, td.p_listContent_resulted ) = test_seq;
            td.s_listState_initial = td.p_listState_initial;
            td.s_listState_resulted = td.p_listState_resulted;
            td.p_listContent_initial = {};
            td.s_listContent_initial = {};
            td.s_listContent_resulted = utils::SListFromPList( td.p_listContent_resulted );
            td.intermediate_reg = bounds;

            // Устанавливаем RegionsList в исходное состояние, проверяем
            err = rl_manip::SetupRegionsList( regList, td );                                            TRACE_CUSTOM_BRK( err, "RegionsList setting-up error" );
            err = rl_check::CheckRegionsList( regList, td, true, true, true, false );                   TRACE_CUSTOM_BRK( err, "RegionsList is broken after setting-up" );

            // Манипулируем RegionsList'ом, последовательно реорганизуя его
            for( const auto manip : manipulations )
            {
                if( std::get<1>( manip ) ) {
                    err = regList->ReleaseRegion( std::get<0>( manip ) );                               TRACE_CUSTOM_BRK( err, "Error during reorganizing ReleaseRegion()" );
                    err = rl_check::CheckRegionsList( regList, td, false, false, false );               TRACE_CUSTOM_BRK( err, "RegionsList is broken after reorganizing ReleaseRegion()" );
                }
                else {
                    CELL *p_out = nullptr;
                    err = regList->GrabRegion( std::get<0>( manip ).size, &p_out );                     TRACE_CUSTOM_BRK( err, "Error during reorganizing GrabRegion()" );

                    if( p_out != std::get<0>( manip ).start ) {
                        err = ERR_CUSTOM( "Output pointer refers to unexpected memory address" );       TRACE_CUSTOM_BRK( err, "GrabRegion() Wrong behavior" );
                    }
                    err = rl_check::CheckRegionsList( regList, td, false, false, false );               TRACE_CUSTOM_BRK( err, "RegionsList is broken after reorganizing GrabRegion()" );
                }
            }
            if( err )
                break;

            // Проверяем реорганизованный RegionsList
            td.intermediate_reg = bounds;
            err = rl_check::CheckRegionsList( regList, td );                                            TRACE_CUSTOM_BRK( err, "RegionsList is broken after reorganization" );
        }
        TRACE_CUSTOM_PRNT_CNT( err, "Multiple reorganizations test-sequence failed")
    } while(0);

    Log::test( "Log.txt" ) << LogTest::Finished{ err == nullptr };
}


// Проверяет инициализирующую вставку в RegionsList (когда он только создан и ничего не содержит)
void RegionsList_Tester::Test_InitialReleaseRegion()
{
    Log::test( "Log.txt" ) << LogTest::Start{ "Test Insertions Initial" };

    size_t regList_initCap = 5;
    size_t pitch_size = 20000;
    CELL* memoryPitch = new CELL[pitch_size];
    Region_P bounds = { memoryPitch, pitch_size };

    auto regList = std::make_shared<RegionsList<CELL>>( regList_initCap );

    // Тестовые вставки
    Region_P initial_insertion = { memoryPitch, pitch_size };
    auto res = rl_check::Check_RegList_Init( regList, regList_initCap, initial_insertion, bounds );     TRACE_CUSTOM_PRNT( std::get<0>(res), "RegionsList initialization error" );

    delete[] memoryPitch;
    Log::test( "Log.txt" ) << LogTest::Finished{ std::get<0>( res ) == nullptr };
}


// Проверяет вставку в RegionsList, смежную справа, потом слева, когда он содержит 1 регион
void RegionsList_Tester::Test_SecondRelease_LeftRightAdj()
{
    Log::test( "Log.txt" ) << LogTest::Start{ "Test Second Left/Right Adjacent Insertions" };

    Error_BasePtr err = nullptr;
    size_t regList_initCap = 5;
    size_t pitch_size = 1000;
    CELL* memoryPitch = new CELL[pitch_size];
    Region_P bounds = { memoryPitch, pitch_size };

    auto regList = std::make_shared<RegionsList<CELL>>( regList_initCap );

    // Тестовые вставки
    Region_P initial_insertion   = { memoryPitch + 500, 50 };
    Region_P left_insertion_adj  = { memoryPitch + 450, 50 };
    Region_P right_insertion_adj = { memoryPitch + 550, 50 };

    do {
        // Проверяем RegionsList в процессе инициализации
        TestData td;
        std::tie( err, td ) = rl_check::Check_RegList_Init( regList, regList_initCap, initial_insertion, bounds );      TRACE_CUSTOM_PRNT_CNT( err, "RegionsList initialization error" );
        
        // Делаем смежную вставку слева
        err = regList->ReleaseRegion( left_insertion_adj );                                                             TRACE_CUSTOM_PRNT_CNT( err, "Error during RegionsList left-adjacent insertion" );

        // Проверяем корректность RegionsList после смежной вставки слева
        td.p_listContent_resulted = { { memoryPitch + 450, 100 } };
        td.s_listContent_resulted = utils::SListFromPList( td.p_listContent_resulted );
        err = rl_check::CheckRegionsList( regList, td );                                                                TRACE_CUSTOM_PRNT_CNT( err, "RegionsList is broken after left-adjacent insertion" );

        // Делаем смежную вставку справа
        err = regList->ReleaseRegion( right_insertion_adj );                                                            TRACE_CUSTOM_PRNT_CNT( err, "Error during RegionsList right-adjacent insertion" );

        // Проверяем корректность RegionsList после смежной вставки справа
        td.p_listContent_resulted = { { memoryPitch + 450, 150 } };
        td.s_listContent_resulted = utils::SListFromPList( td.p_listContent_resulted );
        err = rl_check::CheckRegionsList( regList, td );                                                                TRACE_CUSTOM_PRNT_CNT( err, "RegionsList is broken after right-adjacent insertion" );

    } while (0);

    delete[] memoryPitch;
    Log::test( "Log.txt" ) << LogTest::Finished{ err == nullptr };
}


// Проверяет вставку меньшей ширины слева в RegionsList, когда он содержит 1 регион
void RegionsList_Tester::Test_SecondRelease_Left_Less_Size()
{
    Log::test( "Log.txt" ) << LogTest::Start{ "Test Second Left Insertion With Less Size" };

    Error_BasePtr err = nullptr;
    size_t regList_initCap = 5;
    size_t pitch_size = 1000;
    CELL* memoryPitch = new CELL[pitch_size];
    Region_P bounds = { memoryPitch, pitch_size };

    auto regList = std::make_shared<RegionsList<CELL>>( regList_initCap );

    // Тестовые вставки
    Region_P initial_insertion        = { memoryPitch + 500, 50 };
    Region_P left_insertion_size_less = { memoryPitch + 450, 49 };

    do {
        // Проверяем RegionsList в процессе инициализации
        TestData td;
        std::tie( err, td ) = rl_check::Check_RegList_Init( regList, regList_initCap, initial_insertion, bounds );      TRACE_CUSTOM_PRNT_CNT( err, "RegionsList initialization error" );

        // Делаем вставку слева меньшей ширины
        err = regList->ReleaseRegion( left_insertion_size_less );                                                       TRACE_CUSTOM_PRNT_CNT( err, "Error during RegionsList left-size-less insertion" );

        // Проверяем корректность RegionsList после вставки слева меньшей ширины
        td.p_listState_resulted = { 5, 2, 1, 2, 1, 3 };
        td.s_listState_resulted = { 5, 2, 1, 2, 1, 3 };
        td.p_listContent_resulted = { left_insertion_size_less, initial_insertion };
        td.s_listContent_resulted = utils::SListFromPList( td.p_listContent_resulted );
        err = rl_check::CheckRegionsList( regList, td );                                                                TRACE_CUSTOM_PRNT_CNT( err, "RegionsList is broken after left-size-less insertion" );

    } while (0);

    delete[] memoryPitch;
    Log::test( "Log.txt" ) << LogTest::Finished{ err == nullptr };
}


// Проверяет вставку большей ширины слева в RegionsList, когда он содержит 1 регион
void RegionsList_Tester::Test_SecondRelease_Left_Great_Size()
{
    Log::test( "Log.txt" ) << LogTest::Start{ "Test Second Left Insertion With Great Size" };

    Error_BasePtr err = nullptr;
    size_t regList_initCap = 5;
    size_t pitch_size = 1000;
    CELL* memoryPitch = new CELL[pitch_size];
    Region_P bounds = { memoryPitch, pitch_size };

    auto regList = std::make_shared<RegionsList<CELL>>( regList_initCap );

    // Тестовые вставки
    Region_P initial_insertion         = { memoryPitch + 500, 50 };
    Region_P left_insertion_size_great = { memoryPitch + 448, 51 };

    do {
        // Проверяем RegionsList в процессе инициализации
        TestData td;
        std::tie( err, td ) = rl_check::Check_RegList_Init( regList, regList_initCap, initial_insertion, bounds );      TRACE_CUSTOM_PRNT_CNT( err, "RegionsList initialization error" );

        // Делаем вставку слева большей ширины
        err = regList->ReleaseRegion( left_insertion_size_great );                                                      TRACE_CUSTOM_PRNT_CNT( err, "Error during RegionsList left-size-great insertion" );

        // Проверяем корректность RegionsList после вставки слева большей ширины
        td.p_listState_resulted = { 5, 2, 1, 2, 1, 3 };
        td.s_listState_resulted = { 5, 2, 2, 1, 2, 4 };
        td.p_listContent_resulted = { left_insertion_size_great, initial_insertion };
        td.s_listContent_resulted = utils::SListFromPList( td.p_listContent_resulted );
        err = rl_check::CheckRegionsList( regList, td );                                                                TRACE_CUSTOM_PRNT_CNT( err, "RegionsList is broken after left-size-great insertion" );

    } while (0);

    delete[] memoryPitch;
    Log::test( "Log.txt" ) << LogTest::Finished{ err == nullptr };
}


// Проверяет вставку равной ширины слева в RegionsList, когда он содержит 1 регион
void RegionsList_Tester::Test_SecondRelease_Left_Equal_Size()
{
    Log::test( "Log.txt" ) << LogTest::Start{ "Test Second Left Insertion With Equal Size" };

    Error_BasePtr err = nullptr;
    size_t regList_initCap = 5;
    size_t pitch_size = 1000;
    CELL* memoryPitch = new CELL[pitch_size];
    Region_P bounds = { memoryPitch, pitch_size };

    auto regList = std::make_shared<RegionsList<CELL>>( regList_initCap );

    // Тестовые вставки
    Region_P initial_insertion =         { memoryPitch + 500, 50 };
    Region_P left_insertion_size_equal = { memoryPitch + 449, 50 };

    do {
        // Проверяем RegionsList в процессе инициализации
        TestData td;
        std::tie( err, td ) = rl_check::Check_RegList_Init( regList, regList_initCap, initial_insertion, bounds );      TRACE_CUSTOM_PRNT_CNT( err, "RegionsList initialization error" );

        // Делаем вставку слева равной ширины
        err = regList->ReleaseRegion( left_insertion_size_equal );                                                      TRACE_CUSTOM_PRNT_CNT( err, "Error during RegionsList left-size-equal insertion" );

        // Проверяем корректность RegionsList после вставки слева равной ширины
        td.p_listState_resulted = { 5, 2, 1, 2, 1, 3 };
        td.s_listState_resulted = { 5, 2, 1, 2, 1, 3 };
        td.p_listContent_resulted = { left_insertion_size_equal, initial_insertion };
        td.s_listContent_resulted = utils::SListFromPList( td.p_listContent_resulted );
        err = rl_check::CheckRegionsList( regList, td );                                                                TRACE_CUSTOM_PRNT_CNT( err, "RegionsList is broken after left-size-equal insertion" );

    } while (0);

    delete[] memoryPitch;
    Log::test( "Log.txt" ) << LogTest::Finished{ err == nullptr };
}


// Проверяет вставку меньшей ширины справа в RegionsList, когда он содержит 1 регион
void RegionsList_Tester::Test_SecondRelease_Right_Less_Size()
{
    Log::test( "Log.txt" ) << LogTest::Start{ "Test Second Right Insertion With Less Size" };

    Error_BasePtr err = nullptr;
    size_t regList_initCap = 5;
    size_t pitch_size = 1000;
    CELL* memoryPitch = new CELL[pitch_size];
    Region_P bounds = { memoryPitch, pitch_size };

    auto regList = std::make_shared<RegionsList<CELL>>( regList_initCap );

    // Тестовые вставки
    Region_P initial_insertion =         { memoryPitch + 500, 50 };
    Region_P right_insertion_size_less = { memoryPitch + 551, 49 };

    do {
        // Проверяем RegionsList в процессе инициализации
        TestData td;
        std::tie( err, td ) = rl_check::Check_RegList_Init( regList, regList_initCap, initial_insertion, bounds );      TRACE_CUSTOM_PRNT_CNT( err, "RegionsList initialization error" );

        // Делаем вставку справа меньшей ширины
        err = regList->ReleaseRegion( right_insertion_size_less );                                                      TRACE_CUSTOM_PRNT_CNT( err, "Error during RegionsList right-size-less insertion" );

        // Проверяем корректность RegionsList после вставки справа меньшей ширины
        td.p_listState_resulted = { 5, 2, 2, 1, 2, 4 };
        td.s_listState_resulted = { 5, 2, 1, 2, 1, 3 };
        td.p_listContent_resulted = { initial_insertion, right_insertion_size_less };
        td.s_listContent_resulted = utils::SListFromPList( td.p_listContent_resulted );
        err = rl_check::CheckRegionsList( regList, td );                                                                TRACE_CUSTOM_PRNT_CNT( err, "RegionsList is broken after right-size-less insertion" );

    } while (0);

    delete[] memoryPitch;
    Log::test( "Log.txt" ) << LogTest::Finished{ err == nullptr };
}


// Проверяет вставку большей ширины справа в RegionsList, когда он содержит 1 регион
void RegionsList_Tester::Test_SecondRelease_Right_Great_Size()
{
    Log::test( "Log.txt" ) << LogTest::Start{ "Test Second Right Insertion With Great Size" };

    Error_BasePtr err = nullptr;
    size_t regList_initCap = 5;
    size_t pitch_size = 1000;
    CELL* memoryPitch = new CELL[pitch_size];
    Region_P bounds = { memoryPitch, pitch_size };

    auto regList = std::make_shared<RegionsList<CELL>>( regList_initCap );

    // Тестовые вставки
    Region_P initial_insertion =          { memoryPitch + 500, 50 };
    Region_P right_insertion_size_great = { memoryPitch + 551, 51 };

    do {
        // Проверяем RegionsList в процессе инициализации
        TestData td;
        std::tie( err, td ) = rl_check::Check_RegList_Init( regList, regList_initCap, initial_insertion, bounds );      TRACE_CUSTOM_PRNT_CNT( err, "RegionsList initialization error" );

        // Делаем вставку справа большей ширины
        err = regList->ReleaseRegion( right_insertion_size_great );                                                     TRACE_CUSTOM_PRNT_CNT( err, "Error during RegionsList right-size-great insertion" );
        
        // Проверяем корректность RegionsList после вставки справа большей ширины
        td.p_listState_resulted = { 5, 2, 2, 1, 2, 4 };
        td.s_listState_resulted = { 5, 2, 2, 1, 2, 4 };
        td.p_listContent_resulted = { initial_insertion, right_insertion_size_great };
        td.s_listContent_resulted = utils::SListFromPList( td.p_listContent_resulted );
        err = rl_check::CheckRegionsList( regList, td );                                                                TRACE_CUSTOM_PRNT_CNT( err, "RegionsList is broken after right-size-great insertion" );

    } while (0);

    delete[] memoryPitch;
    Log::test( "Log.txt" ) << LogTest::Finished{ err == nullptr };
}


// Проверяет вставку равной ширины справа в RegionsList, когда он содержит 1 регион
void RegionsList_Tester::Test_SecondRelease_Right_Equal_Size()
{
    Log::test( "Log.txt" ) << LogTest::Start{ "Test Second Right Insertion With Equal Size" };

    Error_BasePtr err = nullptr;
    size_t regList_initCap = 5;
    size_t pitch_size = 1000;
    CELL* memoryPitch = new CELL[pitch_size];
    Region_P bounds = { memoryPitch, pitch_size };

    auto regList = std::make_shared<RegionsList<CELL>>( regList_initCap );

    // Тестовые вставки
    Region_P initial_insertion =          { memoryPitch + 500, 50 };
    Region_P right_insertion_size_equal = { memoryPitch + 551, 50 };

    do {
        // Проверяем RegionsList в процессе инициализации
        TestData td;
        std::tie( err, td ) = rl_check::Check_RegList_Init( regList, regList_initCap, initial_insertion, bounds );      TRACE_CUSTOM_PRNT_CNT( err, "RegionsList initialization error" );

        // Делаем вставку справа равной ширины
        err = regList->ReleaseRegion( right_insertion_size_equal );                                                     TRACE_CUSTOM_PRNT_CNT( err, "Error during RegionsList right-size-equal insertion" );

        // Проверяем корректность RegionsList после вставки справа равной ширины
        td.p_listState_resulted = { 5, 2, 2, 1, 2, 4 };
        td.s_listState_resulted = { 5, 2, 2, 1, 2, 4 };
        td.p_listContent_resulted = { initial_insertion, right_insertion_size_equal };
        td.s_listContent_resulted = utils::SListFromPList( td.p_listContent_resulted );
        err = rl_check::CheckRegionsList( regList, td );                                                                TRACE_CUSTOM_PRNT_CNT( err, "RegionsList is broken after right-size-equal insertion" );

    } while (0);

    delete[] memoryPitch;
    Log::test( "Log.txt" ) << LogTest::Finished{ err == nullptr };
}


// Проверяет захват региона, когда в RegionsList один элемент
void RegionsList_Tester::Test_GrabbingFromSingleSizedList()
{
    Log::test( "Log.txt" ) << LogTest::Start{ "Test Grabbing From Single Sized List", 1 };

    Error_BasePtr err = nullptr;
    size_t regList_initCap = 0;
    uint32_t pitch_size = 100;
    uint32_t grabb_size = 99;
    CELL *memoryPitch = new CELL[pitch_size];
    CELL *grabbedReg = nullptr;
    Region_P bounds = { memoryPitch, pitch_size };

    auto regList = std::make_shared<RegionsList<CELL>>( regList_initCap );

    // Тестовые вставки
    Region_P initial_region{ memoryPitch, pitch_size };

    do {
        // Проверяем RegionsList в процессе инициализации
        std::tie(err, std::ignore) = rl_check::Check_RegList_Init( regList, regList_initCap, initial_region, bounds );  TRACE_CUSTOM_PRNT_CNT( err, "RegionsList initialization error" );

        // Делаем захват региона максимальной ширины, возвращая RegionsList в начальное состояние
        err = regList->GrabRegion( pitch_size, &grabbedReg );                                                           TRACE_CUSTOM_PRNT_CNT( err, "Error during grabbing region with max size" );

        // Проверяем корректность RegionsList после захвата региона максимальной ширины
        TestData td = { 0 };
        td.p_listState_resulted = { 3, 0, 1, 2, 1, 1 };
        td.s_listState_resulted = { 3, 0, 1, 2, 1, 1 };
        err = rl_check::CheckRegionsList( regList, td );                                                                TRACE_CUSTOM_PRNT_CNT( err, "RegionsList is broken after grabbing region with max size" );
        
        // Проверяем указатель на захваченный регион
        if (grabbedReg != memoryPitch)
        {
            err = ERR_CUSTOM( "Broke output pointer (it should refer to start of memory managed by RegionsList)");      TRACE_CUSTOM_PRNT_CNT( err, "GrabRegion() Wrong behavior after grabbing region with max size" );
        }
        grabbedReg = nullptr;

        // RegionsList сейчас пуст. Инициализируем снова, проверяем его в процессе инициализации
        std::tie(err, std::ignore) = rl_check::Check_RegList_Init( regList, regList_initCap, initial_region, bounds );  TRACE_CUSTOM_PRNT_CNT( err, "RegionsList initialization error" );

        // Делаем захват региона НЕмаксимальной ширины
        err = regList->GrabRegion( grabb_size, &grabbedReg );                                                           TRACE_CUSTOM_PRNT_CNT( err, "Error during grabbing region with non-max size" );

        // Проверяем корректность RegionsList после захвата региона НЕмаксимальной ширины
        td.p_listState_resulted = { 3, 1, 1, 1, 1, 2 };
        td.s_listState_resulted = { 3, 1, 1, 1, 1, 2 };
        td.p_listContent_resulted = { { memoryPitch + grabb_size, 1 } };
        td.s_listContent_resulted = utils::SListFromPList( td.p_listContent_resulted );
        td.intermediate_reg = Region_P{ memoryPitch, pitch_size };
        err = rl_check::CheckRegionsList( regList, td );                                                                TRACE_CUSTOM_PRNT_CNT( err, "RegionsList is broken after grabbing region with non-max size" );

        // Проверяем указатель на захваченный регион
        if (grabbedReg != memoryPitch)
        {
            err = ERR_CUSTOM( "Broke output pointer (it should refer to start of memory managed by RegionsList)");      TRACE_CUSTOM_PRNT_CNT( err, "GrabRegion() Wrong behavior after grabbing  region with non-max size" );
        }
    } while (0);

    delete[] memoryPitch;
    Log::test( "Log.txt" ) << LogTest::Finished{ err == nullptr };
}


// Проверяет захват из RegionsList регионов различной ширины при различных исходных состояниях RegionsList
void RegionsList_Tester::Test_GrabbingsComplex()
{
    std::vector<TestData> grabbs_complex;
    GenerateGrabsComplex( grabbs_complex );

    Log::test( "Log.txt" ) << LogTest::Start{ "Test Grabbings Complex", grabbs_complex.size() };

    std::list<Error_BasePtr> errors;
    std::mutex mutex_error;

    concurrency::cancellation_token_source cts;
    concurrency::run_with_cancellation_token(
        [&cts, &grabbs_complex, &errors, &mutex_error, this]()
        {
            concurrency::parallel_for_each( grabbs_complex.begin(), grabbs_complex.end(),
                [&cts, &errors, &mutex_error, this]( TestData test_step )
                {
                    Log::test() << LogTest::Progress{};
                    Error_BasePtr err = nullptr;
                    CELL* marker = new CELL;

                    // Костыль. TestData::intermediate_reg играет разную роль: для CheckRegionsList() это Bounds, для GenerateGrabsComplex() это запрашиваемая size и ожидаемый start при вызове GrabRegion().
                    Region_P req_size_exp_start = test_step.intermediate_reg;
                    test_step.intermediate_reg = { InsertionsComplex::mem, (size_t)InsertionsComplex::Bounds::MEM_SIZE };       // Теперь в TestData::intermediate_reg записано значение Bounds 

                    std::shared_ptr<RegionsList<CELL>> tested_regList = std::make_unique<RegionsList<CELL>>( 0 );

                    do {
                        // Устанавливаем RegionsList в исходное состояние, проверяем
                        err = rl_manip::SetupRegionsList( tested_regList, test_step );                                  TRACE_CUSTOM_CNT( err, "RegionsList setting-up error in step:\n" + test_step.to_String() );
                        err = rl_check::CheckRegionsList( tested_regList, test_step, true, true, true, false );         TRACE_CUSTOM_CNT( err, "RegionsList is broken after setting-up in step:\n" + test_step.to_String() );

                        // Производим захват
                        CELL* grabbedReg = marker;
                        err = tested_regList->GrabRegion( req_size_exp_start.size, &grabbedReg );

                        // Если ожидаемый адрес захваченного региона - null, значит запрашиваемый регион слишком велик. GrabRegion() должна вернуть ошибку, а выходной указатель - остаться неизменённым
                        if (req_size_exp_start.start == nullptr) {
                            auto grabErr = ErrorCast<Error_RegionsList>( err );
                            if ( !grabErr || grabErr->Type() != ERL_Type::CONSISTENT_REG_NOTFOUND )
                            {
                                err = ERR_CUSTOM( "Incorrect condition didn't handled (Grab the region with size greater than S-List has)" );           TRACE_CUSTOM_CNT( err, "GrabRegion() Wrong behavior" );
                            }
                            if (grabbedReg != marker)
                            {
                                err = ERR_CUSTOM( "Output pointer has been changed during grabbing the region with size greater than S-List has" );     TRACE_CUSTOM_CNT( err, "GrabRegion() Wrong behavior" );
                            }
                            err.reset();
                        }
                        // Если указатель на захваченный регион не совпадает с ожидаемым
                        else if( grabbedReg != req_size_exp_start.start )
                        {
                            err = ERR_CUSTOM( "Output pointer refers to unexpected memory address" );                   TRACE_CUSTOM_CNT( err, "GrabRegion() Wrong behavior" );
                        }
                        // Проверяем RegionsList теперь
                        err = rl_check::CheckRegionsList( tested_regList, test_step );                                  TRACE_CUSTOM_CNT( err, "RegionsList is broken after grabbing in step:\n" + test_step.to_String() );

                        delete marker;
                        return;

                    } while (0);
                    {
                        std::lock_guard<std::mutex> mut( mutex_error );
                        errors.push_back( err );
                        delete marker;
                        cts.cancel();
                    }
                });
        },
        cts.get_token());

    for (auto err : errors) {
        err->Print();
    }
    Log::test( "Log.txt" ) << LogTest::Finished{ errors.empty() };
}


// Проверяет вставки в RegionsList (разной ширины, смежные и нет - во всём многообразии). Изначально RegionsList хранит 5 равноудалённых регионов разной ширины
void RegionsList_Tester::Test_InsertionsComplex()
{
    std::vector<std::tuple<std::string, TestData>> insertions;
    GenerateInsertionsComplex( &insertions );

    Log::test( "Log.txt" ) << LogTest::Start{ "Test Insertions Complex", insertions.size() };

    std::list<Error_BasePtr> errors;
    std::mutex error_write_mutex;

    concurrency::cancellation_token_source cts;
    concurrency::run_with_cancellation_token(
        [&insertions, &errors, &error_write_mutex, &cts, this]()
        {
            concurrency::parallel_for_each( insertions.begin(), insertions.end(),
                [&errors, &error_write_mutex, &cts]( std::tuple<std::string, TestData> step /*std::string step*/ )
                {
                    Log::test() << LogTest::Progress{};
                    auto regList = std::make_shared<RegionsList<CELL>>( 0 ); 
                    Error_BasePtr err = nullptr;

                    do {
                        TestData t_data = std::get<1>( step );
                        std::string mnemo = std::get<0>( step );

                        // Костыль. TestData::intermediate_reg играет разную роль: для CheckRegionsList() это Bounds, для GenerateInsertionsComplex() это Released Region при вызове ReleaseRegion().
                        Region_P released_reg = t_data.intermediate_reg;
                        t_data.intermediate_reg = { InsertionsComplex::mem, (size_t)InsertionsComplex::Bounds::MEM_SIZE };       // Теперь в TestData::intermediate_reg записано значение Bounds 

                        // Устанавливаем RegionsList в исходное состояние, проверяем
                        err = rl_manip::SetupRegionsList( regList, t_data );                                            TRACE_CUSTOM_CNT( err, "RegionsList setting-up error in step:\n" + t_data.to_String() + "\nReleased: " + utils::to_string(released_reg) + "\n" + mnemo);
                        err = rl_check::CheckRegionsList( regList, t_data, true, true, true, false );                   TRACE_CUSTOM_CNT( err, "RegionsList is broken after setting-up in step:\n" + t_data.to_String() + "\nReleased: " + utils::to_string( released_reg ) + "\n" + mnemo );

                        // Производим вставку
                        err = regList->ReleaseRegion( released_reg );                                                   TRACE_CUSTOM_CNT( err, "Error during region insertion in step:\n" + t_data.to_String() + "\nReleased: " + utils::to_string( released_reg ) + "\n" + mnemo );
                        
                        // Проверяем RegionsList теперь
                        err = rl_check::CheckRegionsList( regList, t_data );                                            TRACE_CUSTOM_CNT( err, "RegionsList is broken after insertion in step:\n" + t_data.to_String() + "\nReleased: " + utils::to_string( released_reg ) + "\n" + mnemo );
                        return;

                    } while (0);
                    {
                        std::lock_guard l( error_write_mutex );
                        errors.push_back( err );
                    }
                    cts.cancel();
                }
            );
        }, cts.get_token() );

    for (auto err : errors) {
        err->Print();
    }
    Log::test( "Log.txt" ) << LogTest::Finished{ errors.empty() };
}


// Проверяет случайно многократные захват/вставку из/в RegionsList
void RegionsList_Tester::Test_GrabbingsInsertionsRandom(const ConfigGIR &conf)
{
    size_t transactions = conf.transactions;
    size_t pitch_size = conf.mem_pitch_size;
    size_t min_grab_size = conf.min_grab_size;
    size_t max_grab_size = conf.max_grab_size;
    CELL* memoryPitch = new CELL[pitch_size];
    CELL* grabbedReg = nullptr;

    Log::test( "Log.txt" ) << LogTest::Start{ "Test Grabbings Insertions Random (Transactions: " + std::to_string( conf.transactions ) + " Memory cells: " + std::to_string(conf.mem_pitch_size) +
                                              " Min grab: " + std::to_string( conf.min_grab_size ) + " Max grab: " + std::to_string( conf.max_grab_size ) + ")", transactions };

    // Пишем в память контрольные значения
    uint8_t initial_val = utils::random_int( 0, 200 );
    for (size_t i = 0; i < pitch_size; ++i) {
        memoryPitch[i] = CELL( initial_val );
    }

    std::vector<Region_P> grabbedRegions;                                                           // Промежуточный вектор с захваченными регионами
    std::list<std::pair<std::vector<TestData>, std::vector<TestData>>> performed_transactions;
    TestData td = {};
    Error_BasePtr err;
    Error_BasePtr grab_err;

    // Создаём/инициализируем тестируемый RegionsList
    Region_P initial_reg{ memoryPitch, pitch_size };
    size_t initial_capacity = 40;

    auto regList = std::make_shared<RegionsList<CELL>>( initial_capacity );

    TestData initial_td;
    std::tie(err, initial_td) = rl_check::Check_RegList_Init( regList, initial_capacity, initial_reg, initial_reg );    TRACE_CUSTOM_PRNT_RET( err, "RegionsList initialization error" );
    
    err = rl_manip::GetRegionsListDetails( regList, td );                                                               TRACE_CUSTOM_PRNT_RET( err, "Can't get RegionsList details after initialization" );

    std::mutex mut;
    std::condition_variable cond;
    bool flag = false;
    bool stop = false;
    size_t count_grab = 0;
    size_t count_release = 0;
    std::chrono::nanoseconds release_time_ns( 0 );
    std::chrono::nanoseconds grab_time_ns( 0 );

    std::thread grab( [&, transactions]() mutable
        {
            for(size_t i = 0; i < transactions; ++i)
            {
                Log::test() << LogTest::Progress{};
                
                if (stop) { return; }   // Если release-поток сказал "stop" - выходим.

                // Делаем захват, пока GrabRegion() не вернёт ошибку, или пока она не появится при заполнении/проверке TestData
                uint8_t attempts_counter = 0;
                performed_transactions.push_back( std::make_pair<std::vector<TestData>, std::vector<TestData>>( {}, {} ) );
                do {
                    err = nullptr;
                    grab_err = nullptr;
                    grabbedReg = nullptr;
                    size_t aquired_size = utils::random_int( static_cast<int>(min_grab_size), static_cast<int>(max_grab_size) );

                    // Свапим состояния ДО и ПОСЛЕ
                    td.Swap_Initial_and_Resulted();

                    // Пытаемся захватить регион
                    auto start = std::chrono::high_resolution_clock::now();
                    grab_err = regList->GrabRegion( aquired_size, &grabbedReg );
                    auto stop = std::chrono::high_resolution_clock::now();

                    // Запоминаем результирующее состояние после попытки захвата
                    td.intermediate_reg = { grabbedReg, aquired_size };
                    err = rl_manip::GetRegionsListDetails( regList, td );                                               TRACE_CUSTOM_BRK( err, "Can't get RegionsList details after another one Grab" );
                    performed_transactions.back().first.push_back( td );

                    // Проверяем RegionsList
                    err = rl_check::CheckRegionsList( regList, td, false, false, false );                               TRACE_CUSTOM_BRK( err, "RegionsList is broken after another one Grab" );
                    err = rl_check::CheckIfContentOutOfBounds( td.p_listContent_resulted, initial_reg );                TRACE_CUSTOM_BRK( err, "Element from P-List is out of bounds after another one Grab" );
                    err = rl_check::CheckIfContentOutOfBounds( td.s_listContent_resulted, initial_reg );                TRACE_CUSTOM_BRK( err, "Element from P-List is out of bounds after another one Grab" );

                    // Если в GrabRegion() словили ошибку "Захват из пустого списка" или "Регион подходящей ширины не найден" более 5 раз, или другую ошибку - покидаем цикл
                    if (grab_err)
                    {
                        auto regListErr = ErrorCast<Error_RegionsList>( grab_err );
                        auto paramErr = ErrorCast<Error_Param>( grab_err );

                        if( paramErr && !aquired_size ) {
                            if( attempts_counter++ >= 5 )
                                break;
                            continue;
                        }
                        else if( !regListErr || ( regListErr->Type() != ERL_Type::GRAB_FROM_EMPTY_LIST && regListErr->Type() != ERL_Type::CONSISTENT_REG_NOTFOUND )) {
                            err = grab_err;
                            break;
                        }
                        if( ( regListErr->Type() == ERL_Type::GRAB_FROM_EMPTY_LIST || regListErr->Type() == ERL_Type::CONSISTENT_REG_NOTFOUND) && attempts_counter++ >= 5)
                            break;
                        continue;
                    }
                    grabbedRegions.push_back( td.intermediate_reg );

                    // Плюсуем счётчик и время успешных захватов
                    grab_time_ns += (stop - start);
                    count_grab++;

                } while (1);

                // Если возникла непредусмотренная ошибка - логируем и сигналим "stop"
                if (err)
                {
                    err->Print();
                    performed_transactions.back().first.push_back( td );
                    stop = true;
                }
                // Разблокируем release-поток, если сигналили "stop" - выходим
                flag = true;
                cond.notify_one();
                if (stop) { return; }

                std::unique_lock<std::mutex> l( mut );
                cond.wait( l, [&] {return !flag; } );
                l.unlock();
            };
        } );
    std::thread release( [&, transactions]() mutable
        {
            for (size_t i = 0; i < transactions; ++i)
            {
                std::unique_lock<std::mutex> l( mut );
                cond.wait( l, [&]() {return flag; } );
                l.unlock();
                if (stop) { return; }

                // Делаем релиз 5 случайных регионов из вектора, сформированного grab-потоком
                for (int i = 0; i < 5; ++i)
                {
                    if (grabbedRegions.empty()) {
                        //Log::warning() << "Nothing to release!" << Log::endlog{};
                        break;
                    }
                    err = nullptr;

                    // Свапим состояния ДО и ПОСЛЕ
                    td.Swap_Initial_and_Resulted();

                    // Генерим случайный индекс вектора, по нему выдираем регион из списка захваченных и релизим его
                    size_t index = utils::random_int( 0, static_cast<int>(grabbedRegions.size()) - 1 );

                    td.intermediate_reg = grabbedRegions[index];
                    grabbedRegions.erase( grabbedRegions.begin() + index );

                    auto start = std::chrono::high_resolution_clock::now();
                    err = regList->ReleaseRegion( td.intermediate_reg );                                                TRACE_CUSTOM_BRK( err, "Error during releasing the region" );
                    auto stop = std::chrono::high_resolution_clock::now();

                    // Запоминаем результирующее состояние после релиза
                    err = rl_manip::GetRegionsListDetails( regList, td );                                               TRACE_CUSTOM_BRK( err, "Can't get RegionsList details after another one Release" );
                    performed_transactions.back().second.push_back( td );

                    // Проверяем RegionsList
                    err = rl_check::CheckRegionsList( regList, td, false, false, false );                               TRACE_CUSTOM_BRK( err, "RegionsList is broken after another one Release" );
                    err = rl_check::CheckIfContentOutOfBounds( td.p_listContent_resulted, initial_reg );                TRACE_CUSTOM_BRK( err, "Element from P-List is out of bounds after another one Release" );
                    err = rl_check::CheckIfContentOutOfBounds( td.s_listContent_resulted, initial_reg );                TRACE_CUSTOM_BRK( err, "Element from P-List is out of bounds after another one Release" );

                    // Плюсуем счётчик и время успешных освобождений
                    release_time_ns += (stop - start);
                    count_release++;
                }
                // В случае любой ошибки - логируем и сигналим "stop"
                if (err)
                {
                    err->Print();
                    performed_transactions.back().second.push_back( td );
                    stop = true;
                }
                // Разблокируем grab-поток, если сигналили "stop" - выходим
                flag = false;
                cond.notify_one();
                if (stop) { return; }
            };
        } );
    grab.join();
    release.join();

    // Если Grab- или Release- завершились с непредусмотренной ошибкой - выводим в лог все транзакции и выходим
    if (err) {
        //log_transactions( performed_transactions, &Log::debug("Log.txt") );
        delete[] memoryPitch;
        Log::test( "Log.txt" ) << LogTest::Finished{ false };
        return;
    }
    do {
        td.intermediate_reg = initial_reg;
        err = rl_check::CheckRegionsList( regList, td, false, false, true );                                            TRACE_CUSTOM_PRNT_CNT( err, "RegionsList is broken after all transactions" );

        // Освобождаем оставшиеся регионы (это должно привести RegList в исходное состояние с 1 регионом)
        for (auto& reg : grabbedRegions) {
            err = regList->ReleaseRegion( reg );
            if (err) { break; }
        }
        TRACE_CUSTOM_PRNT_CNT( err, "Can't release the region is remaining in a list of grabbed regions" );

        // Проверяем RegionsList теперь - он должен был вернуться в исходное состояние (как при инициализации). State не проверяется, т.к. он изменился из-за расширений списков.
        err = rl_check::CheckRegionsList( regList, initial_td, false, true, true );     TRACE_CUSTOM_PRNT_CNT(err, "RegionsList is broken after all the regions has been released (initial state expected)" );

        // Проверяем состояние (только число регионов в списке)
        if (rl_manip::GetState<Region_P>( regList ).size != 1) {
            err = ERR_CUSTOM( "P-State.size != 1" );                                    TRACE_CUSTOM_PRNT_CNT(err, "RegionsList is broken after all the regions has been released (initial state expected)" );
        }
        if (rl_manip::GetState<Region_S>( regList ).size != 1) {
            err = ERR_CUSTOM( "S-State.size != 1" );                                    TRACE_CUSTOM_PRNT_CNT(err, "RegionsList is broken after all the regions has been released (initial state expected)" );
        }
        // Проверяем память (она должна выглядеть как вначале)
        for (size_t i = 0; i < pitch_size; ++i) {
            if (memoryPitch[i] != initial_val) {
                err = ERR_CUSTOM( "Memory has different values comparing before Grab/Release manipulations" );
                break;
            }
        }
        TRACE_CUSTOM_PRNT_CNT( err, "Memory error" );
    } while (0);

    if (err) {
        //log_transactions( performed_transactions, &Log::debug( "Log.txt" ) );
    }
    if( count_grab != 0 && count_release != 0 ) {
        unsigned long long grab_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>( grab_time_ns ).count();
        unsigned long long release_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>( release_time_ns ).count();
        Log::info( "Log.txt" ) << "Time spent for " << count_grab << " Grabbs:   " << grab_time_ms << "ms (average " << grab_time_ns.count() / count_grab << " ns)\n"
                               << "Time spent for " << count_release << " Releases: " << release_time_ms << "ms (average " << release_time_ns.count() / count_release << " ns)\n" << Log::endlog{};
    }
    Log::test( "Log.txt" ) << LogTest::Finished{ err == nullptr };
    delete[] memoryPitch;
    return;
}


std::vector<std::array<uint8_t, InsertionPlace::P5_Finish>> CreateInitialSizesSet()
{
    std::vector<std::array<uint8_t, InsertionPlace::P5_Finish>> initial_sizes;
    for (size_t i = 11111; i <= 55555; ++i)
    {
        uint8_t p1, p2, p3, p4, p5;
        p1 = static_cast<uint8_t>((i - i % 10000) / 10000);
        p2 = static_cast<uint8_t>(((i - p1 * 10000ULL) - (i - p1 * 10000ULL) % 1000ULL) / 1000ULL);
        p3 = static_cast<uint8_t>(((i - p1 * 10000ULL - p2 * 1000ULL) - (i - p1 * 10000ULL - p2 * 1000ULL) % 100ULL) / 100ULL);
        p4 = static_cast<uint8_t>(((i - p1 * 10000ULL - p2 * 1000ULL - p3 * 100ULL) - (i - p1 * 10000ULL - p2 * 1000ULL - p3 * 100ULL) % 10ULL) / 10ULL);
        p5 = static_cast<uint8_t>(((i - p1 * 10000ULL - p2 * 1000ULL - p3 * 100ULL - p4 * 10ULL)));

        if (p1 > 5 || p2 > 5 || p3 > 5 || p4 > 5 || p5 > 5 || !p1 || !p2 || !p3 || !p4 || !p5)
            continue;

        initial_sizes.push_back( { p1, p2, p3, p4, p5 } );
    }
    return initial_sizes;
}


void RegionsList_Tester::GenerateGrabsComplex( std::vector<TestData> &out_grabbs )
{
    Log::test( "Log.txt" ) << LogTest::Start{ "Generate Insertions For Test Grabbings Complex", 1 };

    // Формируем все возможные комбинации исходных Size для 5 регионов списка. Max size = 5, Min size = 1.
    std::vector<std::array<uint8_t, InsertionPlace::P5_Finish>> initial_sizes = CreateInitialSizesSet();

    std::vector<Region_P> p_list_initial( InsertionPlace::P5_Finish );	// Исходный P-список
    std::vector<Region_S> s_list_initial( 0 );	// Исходный S-список

    TestData td;
    for (size_t n = 0; n < initial_sizes.size(); ++n)
    {
        // Формируем исходный P-список
        for (int i = 0; i < InsertionPlace::P5_Finish; ++i)
        {
            Region_P reg = { InsertionsComplex::regPtrs[i], initial_sizes[n][i] };
            p_list_initial[i] = reg;
        }
        // Формируем исходный S-список на основе P-списка (сортируем эл-ты сначала по Size, затем по Pointer)
        s_list_initial = utils::SListFromPList( p_list_initial );

        // Перебор состояний LR-полей списка (L0R0, L0R1, L1R0, L1R1)
        for (const auto& f : InsertionsComplex::FieldStates)
        {
            // Перебор запросов шириной 1-5
            for (size_t i = 1; i < 6; ++i)
            {
                std::vector<Region_P> p_list_resulted( p_list_initial );
                std::vector<Region_S> s_list_resulted( s_list_initial );
                switch (f)                   /*   P-State init   ||  S-State  init   ||  P-State  rslt   ||  S-State  rslt   || P-Cont init  ||  S-Cont init  */
                {                            /* C  S  L  R  B  E || C  S  L  R  B  E || C  S  L  R  B  E || C  S  L  R  B  E ||              ||               */
                case FieldState::L0_R0: td = { {5, 5, 0, 0, 0, 5}, {5, 5, 0, 0, 0, 5}, {5, 5, 0, 0, 0, 5}, {5, 5, 0, 0, 0, 5}, p_list_initial, s_list_initial, {}, {}, {nullptr, i} }; break;
                case FieldState::L0_R1: td = { {6, 5, 0, 1, 0, 5}, {6, 5, 0, 1, 0, 5}, {6, 5, 0, 1, 0, 5}, {6, 5, 0, 1, 0, 5}, p_list_initial, s_list_initial, {}, {}, {nullptr, i} }; break;
                case FieldState::L1_R0: td = { {6, 5, 1, 0, 1, 6}, {6, 5, 1, 0, 1, 6}, {6, 5, 1, 0, 1, 6}, {6, 5, 1, 0, 1, 6}, p_list_initial, s_list_initial, {}, {}, {nullptr, i} }; break;
                case FieldState::L1_R1: td = { {7, 5, 1, 1, 1, 6}, {7, 5, 1, 1, 1, 6}, {7, 5, 1, 1, 1, 6}, {7, 5, 1, 1, 1, 6}, p_list_initial, s_list_initial, {}, {}, {nullptr, i} }; break;
                }
                size_t index = utils::lower_bound<CELL>( s_list_resulted.data(), s_list_resulted.size(), i );

                // Подходящий регион не найден?
                if (index == s_list_resulted.size()) {
                    td.p_listContent_resulted = p_list_initial;
                    td.s_listContent_resulted = s_list_initial;
                    out_grabbs.push_back( td );
                    continue;
                }
                td.intermediate_reg.start = s_list_resulted[index].start;

                // Найденный регион с таким size не единственный?
                if (s_list_resulted[index].count != 1) {
                    s_list_resulted[index + 1].count = s_list_resulted[index].count - 1;
                }
                // Найденный регион по ширине равен запрашиваемому?
                if (s_list_resulted[index].size == i)
                {
                    // Удаляем элемент из P-List
                    Side delSide_p = utils::DeleteRegion<RegionP, CELL>( p_list_resulted, { s_list_resulted[index].start, s_list_resulted[index].size } );
                    if (delSide_p == Side::Side_LEFT) {
                        td.p_listState_resulted.spaceLeft++;
                        td.p_listState_resulted.begin_pos++;
                    }
                    else if (delSide_p == Side::Side_RIGHT) {
                        td.p_listState_resulted.spaceRight++;
                        td.p_listState_resulted.end_pos--;
                    }
                    td.p_listState_resulted.size--;

                    // Удаляем элемент из S-List
                    Side delSide_s = utils::DeleteRegion<RegionP, CELL>( s_list_resulted, s_list_resulted[index] );
                    if (delSide_s == Side::Side_LEFT) {
                        td.s_listState_resulted.spaceLeft++;
                        td.s_listState_resulted.begin_pos++;
                    }
                    else if (delSide_s == Side::Side_RIGHT) {
                        td.s_listState_resulted.spaceRight++;
                        td.s_listState_resulted.end_pos--;
                    }
                    td.s_listState_resulted.size--;
                }
                else {
                    // Модифицируем элемент в P-List
                    size_t p_modify_index = utils::FindRegion<RegionP, CELL>( p_list_resulted, { s_list_resulted[index].start, s_list_resulted[index].size } );
                    p_list_resulted[p_modify_index].start += i;
                    p_list_resulted[p_modify_index].size -= i;

                    // Модифицированный элемент в S-List
                    RegionS<CELL> toInsert = { s_list_resulted[index].start + i, s_list_resulted[index].size - i, 0 };

                    // Определяем стороны удаления/вставки в S-List
                    Side delSide_s = utils::DeleteRegion<RegionP, CELL>( s_list_resulted, s_list_resulted[index] );
                    Side insSide_s = utils::Find_SList_InsertionSide<CELL>( s_list_resulted, toInsert );

                    // Получаем S-List из P-List
                    s_list_resulted = utils::SListFromPList<CELL>( p_list_resulted );

                    // В зависимости от комбинации сторон удаления/вставки получаем результирующее состояние
                    switch (f) {
                    case FieldState::L0_R0: {
                        if (delSide_s == Side::Side_LEFT && insSide_s == Side::Side_RIGHT) { td.s_listState_resulted = { 10, 5, 3, 2, 3, 8 }; }
                        if (delSide_s == Side::Side_RIGHT && insSide_s == Side::Side_LEFT) { td.s_listState_resulted = { 10, 5, 2, 3, 2, 7 }; }
                        break; }
                    case FieldState::L0_R1: {
                        if (delSide_s == Side::Side_LEFT && insSide_s == Side::Side_RIGHT) { td.s_listState_resulted = { 6, 5, 1, 0, 1, 6 }; }
                        if (delSide_s == Side::Side_RIGHT && insSide_s == Side::Side_LEFT) { td.s_listState_resulted = { 12, 5, 3, 4, 3, 8 }; }
                        break; }
                    case FieldState::L1_R0: {
                        if (delSide_s == Side::Side_LEFT && insSide_s == Side::Side_RIGHT) { td.s_listState_resulted = { 12, 5, 4, 3, 4, 9 }; }
                        if (delSide_s == Side::Side_RIGHT && insSide_s == Side::Side_LEFT) { td.s_listState_resulted = { 6, 5, 0, 1, 0, 5 }; }
                        break; }
                    case FieldState::L1_R1: {
                        if (delSide_s == Side::Side_LEFT && insSide_s == Side::Side_RIGHT) { td.s_listState_resulted = { 7, 5, 2, 0, 2, 7 }; }
                        if (delSide_s == Side::Side_RIGHT && insSide_s == Side::Side_LEFT) { td.s_listState_resulted = { 7, 5, 0, 2, 0, 5 }; }
                        break; }
                    }
                }
                td.p_listContent_resulted = p_list_resulted;
                td.s_listContent_resulted = s_list_resulted;
                out_grabbs.push_back( td );
            }
        }
    }
    Log::test( "Log.txt" ) << LogTest::Finished{ true };
}


/*__________________________________________________________________________________________________
 *      Формирует тестовый набор вставок для различных исходных состояний Pointers-списка.	Вставки |
 *  разные, в т.ч. смежные. Вычисляется соответствующее исходное  состояние Size-списка и ожидаемые |
 *  результирующие состояния Size- и Pointer-списков.                                               |
 *      Сценарий используется тестом "ComplexInsertions".                                           |
 *_________________________________________________________________________________________________*/
void RegionsList_Tester::GenerateInsertionsComplex( std::vector<std::tuple<std::string, TestData>> *out_releases )
{
    Log::test( "Log.txt" ) << LogTest::Start{ "Generate Insertions For Test Insertions Complex", 1 };

    // Формируем все возможные комбинации исходных Size для 5 регионов списка. Max size = 5, Min size = 1.
    std::vector<std::array<uint8_t, InsertionPlace::P5_Finish>> initial_sizes = CreateInitialSizesSet();

    std::mutex write_ins_mutex;

    concurrency::parallel_for_each( initial_sizes.begin(), initial_sizes.end(), [&out_releases, &write_ins_mutex]( std::array<uint8_t, InsertionPlace::P5_Finish> sizes )
        {
            std::vector<Region_P> p_list_initial( InsertionPlace::P5_Finish );  // Исходный P-список
            std::vector<Region_S> s_list_initial( 0 );                          // Исходный S-список

            // Формируем исходный P-список
            for (int i = 0; i < InsertionPlace::P5_Finish; ++i)
            {
                Region_P reg = { InsertionsComplex::regPtrs[i], sizes[i] };
                p_list_initial[i] = reg;
            }
            // Формируем исходный S-список на основе P-списка (сортируем эл-ты сначала по Size, затем по Pointer)
            s_list_initial = utils::SListFromPList( p_list_initial );

            // Формируем вставки. Участков для вставки: "regNum + 1" - в начало, в конец и между регионами списка.
            CELL* start = InsertionsComplex::mem;
            CELL* stop = InsertionsComplex::regPtrs[0];

            // Перебор регионов
            for (auto const& i : InsertionsComplex::InsertionPlaces)
            {
                std::vector<Region_P> insertions;       // Вставки на данном участке
                size_t insertPosNum = stop - start;     // Число позиций для вставки на участке

                for (size_t p = 0; p < insertPosNum; ++p) {             // Перебор всех позиций участка
                    for (size_t s = 1; s <= insertPosNum - p; ++s) {    // Перебор возможных size вставки
                        insertions.push_back( { start + p, s } );       // Генерим вставку
                    }
                }
                // Перебор состояний LR-полей списка (L0R0, L0R1, L1R0, L1R1)
                for (const auto& f : InsertionsComplex::FieldStates)
                {
                    // Перебор сгенеренных вставок, определение исходного состояния P-списка
                    for (const auto& reg : insertions)
                    {
                        std::vector<Region_P> p_list_resulted = p_list_initial;
                        std::vector<Region_S> s_list_resulted = s_list_initial;
                        TestData tdata;
                        tdata.p_listContent_initial = p_list_initial;
                        tdata.s_listContent_initial = s_list_initial;
                        tdata.intermediate_reg = reg;
                        std::string p_mnemonic, s_mnemonic;
                        
                        Region_S to_del_1, to_del_2, to_ins;
                        Region_P* left, * right;
                        left = right = nullptr;

                        if (i < InsertionPlace::P5_Finish) { right = &p_list_resulted[i]; }

                        if (i) { left = &p_list_resulted[i - 1ULL]; }

                        switch (i) {
                        // Если тестим вставки в начало (слева от первого региона в списке)
                        case InsertionPlace::Start_P1: {
                            // | #[0]   [1]   [2]   [3]   [4]  |
                            if (utils::CheckAdj( reg, *right ))
                            {
                                Side del_1_side = utils::DeleteRegion<RegionP, CELL>( s_list_resulted, utils::RegionPtoS( *right ) );
                                right->start = reg.start;
                                right->size += reg.size;
                                Side ins_side = utils::Find_SList_InsertionSide( s_list_resulted, utils::RegionPtoS( *right ) );
                                InsertionsComplex::S_ActionType combo = InsertionsComplex::SListActionDetermination( ins_side, del_1_side, Side_NONE );
                                std::tie( p_mnemonic, tdata.p_listState_initial, tdata.p_listState_resulted ) = InsertionsComplex::P_States[f][InsertionsComplex::Start_P1_Adj_R];
                                std::tie( s_mnemonic, tdata.s_listState_initial, tdata.s_listState_resulted ) = InsertionsComplex::S_States[f][combo];
                                tdata.p_listContent_resulted = p_list_resulted;
                                tdata.s_listContent_resulted = utils::SListFromPList( p_list_resulted );
                            }
                            // |# [0]   [1]   [2]   [3]   [4]  |
                            else {
                                p_list_resulted.insert( p_list_resulted.begin(), reg );
                                Side ins_side = utils::Find_SList_InsertionSide( s_list_resulted, { reg.start, reg.size, 0 } );
                                InsertionsComplex::S_ActionType combo = InsertionsComplex::SListActionDetermination( ins_side, Side_NONE, Side_NONE );
                                std::tie( p_mnemonic, tdata.p_listState_initial, tdata.p_listState_resulted ) = InsertionsComplex::P_States[f][InsertionsComplex::Start_P1_Adj_None];
                                std::tie( s_mnemonic, tdata.s_listState_initial, tdata.s_listState_resulted ) = InsertionsComplex::S_States[f][combo];
                                tdata.p_listContent_resulted = p_list_resulted;
                                tdata.s_listContent_resulted = utils::SListFromPList( p_list_resulted );
                            }
                            break;
                        }
                        // Если тестим вставки в конец (справа от последнего региона в списке)
                        case InsertionPlace::P5_Finish:
                        {
                            // |  [0]   [1]   [2]   [3]   [4]# |
                            if (utils::CheckAdj( *left, reg ))
                            {
                                Side del_1_side = utils::DeleteRegion<RegionP, CELL>( s_list_resulted, utils::RegionPtoS( *left ) );
                                left->size += reg.size;
                                Side ins_side = utils::Find_SList_InsertionSide( s_list_resulted, utils::RegionPtoS( *left ) );
                                InsertionsComplex::S_ActionType combo = InsertionsComplex::SListActionDetermination( ins_side, del_1_side, Side_NONE );
                                std::tie( p_mnemonic, tdata.p_listState_initial, tdata.p_listState_resulted ) = InsertionsComplex::P_States[f][InsertionsComplex::P5_Finish_Adj_L];
                                std::tie( s_mnemonic, tdata.s_listState_initial, tdata.s_listState_resulted ) = InsertionsComplex::S_States[f][combo];
                                tdata.p_listContent_resulted = p_list_resulted;
                                tdata.s_listContent_resulted = utils::SListFromPList( p_list_resulted );
                            }
                            // |  [0]   [1]   [2]   [3]   [4] #|
                            else {
                                p_list_resulted.insert( p_list_resulted.end(), reg );
                                Side ins_side = utils::Find_SList_InsertionSide( s_list_resulted, { reg.start, reg.size, 0 } );
                                InsertionsComplex::S_ActionType combo = InsertionsComplex::SListActionDetermination( ins_side, Side_NONE, Side_NONE );
                                std::tie( p_mnemonic, tdata.p_listState_initial, tdata.p_listState_resulted ) = InsertionsComplex::P_States[f][InsertionsComplex::P5_Finish_Adj_None];
                                std::tie( s_mnemonic, tdata.s_listState_initial, tdata.s_listState_resulted ) = InsertionsComplex::S_States[f][combo];
                                tdata.p_listContent_resulted = p_list_resulted;
                                tdata.s_listContent_resulted = utils::SListFromPList( p_list_resulted );
                            }
                            break;
                        }
                        // Если тестим вставки между регионами
                        default:
                        {
                            if (utils::CheckAdj( *left, reg, *right ) == Adjacency::Adj_Both)
                            {
                                switch (i) {
                                case InsertionPlace::P1_P2: { std::tie( p_mnemonic, tdata.p_listState_initial, tdata.p_listState_resulted ) = InsertionsComplex::P_States[f][InsertionsComplex::P1_P2_Adj_RL]; break; } // |  [0]###[1]   [2]   [3]   [4]  |
                                case InsertionPlace::P2_P3: { std::tie( p_mnemonic, tdata.p_listState_initial, tdata.p_listState_resulted ) = InsertionsComplex::P_States[f][InsertionsComplex::P2_P3_Adj_RL]; break; } // |  [0]   [1]###[2]   [3]   [4]  |
                                case InsertionPlace::P3_P4: { std::tie( p_mnemonic, tdata.p_listState_initial, tdata.p_listState_resulted ) = InsertionsComplex::P_States[f][InsertionsComplex::P3_P4_Adj_RL]; break; } // |  [0]   [1]   [2]###[3]   [4]  |
                                case InsertionPlace::P4_P5: { std::tie( p_mnemonic, tdata.p_listState_initial, tdata.p_listState_resulted ) = InsertionsComplex::P_States[f][InsertionsComplex::P4_P5_Adj_RL]; break; } // |  [0]   [1]   [2]   [3]###[4]  |
                                }
                                to_del_1 = utils::RegionPtoS( *left );
                                to_del_2 = utils::RegionPtoS( *right );
                                left->size += reg.size + right->size;
                                to_ins = utils::RegionPtoS( *left );
                                p_list_resulted.erase( p_list_resulted.begin() + i );
                                Side del_1_side = utils::DeleteRegion<RegionP, CELL>( s_list_resulted, to_del_1 );
                                Side del_2_side = utils::DeleteRegion<RegionP, CELL>( s_list_resulted, to_del_2 );
                                Side ins_side = utils::Find_SList_InsertionSide( s_list_resulted, to_ins );
                                InsertionsComplex::S_ActionType combo = InsertionsComplex::SListActionDetermination( ins_side, del_1_side, del_2_side );
                                std::tie( s_mnemonic, tdata.s_listState_initial, tdata.s_listState_resulted ) = InsertionsComplex::S_States[f][combo];
                                tdata.p_listContent_resulted = p_list_resulted;
                                tdata.s_listContent_resulted = utils::SListFromPList( p_list_resulted );
                            }
                            else if (utils::CheckAdj( *left, reg, *right ) == Adjacency::Adj_Left)
                            {
                                switch (i) {
                                case InsertionPlace::P1_P2: { std::tie( p_mnemonic, tdata.p_listState_initial, tdata.p_listState_resulted ) = InsertionsComplex::P_States[f][InsertionsComplex::P1_P2_Adj_L]; break; } // |  [0]###[1]   [2]   [3]   [4]  |
                                case InsertionPlace::P2_P3: { std::tie( p_mnemonic, tdata.p_listState_initial, tdata.p_listState_resulted ) = InsertionsComplex::P_States[f][InsertionsComplex::P2_P3_Adj_L]; break; } // |  [0]   [1]###[2]   [3]   [4]  |
                                case InsertionPlace::P3_P4: { std::tie( p_mnemonic, tdata.p_listState_initial, tdata.p_listState_resulted ) = InsertionsComplex::P_States[f][InsertionsComplex::P3_P4_Adj_L]; break; } // |  [0]   [1]   [2]###[3]   [4]  |
                                case InsertionPlace::P4_P5: { std::tie( p_mnemonic, tdata.p_listState_initial, tdata.p_listState_resulted ) = InsertionsComplex::P_States[f][InsertionsComplex::P4_P5_Adj_L]; break; } // |  [0]   [1]   [2]   [3]###[4]  |
                                }
                                Side del_1_side = utils::DeleteRegion<RegionP, CELL>( s_list_resulted, utils::RegionPtoS( *left ) );
                                left->size += reg.size;
                                Side ins_side = utils::Find_SList_InsertionSide( s_list_resulted, utils::RegionPtoS( *left ) );
                                InsertionsComplex::S_ActionType combo = InsertionsComplex::SListActionDetermination( ins_side, del_1_side, Side_NONE );
                                std::tie( s_mnemonic, tdata.s_listState_initial, tdata.s_listState_resulted ) = InsertionsComplex::S_States[f][combo];
                                tdata.p_listContent_resulted = p_list_resulted;
                                tdata.s_listContent_resulted = utils::SListFromPList( p_list_resulted );
                            }
                            else if (utils::CheckAdj( *left, reg, *right ) == Adjacency::Adj_NONE)
                            {
                                switch (i) {
                                case InsertionPlace::P1_P2: { std::tie( p_mnemonic, tdata.p_listState_initial, tdata.p_listState_resulted ) = InsertionsComplex::P_States[f][InsertionsComplex::P1_P2_Adj_None]; break; } // |  [0]###[1]   [2]   [3]   [4]  |
                                case InsertionPlace::P2_P3: { std::tie( p_mnemonic, tdata.p_listState_initial, tdata.p_listState_resulted ) = InsertionsComplex::P_States[f][InsertionsComplex::P2_P3_Adj_None]; break; } // |  [0]   [1]###[2]   [3]   [4]  |
                                case InsertionPlace::P3_P4: { std::tie( p_mnemonic, tdata.p_listState_initial, tdata.p_listState_resulted ) = InsertionsComplex::P_States[f][InsertionsComplex::P3_P4_Adj_None]; break; } // |  [0]   [1]   [2]###[3]   [4]  |
                                case InsertionPlace::P4_P5: { std::tie( p_mnemonic, tdata.p_listState_initial, tdata.p_listState_resulted ) = InsertionsComplex::P_States[f][InsertionsComplex::P4_P5_Adj_None]; break; } // |  [0]   [1]   [2]   [3]###[4]  |
                                }
                                p_list_resulted.insert( p_list_resulted.begin() + i, reg );
                                Side ins_side = utils::Find_SList_InsertionSide( s_list_resulted, utils::RegionPtoS( reg ) );
                                InsertionsComplex::S_ActionType combo = InsertionsComplex::SListActionDetermination( ins_side, Side_NONE, Side_NONE );
                                std::tie( s_mnemonic, tdata.s_listState_initial, tdata.s_listState_resulted ) = InsertionsComplex::S_States[f][combo];
                                tdata.p_listContent_resulted = p_list_resulted;
                                tdata.s_listContent_resulted = utils::SListFromPList( p_list_resulted );
                            }
                            else {
                                switch (i) {
                                case InsertionPlace::P1_P2: { std::tie( p_mnemonic, tdata.p_listState_initial, tdata.p_listState_resulted ) = InsertionsComplex::P_States[f][InsertionsComplex::P1_P2_Adj_R]; break; } // |  [0]###[1]   [2]   [3]   [4]  |
                                case InsertionPlace::P2_P3: { std::tie( p_mnemonic, tdata.p_listState_initial, tdata.p_listState_resulted ) = InsertionsComplex::P_States[f][InsertionsComplex::P2_P3_Adj_R]; break; } // |  [0]   [1]###[2]   [3]   [4]  |
                                case InsertionPlace::P3_P4: { std::tie( p_mnemonic, tdata.p_listState_initial, tdata.p_listState_resulted ) = InsertionsComplex::P_States[f][InsertionsComplex::P3_P4_Adj_R]; break; } // |  [0]   [1]   [2]###[3]   [4]  |
                                case InsertionPlace::P4_P5: { std::tie( p_mnemonic, tdata.p_listState_initial, tdata.p_listState_resulted ) = InsertionsComplex::P_States[f][InsertionsComplex::P4_P5_Adj_R]; break; } // |  [0]   [1]   [2]   [3]###[4]  |
                                }
                                Side del_1_side = utils::DeleteRegion<RegionP, CELL>( s_list_resulted, utils::RegionPtoS( *right ) );
                                right->start = reg.start;
                                right->size += reg.size;
                                Side ins_side = utils::Find_SList_InsertionSide( s_list_resulted, utils::RegionPtoS( *right ) );
                                InsertionsComplex::S_ActionType combo = InsertionsComplex::SListActionDetermination( ins_side, del_1_side, Side_NONE );

                                std::tie( s_mnemonic, tdata.s_listState_initial, tdata.s_listState_resulted ) = InsertionsComplex::S_States[f][combo];
                                tdata.p_listContent_resulted = p_list_resulted;
                                tdata.s_listContent_resulted = utils::SListFromPList( p_list_resulted );
                            }
                            break;
                        }
                        }
                        {
                            std::lock_guard l( write_ins_mutex );
                            out_releases->push_back( std::make_tuple( p_mnemonic + s_mnemonic, tdata ) );
                        }
                    }
                }
                // Двигаемся к следующему участку
                if (i == InsertionPlace::P5_Finish)
                    break;
                start = stop + p_list_initial[i].size;
                stop = InsertionsComplex::regPtrs[i + 1];
            }
            s_list_initial.clear();
        }
    );
    Log::test( "Log.txt" ) << LogTest::Finished{ true };
}


void log_transactions( std::list<std::pair<std::vector<TestData>, std::vector<TestData>>> t_list, Log *l )
{
    size_t t = 0;
    for (auto transaction : t_list)
    {
        *l << "Transaction " << std::to_string( t++ ) << "\n";
        size_t g = 0;
        for (TestData grab : transaction.first) {
            *l << "    Grab " << std::to_string( g++ ) << "\n" + grab.to_String( 8 ) << "\n";
        }
        size_t r = 0;
        for (TestData release : transaction.second) {
            *l << "    Release " << std::to_string( r++ ) << "\n" + release.to_String( 8 ) << "\n";
        }
    }
    *l << Log::endlog{};
}


template<class ListType>
inline Error_BasePtr rl_manip::SetState( const ListState& state, RegionsListPtr regList )
{
    // Проверяем, что устанавливаемое состояние - корректно
    Error_BasePtr err = nullptr;
    std::tie(err, std::ignore) = rl_check::Validate_ListState( state );                 TRACE_PARAM_RET_ERR( err, 1, "ListState &state", "State passing ListState validation", "Incorrect state" );

    if constexpr (std::is_same_v<ListType, Region_P>)
    {
        // Число регионов, ёмкость, свободное место справа и слева в P-List
        regList->m_p_list_size = state.size;
        regList->m_p_list_capacity = state.capacity;
        regList->m_p_list_spaceLeft = state.spaceLeft;
        regList->m_p_list_spaceRight = state.spaceRight;

        // Выделяем память для P-List заново, инициализируем
        if (regList->m_p_list != nullptr) {
            free( regList->m_p_list );
        }
        regList->m_p_list = reinterpret_cast<Region_P*>(calloc( state.capacity, sizeof( Region_P ) ));
        memset( regList->m_p_list, 0, state.capacity * sizeof( Region_P ) );

        // Начало и конец элементов в P-List
        regList->m_p_list_begin = regList->m_p_list + state.spaceLeft;
        regList->m_p_list_end = regList->m_p_list_begin + state.size;
    }
    else if constexpr (std::is_same_v<ListType, Region_S>)
    {
        // Число регионов, ёмкость, свободное место справа и слева в S-List
        regList->m_s_list_size = state.size;
        regList->m_s_list_capacity = state.capacity;
        regList->m_s_list_spaceLeft = state.spaceLeft;
        regList->m_s_list_spaceRight = state.spaceRight;

        // Выделяем память для S-List заново, инициализируем
        if (regList->m_s_list != nullptr) {
            free( regList->m_s_list );
        }
        regList->m_s_list = reinterpret_cast<Region_S*>(calloc( state.capacity, sizeof( Region_S ) ));
        memset( regList->m_s_list, 0, state.capacity * sizeof( Region_S ) );

        // Начало и конец элементов в S-List
        regList->m_s_list_begin = regList->m_s_list + state.spaceLeft;
        regList->m_s_list_end = regList->m_s_list_begin + state.size;
    }
    else {
        return ERR_CUSTOM( "Undefined ListType received: " + std::string( typeid( ListType ).name() ) );
    }
    return nullptr;
}


template<class ListType>
inline Error_BasePtr rl_manip::SetContent( const std::vector<ListType>& content, RegionsListPtr regList )
{
    ListType* it;
    ListType* end;

    if constexpr (std::is_same_v<ListType, Region_P>) {
        it = regList->m_p_list_begin;
        end = regList->m_p_list_end;
    }
    else if constexpr (std::is_same_v<ListType, Region_S>) {
        it = regList->m_s_list_begin;
        end = regList->m_s_list_end;
    }
    else {
        return ERR_CUSTOM( "Undefined ListType received: " + std::string( typeid( ListType ).name() ) );
    }
    if (it > end) {
        return ERR_CUSTOM( "List's iterators are in incorrect state (begin > end)" );
    }
    if (content.size() != end - it) {
        return ERR_CUSTOM( "Size of vector to be set must match with List size is set.\nSize of vector: " + std::to_string( content.size() ) + "\nList size is: " + std::to_string( end - it ) );
    }
    size_t pos = 0;
    for (; it != end; ++it, ++pos) {
        *it = content[pos];
    }
    return nullptr;
}


template<class ListType>
inline ListState rl_manip::GetState( RegionsListPtr regList )
{
    if constexpr (std::is_same_v<ListType, Region_P>) {
        return
        {
            regList->m_p_list_capacity,
            regList->m_p_list_size,
            regList->m_p_list_spaceLeft,
            regList->m_p_list_spaceRight,
            regList->m_p_list_spaceLeft,                                // begin pos (not address!)
            regList->m_p_list_spaceLeft + regList->m_p_list_size        // end pos (not address!)
        };
    }
    else if constexpr (std::is_same_v<ListType, Region_S>) {
        return
        {
            regList->m_s_list_capacity,
            regList->m_s_list_size,
            regList->m_s_list_spaceLeft,
            regList->m_s_list_spaceRight,
            regList->m_s_list_spaceLeft,                                // begin pos (not address!)
            regList->m_s_list_spaceLeft + regList->m_s_list_size        // end pos (not address!)
        };
    } else {
        return ListState();
    }
}


template<class ListType>
inline ListFootprints rl_manip::GetFootprints( RegionsListPtr regList )
{
    if constexpr (std::is_same_v<ListType, Region_P>) {
        return
        {
            reinterpret_cast<size_t>(regList->m_p_list),
            reinterpret_cast<size_t>(regList->m_p_list) + (regList->m_p_list_capacity * sizeof( Region_P )),
            reinterpret_cast<size_t>(regList->m_p_list_begin),
            reinterpret_cast<size_t>(regList->m_p_list_end)
        };
    }
    else if constexpr (std::is_same_v<ListType, Region_S>) {
        return
        {
            reinterpret_cast<size_t>(regList->m_s_list),
            reinterpret_cast<size_t>(regList->m_s_list) + (regList->m_s_list_capacity * sizeof( Region_S )),
            reinterpret_cast<size_t>(regList->m_s_list_begin),
            reinterpret_cast<size_t>(regList->m_s_list_end)
        };
    } else {
        return ListFootprints();
    }
}


template<class ListType>
inline std::tuple<Error_BasePtr, std::vector<ListType>> rl_manip::GetContent( RegionsListPtr regList )
{
    std::vector<ListType> out;
    ListType* it;
    ListType* end;

    if constexpr (std::is_same_v<ListType, Region_P>) {
        it = regList->m_p_list_begin;
        end = regList->m_p_list_end;
    }
    else if constexpr (std::is_same_v<ListType, Region_S>) {
        it = regList->m_s_list_begin;
        end = regList->m_s_list_end;
    }
    else {
        return std::tuple<Error_BasePtr, std::vector<ListType>>( ERR_CUSTOM( "Undefined ListType received: " + std::string( typeid( ListType ).name())), {} );
    }
    if (it > end) {
        return std::tuple<Error_BasePtr, std::vector<ListType>>( ERR_CUSTOM( "List iterators are in a wrong state (begin > end)"), {} );
    }
    for (; it != end; ++it) {
        out.push_back( *it );
    }
    return std::tuple( nullptr, out );
}


Error_BasePtr rl_manip::GetRegionsListDetails( RegionsListPtr regList, TestData &td, bool to_resulted )
{
    auto &p_state_target = to_resulted ? td.p_listState_resulted : td.p_listState_initial;
    auto &s_state_target = to_resulted ? td.s_listState_resulted : td.s_listState_initial;
    auto &p_content_target = to_resulted ? td.p_listContent_resulted : td.p_listContent_initial;
    auto &s_content_target = to_resulted ? td.s_listContent_resulted : td.s_listContent_initial;
    p_state_target = GetState<Region_P>( regList );
    s_state_target = GetState<Region_S>( regList );
    Error_BasePtr err;
    std::tie( err, p_content_target ) = GetContent<Region_P>( regList );                                                                TRACE_CUSTOM_RET_ERR( err, "Can't get P-List content" );
    std::tie( err, s_content_target ) = GetContent<Region_S>( regList );                                                                TRACE_CUSTOM_RET_ERR( err, "Can't get S-List content" );
    return nullptr;
}


Error_BasePtr rl_manip::SetupRegionsList( RegionsListPtr regList, const TestData& settings, bool settings_from_initial )
{
    Error_BasePtr err;
    err = SetState<Region_P>( settings_from_initial ? settings.p_listState_initial : settings.p_listState_resulted, regList );          TRACE_CUSTOM_RET_ERR( err, "Can't set P-List state");
    err = SetState<Region_S>( settings_from_initial ? settings.s_listState_initial : settings.s_listState_resulted, regList );          TRACE_CUSTOM_RET_ERR( err, "Can't set S-List state");
    err = SetContent<Region_P>( settings_from_initial ? settings.p_listContent_initial : settings.p_listContent_resulted, regList );    TRACE_CUSTOM_RET_ERR( err, "Can't set P-List content");
    err = SetContent<Region_S>( settings_from_initial ? settings.s_listContent_initial : settings.s_listContent_resulted, regList );    TRACE_CUSTOM_RET_ERR( err, "Can't set S-List content");
    return nullptr;
}


Error_BasePtr rl_check::CheckMarginsPurity( RegionsListPtr regList )
{
    ListFootprints plist_footpr = rl_manip::GetFootprints<Region_P>( regList );
    ListFootprints slist_footpr = rl_manip::GetFootprints<Region_S>( regList );
    ListState plist_state = rl_manip::GetState<Region_P>( regList );
    ListState slist_state = rl_manip::GetState<Region_S>( regList );

    Region_P* p_list_start = reinterpret_cast<Region_P*>(plist_footpr.addr_start);
    Region_P* p_list_mar_l = p_list_start + plist_state.spaceLeft;
    Region_P* p_list_stop = reinterpret_cast<Region_P*>(plist_footpr.addr_stop);
    Region_P* p_list_mar_r = p_list_stop - plist_state.spaceRight;

    Region_S* s_list_start = reinterpret_cast<Region_S*>(slist_footpr.addr_start);
    Region_S* s_list_mar_l = s_list_start + slist_state.spaceLeft;
    Region_S* s_list_stop = reinterpret_cast<Region_S*>(slist_footpr.addr_stop);
    Region_S* s_list_mar_r = s_list_stop - slist_state.spaceRight;

    for (auto it = p_list_start; it != p_list_mar_l; ++it) {
        if (*it != Region_P{ 0 }) {
            return ERR_CUSTOM( "P-List Left margin isn't clean" );
        }
    }
    for (auto it = p_list_mar_r; it != p_list_stop; ++it) {
        if (*it != Region_P{ 0 }) {
            return ERR_CUSTOM( "P-List Right margin isn't clean" );
        }
    }
    for (auto it = s_list_start; it != s_list_mar_l; ++it) {
        if (*it != Region_S{ 0 }) {
            return ERR_CUSTOM( "S-List Left margin isn't clean" );
        }
    }
    for (auto it = s_list_mar_r; it != s_list_stop; ++it) {
        if (*it != Region_S{ 0 }) {
            return ERR_CUSTOM( "S-List Right margin isn't clean" );
        }
    }
    return nullptr;
}


Error_BasePtr rl_check::CheckListsCompliance( const std::vector<Region_P>& p_vec, const std::vector<Region_S>& s_vec )
{
    if ( utils::SListFromPList( p_vec ) != s_vec) {
        return ERR_CUSTOM( "P- and S-Lists are not equivalent:\nP: " + utils::to_string( p_vec ) + "\nS: " + utils::to_string( s_vec ));
    }
    return nullptr;
}


std::tuple<Error_BasePtr, ListState> rl_check::Validate_ListState( const ListState &state )
{
    std::string errMess( "Wrong relations:\n" );
    bool ok = true;
    Error_BasePtr err = nullptr;
    if( state.capacity < state.size ) {
        errMess += "ListState.capacity < ListState.size\n";
        ok = false;
    }
    if( state.spaceLeft + state.spaceRight + state.size != state.capacity ) {
        errMess += "ListState.spaceLeft + ListState.spaceRight + ListState.size != ListState.capacity\n";
        ok = false;
    }
    if( state.begin_pos != state.spaceLeft ) {
        errMess += "ListState.begin_pos != ListState.spaceLeft\n";
        ok = false;
    }
    if( state.end_pos != state.spaceLeft + state.size ) {
        errMess += "ListState.end_pos != ListState.spaceLeft + ListState.size\n";
        ok = false;
    }
    if( !ok ) {
        errMess += "Details:\n" + utils::to_string( state );
        err = ERR_CUSTOM( errMess );
    }
    return std::tuple( err, state );
}


template<class ListType>
Error_BasePtr rl_check::CheckFootprintsVsState( const ListState& state, const ListFootprints& footpr )
{
    std::string errMess( "Wrong relations:\n" );
    bool ok = true;

    if (footpr.addr_begin != footpr.addr_start + (state.spaceLeft * sizeof( ListType ))) {
        errMess += "ListFootprints.addr_begin != ListFootprints.addr_start + (ListState.spaceLeft * sizeof( CELL ))\n";
        ok = false;
    }
    if (footpr.addr_end != footpr.addr_start + (state.spaceLeft * sizeof( ListType )) + (state.size * sizeof( ListType ))) {
        errMess += "ListFootprints.addr_end != ListFootprints.addr_start + (ListState.spaceLeft * sizeof( CELL )) + (ListState.size * sizeof( CELL ))\n";
        ok = false;
    }
    if (footpr.addr_stop != footpr.addr_start + (state.spaceLeft * sizeof( ListType )) + (state.size * sizeof( ListType )) + (state.spaceRight * sizeof( ListType ))) {
        errMess += "ListFootprints.addr_stop != ListFootprints.addr_start + (ListState.spaceLeft * sizeof( CELL )) + (ListState.size * sizeof( CELL )) + (ListState.spaceRight * sizeof( CELL ))\n";
        ok = false;
    }
    if ((footpr.addr_stop - footpr.addr_start) / sizeof( ListType ) != state.capacity) {
        errMess += "(ListFootprints.addr_stop - ListFootprints.addr_start) / sizeof( CELL ) != ListState.capacity\n";
        ok = false;
    }
    if (!ok) {
        errMess += "Details:\nState:   " + utils::to_string( state ) + "\nContent: " + utils::to_string( footpr );
        return ERR_CUSTOM( errMess );
    }
    return nullptr;
}


Error_BasePtr rl_check::CheckState( const ListState &expected, const ListState &gained )
{
    if( expected != gained ) {
        return ERR_CUSTOM( "Expected:\n" + utils::to_string( expected ) + "\nGained:\n" + utils::to_string( gained ));
    }
    return nullptr;
}


template<class ListType>
Error_BasePtr rl_check::CheckContent( const std::vector<ListType>& expected, std::vector<ListType>& gained )
{
    if (expected.size() != gained.size()) {
        return ERR_CUSTOM( "Expected size: " + std::to_string( expected.size() ) + "\nGained size: " + std::to_string( gained.size()));
    }
    if constexpr (std::is_same_v<ListType, Region_P> || std::is_same_v<ListType, Region_S>)
    {
        bool ok = true;
        for (size_t i = 0; i < expected.size(); ++i) {
            ok &= expected[i] == gained[i];
        }
        if (!ok) {
            return ERR_CUSTOM( "Expected content: " + utils::to_string( expected ) + "\nGained content:   " + utils::to_string( gained ));
        }
        return nullptr;
    }
    else {
        return ERR_CUSTOM( "Undefined ListType received: " + std::string( typeid(ListType).name()));
    }
}


template<class ListType>
Error_BasePtr rl_check::CheckIfContentOutOfBounds( const std::vector<ListType>& content, Region_P bounds )
{
    if constexpr (std::is_same_v<ListType, Region_P> || std::is_same_v<ListType, Region_S>)
    {
        std::string errMess;
        for (const auto& reg : content) {
            if (reg.start < bounds.start || reg.start + reg.size > bounds.start + bounds.size) {
                return ERR_CUSTOM( "Bounds:    " + utils::to_string( bounds ) + "\nRegion:    " + utils::to_string( reg ) + 
                                   "\nCell size: " + std::to_string(sizeof(CELL)) + " Bytes\nContent:   " + utils::to_string( content ));
            }
        }
        return nullptr;
    }
    else {
        return ERR_CUSTOM( "Undefined ListType received: " + std::string( typeid(ListType).name()));
    }
}


Error_BasePtr rl_check::CheckRegionsList( RegionsListPtr regList, const TestData& expected, bool check_state, bool check_content, bool check_bounds, bool check_resulted )
{
    Error_BasePtr err = rl_check::CheckMarginsPurity( regList );                                                                                        TRACE_CUSTOM_RET_ERR( err, "Margins are not clean" );
    std::vector<Region_P> P_Content;
    std::vector<Region_S> S_Content;
    std::tie( err, P_Content ) = rl_manip::GetContent<Region_P>( regList );                                                                             TRACE_CUSTOM_RET_ERR( err, "Can't get P-List content" );
    std::tie( err, S_Content ) = rl_manip::GetContent<Region_S>( regList );                                                                             TRACE_CUSTOM_RET_ERR( err, "Can't get S-List content" );
    err = rl_check::CheckListsCompliance( P_Content, S_Content );                                                                                       TRACE_CUSTOM_RET_ERR( err, "Lists compliance error" );
    ListState P_State, S_State;
    std::tie( err, P_State ) = rl_check::Validate_ListState( rl_manip::GetState<Region_P>( regList ));                                                  TRACE_CUSTOM_RET_ERR( err, "Incorrect P-State gained" );
    std::tie( err, S_State ) = rl_check::Validate_ListState( rl_manip::GetState<Region_S>( regList ));                                                  TRACE_CUSTOM_RET_ERR( err, "Incorrect S-State gained" );
    err = rl_check::CheckFootprintsVsState<Region_P>( P_State, rl_manip::GetFootprints<Region_P>( regList ));                                           TRACE_CUSTOM_RET_ERR( err, "Incorrect P-State/P-Footprints combo gained" );
    err = rl_check::CheckFootprintsVsState<Region_S>( S_State, rl_manip::GetFootprints<Region_S>( regList ));                                           TRACE_CUSTOM_RET_ERR( err, "Incorrect S-State/S-Footprints combo gained" );

    if (check_state) {
        err = rl_check::CheckState( check_resulted ? expected.p_listState_resulted : expected.p_listState_initial, P_State );                           TRACE_CUSTOM_RET_ERR( err, "Unexpected P-State gained" );
        err = rl_check::CheckState( check_resulted ? expected.s_listState_resulted : expected.s_listState_initial, S_State );                           TRACE_CUSTOM_RET_ERR( err, "Unexpected S-State gained" );
    }
    if (check_content) {
        err = rl_check::CheckContent<Region_P>( check_resulted ? expected.p_listContent_resulted : expected.p_listContent_initial, P_Content );         TRACE_CUSTOM_RET_ERR( err, "Unexpected P-List content gained" );
        err = rl_check::CheckContent<Region_S>( check_resulted ? expected.s_listContent_resulted : expected.s_listContent_initial, S_Content );         TRACE_CUSTOM_RET_ERR( err, "Unexpected S-List content gained" );
    }
    if (check_bounds) {
        err = rl_check::CheckIfContentOutOfBounds<Region_P>( P_Content, expected.intermediate_reg );                                                    TRACE_CUSTOM_RET_ERR( err, "Element from P-List is out of bounds" );
        err = rl_check::CheckIfContentOutOfBounds<Region_S>( S_Content, expected.intermediate_reg );                                                    TRACE_CUSTOM_RET_ERR( err, "Element from S-List is out of bounds" );
    }
    return nullptr;
}


std::tuple<Error_BasePtr, TestData> rl_check::Check_RegList_Init( RegionsListPtr checkedRegList, size_t init_capacity, const Region_P& init_region, const Region_P& bounds )
{
    // Проверяем состояние пустого RegionsList определённой ёмкости
    TestData td = { 0 };
    td.p_listState_resulted.capacity = init_capacity >= 3 ? init_capacity : 3;
    td.p_listState_resulted.size = 0;
    td.p_listState_resulted.spaceLeft = td.p_listState_resulted.capacity / 2;
    td.p_listState_resulted.spaceRight = td.p_listState_resulted.capacity - td.p_listState_resulted.spaceLeft;
    td.p_listState_resulted.begin_pos = td.p_listState_resulted.spaceLeft;
    td.p_listState_resulted.end_pos = td.p_listState_resulted.begin_pos;
    td.s_listState_resulted = td.p_listState_resulted;

    Error_BasePtr err = rl_check::CheckRegionsList( checkedRegList, td );               TRACE_CUSTOM_RET_VAL( err, std::tuple(err, td), "RegionsList is broken after creation" );

    // Инициализирующая вставка
    err = checkedRegList->ReleaseRegion( init_region );                                 TRACE_CUSTOM_RET_VAL( err, std::tuple( err, td ), "Error during RegionsList initial insertion" );

    // Проверяем корректность RegionsList после инициализирующей вставки
    td.p_listState_resulted.size++;
    td.s_listState_resulted.size++;
    td.p_listState_resulted.spaceRight--;
    td.s_listState_resulted.spaceRight--;
    td.p_listState_resulted.end_pos++;
    td.s_listState_resulted.end_pos++;
    td.p_listContent_resulted = { init_region };
    td.s_listContent_resulted = utils::SListFromPList( td.p_listContent_resulted );
    td.intermediate_reg = bounds;

    err = rl_check::CheckRegionsList( checkedRegList, td );                             TRACE_CUSTOM_RET_VAL( err, std::tuple(err, td), "RegionsList is broken after initial insertion" );
    return std::tuple(nullptr, td);
}