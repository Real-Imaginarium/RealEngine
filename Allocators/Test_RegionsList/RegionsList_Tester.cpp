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


using Ins_complex =    Globals::Ins_complex;
using InsertionPlace = Globals::Ins_complex::InsertionPlace;
using FieldState =     Globals::Ins_complex::FieldState;

using namespace std::filesystem;

// Проверяет вставку уже существующего региона в RegionsList
void RegionsList_Tester::Test_DoubleInserion()
{
    Log::test( "Log.txt" ) << LogTest::Start{ "Test Existing Region Insertions", 1 };

    Error_BasePtr err = nullptr;
    size_t regList_initCap = 0;
    size_t pitch_size = 4000;
    CELL* memoryPitch = new CELL[pitch_size];
    auto regList = std::make_shared<RegionsList<CELL>>( regList_initCap );

    // Тестовые вставки
    Region_P initial_region = { memoryPitch + 100, 100 };
    Region_P second_region  = { memoryPitch + 300, 100 };

    do {
        // Проверяем RegionsList в процессе инициализации
        rl_check::CheckRegionsListInitialization( regList, regList_initCap, initial_region, Region_P{ memoryPitch, pitch_size }, err );
        TRACE_PRINT_CONTINUE( err, "RegionsList initialization error" );

        // Пытаемся заинсертить существующий регион, следим за ошибкой
        auto errRegs = std::dynamic_pointer_cast<Error_RegionsList, Error_Base>(regList->ReleaseRegion( initial_region ));

        if (!errRegs || errRegs->Type() != RegList_ErrType::ERR_EXISTING_REG_INSERTION) {
            err = std::make_shared<Error_Custom>( "Error isn't detected (Trying to insert existing region)", PLACE(), "Log.txt" );
            TRACE_PRINT_CONTINUE( err, "ReleaseRegion() Wrong behavior after initial insertion" );
        }
        // Делаем вторую вставку
        err = regList->ReleaseRegion( second_region );                                  TRACE_PRINT_CONTINUE( err, "Error during RegionsList second insertion" );

        // Пытаемся заинсертить существующий регион, следим за ошибкой
        errRegs = std::dynamic_pointer_cast<Error_RegionsList, Error_Base>(regList->ReleaseRegion( second_region ));

        if (!errRegs || errRegs->Type() != RegList_ErrType::ERR_EXISTING_REG_INSERTION) {
            err = std::make_shared<Error_Custom>( "Error isn't detected (Trying to insert existing region)", PLACE(), "Log.txt" );
            TRACE_PRINT_CONTINUE( err, "ReleaseRegion() Wrong behavior after second insertion" );
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
        TestData td = rl_check::CheckRegionsListInitialization( regList, regList_initCap, initial_region, Region_P{ memoryPitch, pitch_size }, err );
        TRACE_PRINT_CONTINUE( err, "RegionsList initialization error" );

        // Пытаемся заинсертить перекрывающийся слева регион, следим за ошибкой
        auto errRegs = std::dynamic_pointer_cast<Error_RegionsList, Error_Base>(regList->ReleaseRegion( left_overlapped ));
        if (!errRegs || errRegs->Type() != RegList_ErrType::ERR_OVERLAPPED_REG_INSERTION) {
            err = std::make_shared<Error_Custom>( "Error isn't detected (Trying to insert left overlapped region)", PLACE(), "Log.txt" );
            TRACE_PRINT_CONTINUE( err, "ReleaseRegion() Wrong behavior after initial insertion" );
        }
        // Пытаемся заинсертить перекрывающийся справа регион, следим за ошибкой
        errRegs = std::dynamic_pointer_cast<Error_RegionsList, Error_Base>(regList->ReleaseRegion( right_overlapped ));
        if (!errRegs || errRegs->Type() != RegList_ErrType::ERR_OVERLAPPED_REG_INSERTION) {
            err = std::make_shared<Error_Custom>( "Error isn't detected (Trying to insert right overlapped region)", PLACE(), "Log.txt" );
            TRACE_PRINT_CONTINUE( err, "ReleaseRegion() Wrong behavior after initial insertion" );
        }
        // Делаем вторую вставку
        err = regList->ReleaseRegion( second_region );                                  TRACE_PRINT_CONTINUE( err, "Error during RegionsList second insertion" );
        right_overlapped = { memoryPitch + 249, 50 };

        // Пытаемся заинсертить посередине перекрывающийся слева регион
        errRegs = std::dynamic_pointer_cast<Error_RegionsList, Error_Base>(regList->ReleaseRegion( medium_left_overlapped ));
        if (!errRegs || errRegs->Type() != RegList_ErrType::ERR_OVERLAPPED_REG_INSERTION) {
            err = std::make_shared<Error_Custom>( "Error isn't detected (Trying to insert medium left overlapped region)", PLACE(), "Log.txt" );
            TRACE_PRINT_CONTINUE( err, "ReleaseRegion() Wrong behavior after second insertion" );
        }
        // Пытаемся заинсертить посередине перекрывающийся слева и смежный справа регион
        errRegs = std::dynamic_pointer_cast<Error_RegionsList, Error_Base>(regList->ReleaseRegion( medium_left_overlapped_adj ));
        if (!errRegs || errRegs->Type() != RegList_ErrType::ERR_OVERLAPPED_REG_INSERTION) {
            err = std::make_shared<Error_Custom>( "Error isn't detected (Trying to insert medium left overlapped right-adjacent region)", PLACE(), "Log.txt" );
            TRACE_PRINT_CONTINUE( err, "ReleaseRegion() Wrong behavior after second insertion" );
        }
        // Пытаемся заинсертить посередине перекрывающийся справа регион
        errRegs = std::dynamic_pointer_cast<Error_RegionsList, Error_Base>(regList->ReleaseRegion( medium_right_overlapped ));
        if (!errRegs || errRegs->Type() != RegList_ErrType::ERR_OVERLAPPED_REG_INSERTION) {
            err = std::make_shared<Error_Custom>( "Error isn't detected (Trying to insert medium right overlapped region)", PLACE(), "Log.txt" );
            TRACE_PRINT_CONTINUE( err, "ReleaseRegion() Wrong behavior after second insertion" );
        }
        // Пытаемся заинсертить посередине перекрывающийся справа и смежный слева регион
        errRegs = std::dynamic_pointer_cast<Error_RegionsList, Error_Base>(regList->ReleaseRegion( medium_right_overlapped_adj ));
        if (!errRegs || errRegs->Type() != RegList_ErrType::ERR_OVERLAPPED_REG_INSERTION) {
        	err = std::make_shared<Error_Custom>( "Error isn't detected (Trying to insert medium right overlapped left-adjacent region)", PLACE(), "Log.txt" );
            TRACE_PRINT_CONTINUE( err, "ReleaseRegion() Wrong behavior after second insertion" );
        }
        // Пытаемся заинсертить посередине перекрывающийся слева и справа регион
        errRegs = std::dynamic_pointer_cast<Error_RegionsList, Error_Base>(regList->ReleaseRegion( medium_left_right ));
        if (!errRegs || errRegs->Type() != RegList_ErrType::ERR_OVERLAPPED_REG_INSERTION) {
            err = std::make_shared<Error_Custom>( "Error isn't detected (Trying to insert medium right and left overlapped region)", PLACE(), "Log.txt" );
            TRACE_PRINT_CONTINUE( err, "ReleaseRegion() Wrong behavior after second insertion" );
        }
        // Пытаемся заинсертить крайний левый перекрывающийся регион
        errRegs = std::dynamic_pointer_cast<Error_RegionsList, Error_Base>(regList->ReleaseRegion( left_overlapped ));
        if (!errRegs || errRegs->Type() != RegList_ErrType::ERR_OVERLAPPED_REG_INSERTION) {
        	err = std::make_shared<Error_Custom>( "Error isn't detected (Trying to insert left overlapped region)", PLACE(), "Log.txt" );
            TRACE_PRINT_CONTINUE( err, "ReleaseRegion() Wrong behavior after second insertion" );
        }
        // Пытаемся заинсертить крайний правый перекрывающийся регион
        errRegs = std::dynamic_pointer_cast<Error_RegionsList, Error_Base>(regList->ReleaseRegion( right_overlapped ));
        if (!errRegs || errRegs->Type() != RegList_ErrType::ERR_OVERLAPPED_REG_INSERTION) {
            err = std::make_shared<Error_Custom>( "Error isn't detected (Trying to insert right overlapped region)", PLACE(), "Log.txt" );
            TRACE_PRINT_CONTINUE( err, "ReleaseRegion() Wrong behavior after second insertion" );
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
        rl_check::CheckRegionsList( regList, td, err );                                 TRACE_PRINT_CONTINUE( err, "RegionsList is broken after creation" );

        // Пытаемся захватить регион из пустого списка, следим за ошибкой
        auto errRegs = std::dynamic_pointer_cast<Error_RegionsList, Error_Base>(regList->GrabRegion( 1, &grabbedReg ));

        if (!errRegs || errRegs->Type() != RegList_ErrType::ERR_GRAB_FROM_EMPTY_LIST) {
            err = std::make_shared<Error_Custom>( "Error isn't detected (Trying to grab region from empty S-List)", PLACE(), "Log.txt" );
            TRACE_PRINT_CONTINUE( err, "GrabRegion() Wrong behavior after RegionsList creation" );
        }
        if (grabbedReg != marker) {
            err = std::make_shared<Error_Custom>( "Output pointer has been changed during attempt to grab region from empty S-List", PLACE(), "Log.txt" );
            TRACE_PRINT_CONTINUE( err, "GrabRegion() Wrong behavior after RegionsList creation" );
        }

        // Проверяем корректность RegionsList после попытки захвата
        rl_check::CheckRegionsList( regList, td, err );                                 TRACE_PRINT_CONTINUE( err, "RegionsList is broken after attempt to grab" );

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
    CELL* marker = memoryPitch + utils::random_int( 0, (int)pitch_size - 1 );
    CELL* grabbedReg = marker;
    auto regList = std::make_shared<RegionsList<CELL>>( regList_initCap );

    Region_P initial_insertion = { memoryPitch,      10 };
    Region_P second_insertion  = { memoryPitch + 20, 11 };

    do {
        // Проверяем RegionsList в процессе инициализации
        TestData td = rl_check::CheckRegionsListInitialization( regList, regList_initCap, initial_insertion, Region_P{ memoryPitch, pitch_size }, err );
        TRACE_PRINT_CONTINUE( err, "RegionsList initialization error" );

        // Пытаемся захватить слишком большой регион, следим за ошибкой
        auto errRegs = std::dynamic_pointer_cast<Error_RegionsList, Error_Base>(regList->GrabRegion( 11, &grabbedReg ));

        if (!errRegs || errRegs->Type() != RegList_ErrType::ERR_CONSISTENT_REG_NOTFOUND) {
            err = std::make_shared<Error_Custom>( "Error isn't detected (Trying to grab region with size is greater than S-List has)", PLACE(), "Log.txt" );
            TRACE_PRINT_CONTINUE( err, "GrabRegion() Wrong behavior after initial insertion" );
        }
        if (grabbedReg != marker) {
            err = std::make_shared<Error_Custom>( "Output pointer has been changed during attempt to grab region with too big size", PLACE(), "Log.txt" );
            TRACE_PRINT_CONTINUE( err, "GrabRegion() Wrong behavior after initial insertion" );
        }

        // Проверяем корректность RegionsList после попытки захвата слишком большого региона
        rl_check::CheckRegionsList( regList, td, err );                                 TRACE_PRINT_CONTINUE( err, "RegionsList is broken after initial insertion and trying to grab too big region" );

        // Делаем вторую вставку
        err = regList->ReleaseRegion( second_insertion );                               TRACE_PRINT_CONTINUE( err, "Error during RegionsList second insertion" );

        // Проверяем корректность RegionsList после второй вставки
        td.p_listState_resulted = { 3, 2, 1, 0, 1, 3 };
        td.s_listState_resulted = { 3, 2, 1, 0, 1, 3 };
        td.p_listContent_resulted = { initial_insertion, second_insertion };
        td.s_listContent_resulted = utils::SListFromPList( td.p_listContent_resulted );
        rl_check::CheckRegionsList( regList, td, err );                                 TRACE_PRINT_CONTINUE( err, "RegionsList is broken after second insertion" );

        // Пытаемся захватить слишком большой регион после второй вставки, следим за ошибкой
        errRegs = std::dynamic_pointer_cast<Error_RegionsList, Error_Base>(regList->GrabRegion( 12, &grabbedReg ));
        if (!errRegs || errRegs->Type() != RegList_ErrType::ERR_CONSISTENT_REG_NOTFOUND) {
            err = std::make_shared<Error_Custom>( "Error isn't detected (Trying to grab the region with size is greater than S-List has)", PLACE(), "Log.txt" );
            TRACE_PRINT_CONTINUE( err, "GrabRegion() Wrong behavior after second insertion" );
        }
        if (grabbedReg != marker) {
            err = std::make_shared<Error_Custom>( "Output pointer has been changed during attempt to grab region with too big size", PLACE(), "Log.txt" );
            TRACE_PRINT_CONTINUE( err, "GrabRegion() Wrong behavior after second insertion" );
        }

        // Проверяем корректность RegionsList после попытки захвата слишком большого региона
        rl_check::CheckRegionsList( regList, td, err );                                 TRACE_PRINT_CONTINUE( err, "RegionsList is broken after second insertion and trying to grab too big region" );

        delete[] memoryPitch;
        Log::test( "Log.txt" ) << LogTest::Finished{ true };
        return;

    } while (0);

    delete[] memoryPitch;
    Log::test( "Log.txt" ) << LogTest::Finished{ false };
}


// Проверяет работу RegionsList при расширении списков (ExpandList)
void RegionsList_Tester::Test_ListsExpanding()
{
    Log::test( "Log.txt" ) << LogTest::Start{ "Test Lists Expanding", 1 };

    Error_BasePtr err = nullptr;
    size_t regList_initCap = 0;
    size_t pitch_size = 4000;
    CELL* memoryPitch = new CELL[pitch_size];
    auto regList = std::make_shared<RegionsList<CELL>>( regList_initCap );

    // Формируем набор вставок
    std::vector<Region_P> regs( 200 );
    for (size_t i = 0; i < 200; ++i) {
        regs[i] = Region_P{ memoryPitch + i * 10, 9 };
    }

    do {
        // Проверяем корректность RegionsList после создания
        TestData td = { 0 };
        td.p_listState_resulted = { 3, 0, 1, 2, 1, 1 };
        td.s_listState_resulted = { 3, 0, 1, 2, 1, 1 };
        rl_check::CheckRegionsList( regList, td, err );             TRACE_PRINT_CONTINUE( err, "RegionsList is broken after creation" );

        // Производим вставки с целью расширения P- и S-списков
        std::string errMess = "Error during RegionsList insertion №";
        uint8_t c = 0;
        err = regList->ReleaseRegion( regs[100] );                  TRACE_PRINT_CONTINUE( err, errMess + std::to_string(c++) ); // [---] [#B#] [-E-]
        err = regList->ReleaseRegion( regs[101] );                  TRACE_PRINT_CONTINUE( err, errMess + std::to_string(c++) ); // [---] [#B#] [###] E
        err = regList->ReleaseRegion( regs[102] );                  TRACE_PRINT_CONTINUE( err, errMess + std::to_string(c++) ); // [---] [---] [#B#] [###] [###] [-E-]
        err = regList->ReleaseRegion( regs[103] );                  TRACE_PRINT_CONTINUE( err, errMess + std::to_string(c++) ); // [---] [---] [#B#] [###] [###] [###] E
        err = regList->ReleaseRegion( regs[104] );                  TRACE_PRINT_CONTINUE( err, errMess + std::to_string(c++) ); // [---] [---] [---] [---] [#B#] [###] [###] [###] [###] [-E-] [---] [---]

        // Проверяем корерктность RegionsList после 5 вставок и 2 расширений вправо
        td.p_listState_resulted = { 12, 5, 4, 3, 4, 9 };
        td.s_listState_resulted = { 12, 5, 4, 3, 4, 9 };
        td.p_listContent_resulted = { regs[100], regs[101], regs[102], regs[103], regs[104] };
        td.s_listContent_resulted = utils::SListFromPList( td.p_listContent_resulted );
        td.intermediate_reg = Region_P{ memoryPitch, pitch_size };
        rl_check::CheckRegionsList( regList, td, err );             TRACE_PRINT_CONTINUE( err, "RegionsList is broken after 5 insertions (expanded right 2 times)" );

        // Продолжаем расширять список вправо и влево (по одному разу)
        for (uint8_t i = 105; i < 109; ++i, ++c) {      // Расширяем вправо
            err = regList->ReleaseRegion( regs[i] );                TRACE_PRINT_CONTINUE( err, errMess + std::to_string(c) );   // [---] [---] [---] [---] [#B#] [###] [###] [###] [###] [###] [###] [###] E
        }                                                                                                       // last iteration  [---] [---] [---] [---] [---] [---] [---] [---] [#B#] [###] [###] [###] [###] [###] [###] [###] [#№#] [-E-] [---] [---] [---] [---] [---] [---]
        for (uint8_t i = 109; i < 116; ++i, ++c) {      // Заполняем правое поле
            err = regList->ReleaseRegion( regs[i] );                TRACE_PRINT_CONTINUE( err, errMess + std::to_string(c) );   // [---] [---] [---] [---] [---] [---] [---] [---] [#B#] [###] [###] [###] [###] [###] [###] [###] [#№#] [###] [###] [###] [###] [###] [###] [###] E
        }
        for (uint8_t i = 91; i < 100; ++i, ++c) {       // Расширяем влево
            err = regList->ReleaseRegion( regs[i] );                TRACE_PRINT_CONTINUE( err, errMess + std::to_string(c) );   // [#B#] [###] [###] [###] [###] [###] [###] [###] [###] [###] [###] [###] [###] [###] [###] [###] [#№#] [###] [###] [###] [###] [###] [###] [###] E
        }                                                                                                       // last iteration  [---] [---] [---] [---] [---] [---] [---] [---] [---] [---] [---] [#B#] [###] [###] [###] [###] [###] [###] [###] [###] [###] [###] [###] [###] [###] [###] [###] [###] [###] [###] [###] [###] [###] [###] [###] [###] [-E-] [---] [---] [---] [---] [---] [---] [---] [---] [---] [---] [---]
        for (uint8_t i = 116; i < 128; ++i, ++c) {      // Заполняем правое поле
            err = regList->ReleaseRegion( regs[i] );                TRACE_PRINT_CONTINUE( err, errMess + std::to_string(c) );   // [---] [---] [---] [---] [---] [---] [---] [---] [---] [---] [---] [#B#] [###] [###] [###] [###] [###] [###] [###] [###] [###] [###] [###] [###] [###] [###] [###] [###] [###] [###] [###] [###] [###] [###] [###] [###] [###] [###] [###] [###] [###] [###] [###] [###] [###] [###] [###] [###] E
        }
        for (uint8_t i = 80; i < 91; ++i, ++c) {        // Заполняем левое поле
            err = regList->ReleaseRegion( regs[i] );                TRACE_PRINT_CONTINUE( err, errMess + std::to_string(c) );   // [#B#] [###] [###] [###] [###] [###] [###] [###] [###] [###] [###] [###] [###] [###] [###] [###] [###] [###] [###] [###] [###] [###] [###] [###] [###] [###] [###] [###] [###] [###] [###] [###] [###] [###] [###] [###] [###] [###] [###] [###] [###] [###] [###] [###] [###] [###] [###] [###] E
        }
        // Проверяем корерктность RegionsList после 43 вставок и 2 расширений (вправо и влево)
        td.p_listState_resulted = { 48, 48, 0, 0, 0, 48 };
        td.s_listState_resulted = { 48, 48, 0, 0, 0, 48 };
        td.p_listContent_resulted = std::vector<Region_P>( regs.begin() + 80, regs.begin() + 128 );
        td.s_listContent_resulted = utils::SListFromPList( td.p_listContent_resulted );
        rl_check::CheckRegionsList( regList, td, err );             TRACE_PRINT_CONTINUE( err, "RegionsList is broken after 43 insertions (expanded 2 times to right and left)" );

    } while (0);

    delete[] memoryPitch;
    Log::test( "Log.txt" ) << LogTest::Finished{ err == nullptr };
}


// Проверяет инициализирующую вставку в RegionsList (когда он только создан и ничего не содержит)
void RegionsList_Tester::Test_InitialReleaseRegion()
{
    Log::test( "Log.txt" ) << LogTest::Start{ "Test Insertions Initial" };

    Error_BasePtr err = nullptr;
    size_t regList_initCap = 5;
    size_t pitch_size = 20000;
    CELL* memoryPitch = new CELL[pitch_size];
    auto regList = std::make_shared<RegionsList<CELL>>( regList_initCap );

    // Тестовые вставки
    Region_P initial_insertion = { memoryPitch, pitch_size };

    do {
        // Проверяем RegionsList в процессе инициализации
        rl_check::CheckRegionsListInitialization( regList, regList_initCap, initial_insertion, Region_P{ memoryPitch, pitch_size }, err );
        TRACE_PRINT_CONTINUE( err, "RegionsList initialization error" );

    } while (0);

    delete[] memoryPitch;
    Log::test( "Log.txt" ) << LogTest::Finished{ err == nullptr };
}


// Проверяет вставку в RegionsList, смежную справа, потом слева, когда он содержит 1 регион
void RegionsList_Tester::Test_SecondRelease_LeftRightAdj()
{
    Log::test( "Log.txt" ) << LogTest::Start{ "Test Second Left/Right Adjacent Insertions" };

    Error_BasePtr err = nullptr;
    size_t regList_initCap = 5;
    size_t pitch_size = 1000;
    CELL* memoryPitch = new CELL[pitch_size];
    auto regList = std::make_shared<RegionsList<CELL>>( regList_initCap );

    // Тестовые вставки
    Region_P initial_insertion   = { memoryPitch + 500, 50 };
    Region_P left_insertion_adj  = { memoryPitch + 450, 50 };
    Region_P right_insertion_adj = { memoryPitch + 550, 50 };

    do {
        // Проверяем RegionsList в процессе инициализации
        TestData td = rl_check::CheckRegionsListInitialization( regList, regList_initCap, initial_insertion, Region_P{ memoryPitch, pitch_size }, err );
        TRACE_PRINT_CONTINUE( err, "RegionsList initialization error" );
        
        // Делаем смежную вставку слева
        err = regList->ReleaseRegion( left_insertion_adj );                             TRACE_PRINT_CONTINUE( err, "Error during RegionsList left-adjacent insertion" );

        // Проверяем корректность RegionsList после смежной вставки слева
        td.p_listContent_resulted = { { memoryPitch + 450, 100 } };
        td.s_listContent_resulted = utils::SListFromPList( td.p_listContent_resulted );
        rl_check::CheckRegionsList( regList, td, err );                                 TRACE_PRINT_CONTINUE( err, "RegionsList is broken after left-adjacent insertion" );

        // Делаем смежную вставку справа
        err = regList->ReleaseRegion( right_insertion_adj );                            TRACE_PRINT_CONTINUE( err, "Error during RegionsList right-adjacent insertion" );

        // Проверяем корректность RegionsList после смежной вставки справа
        td.p_listContent_resulted = { { memoryPitch + 450, 150 } };
        td.s_listContent_resulted = utils::SListFromPList( td.p_listContent_resulted );
        rl_check::CheckRegionsList( regList, td, err );                                 TRACE_PRINT_CONTINUE( err, "RegionsList is broken after right-adjacent insertion" );

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
    auto regList = std::make_shared<RegionsList<CELL>>( regList_initCap );

    // Тестовые вставки
    Region_P initial_insertion        = { memoryPitch + 500, 50 };
    Region_P left_insertion_size_less = { memoryPitch + 450, 49 };

    do {
        // Проверяем RegionsList в процессе инициализации
        TestData td = rl_check::CheckRegionsListInitialization( regList, regList_initCap, initial_insertion, Region_P{ memoryPitch, pitch_size }, err );
        TRACE_PRINT_CONTINUE( err, "RegionsList initialization error" );

        // Делаем вставку слева меньшей ширины
        err = regList->ReleaseRegion( left_insertion_size_less );                       TRACE_PRINT_CONTINUE( err, "Error during RegionsList left-size-less insertion" );

        // Проверяем корректность RegionsList после вставки слева меньшей ширины
        td.p_listState_resulted = { 5, 2, 1, 2, 1, 3 };
        td.s_listState_resulted = { 5, 2, 1, 2, 1, 3 };
        td.p_listContent_resulted = { left_insertion_size_less, initial_insertion };
        td.s_listContent_resulted = utils::SListFromPList( td.p_listContent_resulted );
        rl_check::CheckRegionsList( regList, td, err );                                 TRACE_PRINT_CONTINUE( err, "RegionsList is broken after left-size-less insertion" );

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
    auto regList = std::make_shared<RegionsList<CELL>>( regList_initCap );

    // Тестовые вставки
    Region_P initial_insertion         = { memoryPitch + 500, 50 };
    Region_P left_insertion_size_great = { memoryPitch + 448, 51 };

    do {
        // Проверяем RegionsList в процессе инициализации
        TestData td = rl_check::CheckRegionsListInitialization( regList, regList_initCap, initial_insertion, Region_P{ memoryPitch, pitch_size }, err );
        TRACE_PRINT_CONTINUE( err, "RegionsList initialization error" );

        // Делаем вставку слева большей ширины
        err = regList->ReleaseRegion( left_insertion_size_great );                      TRACE_PRINT_CONTINUE( err, "Error during RegionsList left-size-great insertion" );

        // Проверяем корректность RegionsList после вставки слева большей ширины
        td.p_listState_resulted = { 5, 2, 1, 2, 1, 3 };
        td.s_listState_resulted = { 5, 2, 2, 1, 2, 4 };
        td.p_listContent_resulted = { left_insertion_size_great, initial_insertion };
        td.s_listContent_resulted = utils::SListFromPList( td.p_listContent_resulted );
        rl_check::CheckRegionsList( regList, td, err );                                 TRACE_PRINT_CONTINUE( err, "RegionsList is broken after left-size-great insertion" );

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
    auto regList = std::make_shared<RegionsList<CELL>>( regList_initCap );

    // Тестовые вставки
    Region_P initial_insertion =         { memoryPitch + 500, 50 };
    Region_P left_insertion_size_equal = { memoryPitch + 449, 50 };

    do {
        // Проверяем RegionsList в процессе инициализации
        TestData td = rl_check::CheckRegionsListInitialization( regList, regList_initCap, initial_insertion, Region_P{ memoryPitch, pitch_size }, err );
        TRACE_PRINT_CONTINUE( err, "RegionsList initialization error" );

        // Делаем вставку слева равной ширины
        err = regList->ReleaseRegion( left_insertion_size_equal );                      TRACE_PRINT_CONTINUE( err, "Error during RegionsList left-size-equal insertion" );

        // Проверяем корректность RegionsList после вставки слева равной ширины
        td.p_listState_resulted = { 5, 2, 1, 2, 1, 3 };
        td.s_listState_resulted = { 5, 2, 1, 2, 1, 3 };
        td.p_listContent_resulted = { left_insertion_size_equal, initial_insertion };
        td.s_listContent_resulted = utils::SListFromPList( td.p_listContent_resulted );
        rl_check::CheckRegionsList( regList, td, err );                                 TRACE_PRINT_CONTINUE( err, "RegionsList is broken after left-size-equal insertion" );

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
    auto regList = std::make_shared<RegionsList<CELL>>( regList_initCap );

    // Тестовые вставки
    Region_P initial_insertion =         { memoryPitch + 500, 50 };
    Region_P right_insertion_size_less = { memoryPitch + 551, 49 };

    do {
        // Проверяем RegionsList в процессе инициализации
        TestData td = rl_check::CheckRegionsListInitialization( regList, regList_initCap, initial_insertion, Region_P{ memoryPitch, pitch_size }, err );
        TRACE_PRINT_CONTINUE( err, "RegionsList initialization error" );

        // Делаем вставку справа меньшей ширины
        err = regList->ReleaseRegion( right_insertion_size_less );                      TRACE_PRINT_CONTINUE( err, "Error during RegionsList right-size-less insertion" );

        // Проверяем корректность RegionsList после вставки справа меньшей ширины
        td.p_listState_resulted = { 5, 2, 2, 1, 2, 4 };
        td.s_listState_resulted = { 5, 2, 1, 2, 1, 3 };
        td.p_listContent_resulted = { initial_insertion, right_insertion_size_less };
        td.s_listContent_resulted = utils::SListFromPList( td.p_listContent_resulted );
        rl_check::CheckRegionsList( regList, td, err );                                 TRACE_PRINT_CONTINUE( err, "RegionsList is broken after right-size-less insertion" );

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
    auto regList = std::make_shared<RegionsList<CELL>>( regList_initCap );

    // Тестовые вставки
    Region_P initial_insertion =          { memoryPitch + 500, 50 };
    Region_P right_insertion_size_great = { memoryPitch + 551, 51 };

    do {
        // Проверяем RegionsList в процессе инициализации
        TestData td = rl_check::CheckRegionsListInitialization( regList, regList_initCap, initial_insertion, Region_P{ memoryPitch, pitch_size }, err );
        TRACE_PRINT_CONTINUE( err, "RegionsList initialization error" );

        // Делаем вставку справа большей ширины
        err = regList->ReleaseRegion( right_insertion_size_great );                     TRACE_PRINT_CONTINUE( err, "Error during RegionsList right-size-great insertion" );
        
        // Проверяем корректность RegionsList после вставки справа большей ширины
        td.p_listState_resulted = { 5, 2, 2, 1, 2, 4 };
        td.s_listState_resulted = { 5, 2, 2, 1, 2, 4 };
        td.p_listContent_resulted = { initial_insertion, right_insertion_size_great };
        td.s_listContent_resulted = utils::SListFromPList( td.p_listContent_resulted );
        rl_check::CheckRegionsList( regList, td, err );                                 TRACE_PRINT_CONTINUE( err, "RegionsList is broken after right-size-great insertion" );

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
    auto regList = std::make_shared<RegionsList<CELL>>( regList_initCap );

    // Тестовые вставки
    Region_P initial_insertion =          { memoryPitch + 500, 50 };
    Region_P right_insertion_size_equal = { memoryPitch + 551, 50 };

    do {
        // Проверяем RegionsList в процессе инициализации
        TestData td = rl_check::CheckRegionsListInitialization( regList, regList_initCap, initial_insertion, Region_P{ memoryPitch, pitch_size }, err );
        TRACE_PRINT_CONTINUE( err, "RegionsList initialization error" );

        // Делаем вставку справа равной ширины
        err = regList->ReleaseRegion( right_insertion_size_equal );                     TRACE_PRINT_CONTINUE( err, "Error during RegionsList right-size-equal insertion" );

        // Проверяем корректность RegionsList после вставки справа равной ширины
        td.p_listState_resulted = { 5, 2, 2, 1, 2, 4 };
        td.s_listState_resulted = { 5, 2, 2, 1, 2, 4 };
        td.p_listContent_resulted = { initial_insertion, right_insertion_size_equal };
        td.s_listContent_resulted = utils::SListFromPList( td.p_listContent_resulted );
        rl_check::CheckRegionsList( regList, td, err );                                 TRACE_PRINT_CONTINUE( err, "RegionsList is broken after right-size-equal insertion" );

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
    auto regList = std::make_shared<RegionsList<CELL>>( regList_initCap );

    // Тестовые вставки
    Region_P initial_region{ memoryPitch, pitch_size };

    do {
        // Проверяем RegionsList в процессе инициализации
        rl_check::CheckRegionsListInitialization( regList, regList_initCap, initial_region, Region_P{ memoryPitch, pitch_size }, err );
        TRACE_PRINT_CONTINUE( err, "RegionsList initialization error" );

        // Делаем захват региона максимальной ширины, возвращая RegionsList в начальное состояние
        err = regList->GrabRegion( pitch_size, &grabbedReg );                           TRACE_PRINT_CONTINUE( err, "Error during grabbing region with max size" );

        // Проверяем корректность RegionsList после захвата региона максимальной ширины
        TestData td = { 0 };
        td.p_listState_resulted = { 3, 0, 1, 2, 1, 1 };
        td.s_listState_resulted = { 3, 0, 1, 2, 1, 1 };
        rl_check::CheckRegionsList( regList, td, err );                                 TRACE_PRINT_CONTINUE( err, "RegionsList is broken after grabbing region with max size" );
        
        // Проверяем указатель на захваченный регион
        if (grabbedReg != memoryPitch) {
            err = std::make_shared<Error_Custom>( "Output pointer is incorrect (it should refer to start of memory managed by RegionsList)", PLACE(), "Log.txt" );
            TRACE_PRINT_CONTINUE( err, "GrabRegion() Wrong behavior after grabbing region with max size" );
        }
        grabbedReg = nullptr;

        // RegionsList сейчас пуст. Инициализируем снова, проверяем его в процессе инициализации
        rl_check::CheckRegionsListInitialization( regList, regList_initCap, initial_region, Region_P{ memoryPitch, pitch_size }, err );
        TRACE_PRINT_CONTINUE( err, "RegionsList initialization error" );

        // Делаем захват региона НЕмаксимальной ширины
        err = regList->GrabRegion( grabb_size, &grabbedReg );                           TRACE_PRINT_CONTINUE( err, "Error during grabbing region with non-max size" );

        // Проверяем корректность RegionsList после захвата региона НЕмаксимальной ширины
        td.p_listState_resulted = { 3, 1, 1, 1, 1, 2 };
        td.s_listState_resulted = { 3, 1, 1, 1, 1, 2 };
        td.p_listContent_resulted = { { memoryPitch + grabb_size, 1 } };
        td.s_listContent_resulted = utils::SListFromPList( td.p_listContent_resulted );
        td.intermediate_reg = Region_P{ memoryPitch, pitch_size };
        rl_check::CheckRegionsList( regList, td, err );                                 TRACE_PRINT_CONTINUE( err, "RegionsList is broken after grabbing region with non-max size" );

        // Проверяем указатель на захваченный регион
        if (grabbedReg != memoryPitch) {
            err = std::make_shared<Error_Custom>( "Output pointer is incorrect (it should refer to start of memory managed by RegionsList)", PLACE(), "Log.txt" );
            TRACE_PRINT_CONTINUE( err, "GrabRegion() Wrong behavior after grabbing  region with non-max size" );
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
                    test_step.intermediate_reg = { Ins_complex::mem, (size_t)Ins_complex::Bounds::MEM_SIZE };       // Теперь в TestData::intermediate_reg записано значение Bounds 

                    std::shared_ptr<RegionsList<CELL>> tested_regList = std::make_unique<RegionsList<CELL>>( 0 );

                    do {
                        // Устанавливаем RegionsList в исходное состояние, проверяем
                        rl_manip::SetupRegionsList( tested_regList, test_step, err );                               TRACE_CONTINUE( err, "RegionsList setting-up error in step:\n" + test_step.to_String() );
                        rl_check::CheckRegionsList( tested_regList, test_step, err, true, true, true, false );      TRACE_CONTINUE( err, "RegionsList is broken after setting-up in step:\n" + test_step.to_String() );

                        // Производим захват
                        CELL* grabbedReg = marker;
                        err = tested_regList->GrabRegion( req_size_exp_start.size, &grabbedReg );

                        // Если ожидаемый адрес захваченного региона - null, значит запрашиваемый регион слишком велик. GrabRegion() должна вернуть ошибку, а выходной указатель - остаться неизменённым
                        if (req_size_exp_start.start == nullptr) {
                            if (!err) {
                                err = std::make_shared<Error_Custom>( "Incorrect condition didn't handled (Trying to grab the region with size greater than S-List has)", PLACE(), "Log.txt" );
                                TRACE_CONTINUE( err, "GrabRegion() Wrong behavior" );
                            }
                            if (grabbedReg != marker) {
                                err = std::make_shared<Error_Custom>( "Output pointer has been changed during the trying to grab the region with size greater than S-List has", PLACE(), "Log.txt" );
                                TRACE_CONTINUE( err, "GrabRegion() Wrong behavior" );
                            }
                            err.reset();
                        }
                        // Если указатель на захваченный регион не совпадает с ожидаемым
                        else if( grabbedReg != req_size_exp_start.start )
                        {
                            err = std::make_shared<Error_Custom>( "Output pointer refers to unexpected memory address", PLACE(), "Log.txt" );
                            TRACE_CONTINUE( err, "GrabRegion() Wrong behavior" );
                        }
                        // Проверяем RegionsList теперь
                        rl_check::CheckRegionsList( tested_regList, test_step, err );                               TRACE_CONTINUE( err, "RegionsList is broken after grabbing in step:\n" + test_step.to_String() );

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
                        t_data.intermediate_reg = { Ins_complex::mem, (size_t)Ins_complex::Bounds::MEM_SIZE };       // Теперь в TestData::intermediate_reg записано значение Bounds 

                        // Устанавливаем RegionsList в исходное состояние, проверяем
                        rl_manip::SetupRegionsList( regList, t_data, err );                                         TRACE_CONTINUE( err, "RegionsList setting-up error in step:\n" + t_data.to_String() + "\n" + mnemo);
                        rl_check::CheckRegionsList( regList, t_data, err, true, true, true, false );                TRACE_CONTINUE( err, "RegionsList is broken after setting-up in step:\n" + t_data.to_String() + "\n" + mnemo );

                        // Производим вставку
                        err = regList->ReleaseRegion( released_reg );                                               TRACE_CONTINUE( err, "Error during region insertion in step:\n" + t_data.to_String() + "\n" + mnemo );
                        
                        // Проверяем RegionsList теперь
                        rl_check::CheckRegionsList( regList, t_data, err );                                         TRACE_CONTINUE( err, "RegionsList is broken after insertion in step:\n" + t_data.to_String() + "\n" + mnemo );
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
void RegionsList_Tester::Test_GrabbingsInsertionsRandom()
{
    size_t transactions = 10000;
    size_t count_grab = 0;
    size_t count_release = 0;
    std::chrono::nanoseconds release_time_ns( 0 );
    std::chrono::nanoseconds grab_time_ns( 0 );

    std::mutex mut;
    std::condition_variable cond;
    bool flag = false;
    bool stop = false;

    Log::test( "Log.txt" ) << LogTest::Start{ "Test Grabbings Insertions Random", transactions };

    size_t pitch_size = 1000;
    size_t max_grab_size = 100;
    size_t min_grab_size = 1;
    CELL* memoryPitch = new CELL[pitch_size];
    CELL* grabbedReg = nullptr;

    uint8_t initial_val = utils::random_int( 0, 200 );      // Этим значением будут инициализированы ячейки памяти

    // Пишем в память контрольные значения
    for (size_t i = 0; i < pitch_size; ++i) {
        memoryPitch[i] = CELL( initial_val );
    }

    std::vector<Region_P> grabbedRegions;       // Промежуточный вектор с захваченными регионами
    TestData td = {};
    Error_BasePtr err;
    std::shared_ptr<Error_RegionsList> grab_err;
    std::list<std::pair<std::vector<TestData>, std::vector<TestData>>> performed_transactions;

    // Создаём/инициализируем тестируемый RegionsList
    Region_P initial_reg{ memoryPitch, pitch_size };
    size_t initial_capacity = 0;
    auto regList = std::make_shared<RegionsList<CELL>>( initial_capacity );
    auto initial_td = rl_check::CheckRegionsListInitialization( regList, initial_capacity, initial_reg, initial_reg, err );     TRACE_PRINT_RETURN_NONE( err, "RegionsList initialization error" );
    rl_manip::GetRegionsListDetails( regList, td, err );                                                                        TRACE_PRINT_RETURN_NONE( err, "Can't get RegionsList details after initialization" );

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
                    grab_err = std::dynamic_pointer_cast<Error_RegionsList, Error_Base>( regList->GrabRegion( aquired_size, &grabbedReg ));
                    auto stop = std::chrono::high_resolution_clock::now();

                    // Запоминаем результирующее состояние после попытки захвата
                    td.intermediate_reg = { grabbedReg, aquired_size };
                    rl_manip::GetRegionsListDetails( regList, td, err );                                    TRACE_BREAK( err, "Can't get RegionsList details after another one Grab" );
                    performed_transactions.back().first.push_back( td );

                    // Проверяем RegionsList
                    rl_check::CheckRegionsList( regList, td, err, false, false, false );                    TRACE_BREAK( err, "RegionsList is broken after another one Grab" );
                    rl_check::CheckIfContentOutOfBounds( td.p_listContent_resulted, initial_reg, err );     TRACE_BREAK( err, "Element from P-List is out of bounds after another one Grab" );
                    rl_check::CheckIfContentOutOfBounds( td.s_listContent_resulted, initial_reg, err );     TRACE_BREAK( err, "Element from P-List is out of bounds after another one Grab" );

                    // Если в GrabRegion() словили ошибку "Захват из пустого списка" или "Регион подходящей ширины не найден" более 5 раз, или другую ошибку - покидаем цикл
                    if (grab_err) {
                        if (attempts_counter++ >= 5 || (grab_err->Type() != RegList_ErrType::ERR_GRAB_FROM_EMPTY_LIST && grab_err->Type() != RegList_ErrType::ERR_CONSISTENT_REG_NOTFOUND))
                            break;
                        else
                            continue;
                    }
                    grabbedRegions.push_back( td.intermediate_reg );

                    // Плюсуем счётчик и время успешных захватов
                    grab_time_ns += (stop - start);
                    count_grab++;

                } while (1);

                // Если возникла непредусмотренная ошибка - логируем и сигналим "stop"
                if (err || (grab_err->Type() != RegList_ErrType::ERR_GRAB_FROM_EMPTY_LIST && grab_err->Type() != RegList_ErrType::ERR_CONSISTENT_REG_NOTFOUND))
                {
                    if (err) { err->Print(); }
                    if (grab_err) { grab_err->Print(); }
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
                        Log::warning() << "Nothing to release!" << Log::endlog{};
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
                    err = std::dynamic_pointer_cast<Error_RegionsList, Error_Base>(regList->ReleaseRegion( td.intermediate_reg ));
                    auto stop = std::chrono::high_resolution_clock::now();

                    // Запоминаем результирующее состояние после релиза
                    rl_manip::GetRegionsListDetails( regList, td, err );                                    TRACE_BREAK( err, "Can't get RegionsList details after another one Release" );
                    performed_transactions.back().second.push_back( td );

                    // Проверяем RegionsList
                    rl_check::CheckRegionsList( regList, td, err, false, false, false );                    TRACE_BREAK( err, "RegionsList is broken after another one Release" );
                    rl_check::CheckIfContentOutOfBounds( td.p_listContent_resulted, initial_reg, err );     TRACE_BREAK( err, "Element from P-List is out of bounds after another one Release" );
                    rl_check::CheckIfContentOutOfBounds( td.s_listContent_resulted, initial_reg, err );     TRACE_BREAK( err, "Element from P-List is out of bounds after another one Release" );

                    // Плюсуем счётчик и время успешных освобождений
                    release_time_ns += (stop - start);
                    count_release++;
                }
                // В случае любой ошибки - логируем и сигналим "stop"
                if (err)
                {
                    if (err) { err->Print(); }
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
    if (err || (grab_err->Type() != RegList_ErrType::ERR_GRAB_FROM_EMPTY_LIST && grab_err->Type() != RegList_ErrType::ERR_CONSISTENT_REG_NOTFOUND)) {
        log_transactions( performed_transactions, &Log::debug("Log.txt") );
        Log::test( "Log.txt" ) << LogTest::Finished{ false };
        return;
    }
    do {
        td.intermediate_reg = initial_reg;
        rl_check::CheckRegionsList( regList, td, err, false, false, true );                     TRACE_PRINT_CONTINUE( err, "RegionsList is broken after all transactions" );

        // Освобождаем оставшиеся регионы (это должно привести RegList в исходное состояние с 1 регионом)
        for (auto& reg : grabbedRegions) {
            err = regList->ReleaseRegion( reg );
            if (err) { break; }
        }
        TRACE_PRINT_CONTINUE( err, "Can't release the region is remaining in a list of grabbed regions" );

        // Проверяем RegionsList теперь - он должен был вернуться в исходное состояние (как при инициализации). State не проверяется, т.к. он изменился из-за расширений списков.
        rl_check::CheckRegionsList( regList, initial_td, err, false, true, true );              TRACE_PRINT_CONTINUE(err, "RegionsList is broken after all the regions has been released (initial state expected)" );

        // Проверяем состояние (только число регионов в списке)
        if (rl_manip::GetState<Region_P>( regList ).size != 1) {
            err = std::make_shared<Error_Custom>( "P-State.size != 1", PLACE(), "Log.txt" );    TRACE_PRINT_CONTINUE(err, "RegionsList is broken after all the regions has been released (initial state expected)" );
        }
        if (rl_manip::GetState<Region_S>( regList ).size != 1) {
            err = std::make_shared<Error_Custom>( "S-State.size != 1", PLACE(), "Log.txt" );    TRACE_PRINT_CONTINUE(err, "RegionsList is broken after all the regions has been released (initial state expected)" );
        }
        // Проверяем память (она должна выглядеть как вначале)
        for (size_t i = 0; i < pitch_size; ++i) {
            if (memoryPitch[i] != initial_val) {
                err = std::make_shared<Error_Custom>( "Memory has different values comparing before Grab/Release manipulations", PLACE(), "Log.txt" );
                break;
            }
        }
        TRACE_PRINT_CONTINUE( err, "Memory error" );
    } while (0);


    if (err) {
        log_transactions( performed_transactions, &Log::debug( "Log.txt" ) );
    }

    Log::info( "Log.txt" ) << "Time spent for " << count_grab << " Grabbs:   " << std::chrono::duration_cast<std::chrono::milliseconds>(grab_time_ns).count() << "ms (average " << grab_time_ns.count()/count_grab << " ns)\n"
                           << "Time spent for " << count_release << " Releases: " << std::chrono::duration_cast<std::chrono::milliseconds>(release_time_ns).count() << "ms (average " << release_time_ns.count()/count_release << " ns)\n" << Log::endlog{};
    Log::test( "Log.txt" ) << LogTest::Finished{ err == nullptr };
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
            Region_P reg = { Ins_complex::regPtrs[i], initial_sizes[n][i] };
            p_list_initial[i] = reg;
        }
        // Формируем исходный S-список на основе P-списка (сортируем эл-ты сначала по Size, затем по Pointer)
        s_list_initial = utils::SListFromPList( p_list_initial );

        // Перебор состояний LR-полей списка (L0R0, L0R1, L1R0, L1R1)
        for (const auto& f : Ins_complex::FieldStates)
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
                Region_P reg = { Ins_complex::regPtrs[i], sizes[i] };
                p_list_initial[i] = reg;
            }
            // Формируем исходный S-список на основе P-списка (сортируем эл-ты сначала по Size, затем по Pointer)
            s_list_initial = utils::SListFromPList( p_list_initial );

            std::string empty_RegP_str = std::string( utils::to_string( p_list_initial[0] ).size(), ' ' );
            std::string empty_RegS_str = std::string( "[" ) + std::string( utils::to_string( s_list_initial[0] ).size() - 2, '-' ) + std::string( "]" );

            // Формируем вставки. Участков для вставки: "regNum + 1" - в начало, в конец и между регионами списка.
            CELL* start = Ins_complex::mem;
            CELL* stop = Ins_complex::regPtrs[0];

            // Перебор регионов
            for (auto const& i : Ins_complex::InsertionPlaces)
            {
                std::vector<Region_P> insertions;       // Вставки на данном участке
                size_t insertPosNum = stop - start;     // Число позиций для вставки на участке

                for (size_t p = 0; p < insertPosNum; ++p) { // Перебор всех позиций участка
                    for (size_t s = 1; s <= insertPosNum - p; ++s) {    // Перебор возможных size вставки
                        insertions.push_back( { start + p, s } );   // Генерим вставку
                    }
                }
                // Перебор состояний LR-полей списка (L0R0, L0R1, L1R0, L1R1)
                for (const auto& f : Ins_complex::FieldStates)
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
                                Ins_complex::S_ActionType combo = Ins_complex::SListActionDetermination( ins_side, del_1_side, Side_NONE );

                                std::tie( p_mnemonic, tdata.p_listState_initial, tdata.p_listState_resulted ) = Ins_complex::P_States[f][Ins_complex::Start_P1_Adj_R];
                                std::tie( s_mnemonic, tdata.s_listState_initial, tdata.s_listState_resulted ) = Ins_complex::S_States[f][combo];
                                tdata.p_listContent_resulted = p_list_resulted;
                                tdata.s_listContent_resulted = utils::SListFromPList( p_list_resulted );
                            }
                            // |# [0]   [1]   [2]   [3]   [4]  |
                            else {
                                p_list_resulted.insert( p_list_resulted.begin(), reg );
                                Side ins_side = utils::Find_SList_InsertionSide( s_list_resulted, { reg.start, reg.size, 0 } );
                                Ins_complex::S_ActionType combo = Ins_complex::SListActionDetermination( ins_side, Side_NONE, Side_NONE );

                                std::tie( p_mnemonic, tdata.p_listState_initial, tdata.p_listState_resulted ) = Ins_complex::P_States[f][Ins_complex::Start_P1_Adj_None];
                                std::tie( s_mnemonic, tdata.s_listState_initial, tdata.s_listState_resulted ) = Ins_complex::S_States[f][combo];
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
                                Ins_complex::S_ActionType combo = Ins_complex::SListActionDetermination( ins_side, del_1_side, Side_NONE );

                                std::tie( p_mnemonic, tdata.p_listState_initial, tdata.p_listState_resulted ) = Ins_complex::P_States[f][Ins_complex::P5_Finish_Adj_L];
                                std::tie( s_mnemonic, tdata.s_listState_initial, tdata.s_listState_resulted ) = Ins_complex::S_States[f][combo];
                                tdata.p_listContent_resulted = p_list_resulted;
                                tdata.s_listContent_resulted = utils::SListFromPList( p_list_resulted );
                            }
                            // |  [0]   [1]   [2]   [3]   [4] #|
                            else {
                                p_list_resulted.insert( p_list_resulted.end(), reg );
                                Side ins_side = utils::Find_SList_InsertionSide( s_list_resulted, { reg.start, reg.size, 0 } );
                                Ins_complex::S_ActionType combo = Ins_complex::SListActionDetermination( ins_side, Side_NONE, Side_NONE );

                                std::tie( p_mnemonic, tdata.p_listState_initial, tdata.p_listState_resulted ) = Ins_complex::P_States[f][Ins_complex::P5_Finish_Adj_None];
                                std::tie( s_mnemonic, tdata.s_listState_initial, tdata.s_listState_resulted ) = Ins_complex::S_States[f][combo];
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
                                case InsertionPlace::P1_P2: { std::tie( p_mnemonic, tdata.p_listState_initial, tdata.p_listState_resulted ) = Ins_complex::P_States[f][Ins_complex::P1_P2_Adj_RL]; break; } // |  [0]###[1]   [2]   [3]   [4]  |
                                case InsertionPlace::P2_P3: { std::tie( p_mnemonic, tdata.p_listState_initial, tdata.p_listState_resulted ) = Ins_complex::P_States[f][Ins_complex::P2_P3_Adj_RL]; break; } // |  [0]   [1]###[2]   [3]   [4]  |
                                case InsertionPlace::P3_P4: { std::tie( p_mnemonic, tdata.p_listState_initial, tdata.p_listState_resulted ) = Ins_complex::P_States[f][Ins_complex::P3_P4_Adj_RL]; break; } // |  [0]   [1]   [2]###[3]   [4]  |
                                case InsertionPlace::P4_P5: { std::tie( p_mnemonic, tdata.p_listState_initial, tdata.p_listState_resulted ) = Ins_complex::P_States[f][Ins_complex::P4_P5_Adj_RL]; break; } // |  [0]   [1]   [2]   [3]###[4]  |
                                }

                                to_del_1 = utils::RegionPtoS( *left );
                                to_del_2 = utils::RegionPtoS( *right );
                                left->size += reg.size + right->size;
                                to_ins = utils::RegionPtoS( *left );
                                p_list_resulted.erase( p_list_resulted.begin() + i );

                                Side del_1_side = utils::DeleteRegion<RegionP, CELL>( s_list_resulted, to_del_1 );
                                Side del_2_side = utils::DeleteRegion<RegionP, CELL>( s_list_resulted, to_del_2 );
                                Side ins_side = utils::Find_SList_InsertionSide( s_list_resulted, to_ins );
                                Ins_complex::S_ActionType combo = Ins_complex::SListActionDetermination( ins_side, del_1_side, del_2_side );

                                std::tie( s_mnemonic, tdata.s_listState_initial, tdata.s_listState_resulted ) = Ins_complex::S_States[f][combo];
                                tdata.p_listContent_resulted = p_list_resulted;
                                tdata.s_listContent_resulted = utils::SListFromPList( p_list_resulted );
                            }
                            else if (utils::CheckAdj( *left, reg, *right ) == Adjacency::Adj_Left)
                            {
                                switch (i) {
                                case InsertionPlace::P1_P2: { std::tie( p_mnemonic, tdata.p_listState_initial, tdata.p_listState_resulted ) = Ins_complex::P_States[f][Ins_complex::P1_P2_Adj_L]; break; } // |  [0]###[1]   [2]   [3]   [4]  |
                                case InsertionPlace::P2_P3: { std::tie( p_mnemonic, tdata.p_listState_initial, tdata.p_listState_resulted ) = Ins_complex::P_States[f][Ins_complex::P2_P3_Adj_L]; break; } // |  [0]   [1]###[2]   [3]   [4]  |
                                case InsertionPlace::P3_P4: { std::tie( p_mnemonic, tdata.p_listState_initial, tdata.p_listState_resulted ) = Ins_complex::P_States[f][Ins_complex::P3_P4_Adj_L]; break; } // |  [0]   [1]   [2]###[3]   [4]  |
                                case InsertionPlace::P4_P5: { std::tie( p_mnemonic, tdata.p_listState_initial, tdata.p_listState_resulted ) = Ins_complex::P_States[f][Ins_complex::P4_P5_Adj_L]; break; } // |  [0]   [1]   [2]   [3]###[4]  |
                                }
                                Side del_1_side = utils::DeleteRegion<RegionP, CELL>( s_list_resulted, utils::RegionPtoS( *left ) );
                                left->size += reg.size;
                                Side ins_side = utils::Find_SList_InsertionSide( s_list_resulted, utils::RegionPtoS( *left ) );
                                Ins_complex::S_ActionType combo = Ins_complex::SListActionDetermination( ins_side, del_1_side, Side_NONE );

                                std::tie( s_mnemonic, tdata.s_listState_initial, tdata.s_listState_resulted ) = Ins_complex::S_States[f][combo];
                                tdata.p_listContent_resulted = p_list_resulted;
                                tdata.s_listContent_resulted = utils::SListFromPList( p_list_resulted );
                            }
                            else if (utils::CheckAdj( *left, reg, *right ) == Adjacency::Adj_NONE)
                            {
                                switch (i) {
                                case InsertionPlace::P1_P2: { std::tie( p_mnemonic, tdata.p_listState_initial, tdata.p_listState_resulted ) = Ins_complex::P_States[f][Ins_complex::P1_P2_Adj_None]; break; } // |  [0]###[1]   [2]   [3]   [4]  |
                                case InsertionPlace::P2_P3: { std::tie( p_mnemonic, tdata.p_listState_initial, tdata.p_listState_resulted ) = Ins_complex::P_States[f][Ins_complex::P2_P3_Adj_None]; break; } // |  [0]   [1]###[2]   [3]   [4]  |
                                case InsertionPlace::P3_P4: { std::tie( p_mnemonic, tdata.p_listState_initial, tdata.p_listState_resulted ) = Ins_complex::P_States[f][Ins_complex::P3_P4_Adj_None]; break; } // |  [0]   [1]   [2]###[3]   [4]  |
                                case InsertionPlace::P4_P5: { std::tie( p_mnemonic, tdata.p_listState_initial, tdata.p_listState_resulted ) = Ins_complex::P_States[f][Ins_complex::P4_P5_Adj_None]; break; } // |  [0]   [1]   [2]   [3]###[4]  |
                                }
                                p_list_resulted.insert( p_list_resulted.begin() + i, reg );
                                Side ins_side = utils::Find_SList_InsertionSide( s_list_resulted, utils::RegionPtoS( reg ) );
                                Ins_complex::S_ActionType combo = Ins_complex::SListActionDetermination( ins_side, Side_NONE, Side_NONE );

                                std::tie( s_mnemonic, tdata.s_listState_initial, tdata.s_listState_resulted ) = Ins_complex::S_States[f][combo];
                                tdata.p_listContent_resulted = p_list_resulted;
                                tdata.s_listContent_resulted = utils::SListFromPList( p_list_resulted );
                            }
                            else {
                                switch (i) {
                                case InsertionPlace::P1_P2: { std::tie( p_mnemonic, tdata.p_listState_initial, tdata.p_listState_resulted ) = Ins_complex::P_States[f][Ins_complex::P1_P2_Adj_R]; break; } // |  [0]###[1]   [2]   [3]   [4]  |
                                case InsertionPlace::P2_P3: { std::tie( p_mnemonic, tdata.p_listState_initial, tdata.p_listState_resulted ) = Ins_complex::P_States[f][Ins_complex::P2_P3_Adj_R]; break; } // |  [0]   [1]###[2]   [3]   [4]  |
                                case InsertionPlace::P3_P4: { std::tie( p_mnemonic, tdata.p_listState_initial, tdata.p_listState_resulted ) = Ins_complex::P_States[f][Ins_complex::P3_P4_Adj_R]; break; } // |  [0]   [1]   [2]###[3]   [4]  |
                                case InsertionPlace::P4_P5: { std::tie( p_mnemonic, tdata.p_listState_initial, tdata.p_listState_resulted ) = Ins_complex::P_States[f][Ins_complex::P4_P5_Adj_R]; break; } // |  [0]   [1]   [2]   [3]###[4]  |
                                }
                                Side del_1_side = utils::DeleteRegion<RegionP, CELL>( s_list_resulted, utils::RegionPtoS( *right ) );
                                right->start = reg.start;
                                right->size += reg.size;
                                Side ins_side = utils::Find_SList_InsertionSide( s_list_resulted, utils::RegionPtoS( *right ) );
                                Ins_complex::S_ActionType combo = Ins_complex::SListActionDetermination( ins_side, del_1_side, Side_NONE );

                                std::tie( s_mnemonic, tdata.s_listState_initial, tdata.s_listState_resulted ) = Ins_complex::S_States[f][combo];
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
                stop = Ins_complex::regPtrs[i + 1];
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
inline void rl_manip::SetState( const ListState& state, RegionsListPtr regList, Error_BasePtr& err )
{
    // Проверяем, что устанавливаемое состояние - корректно
    rl_check::Validate_ListState( state, err );                                         TRACE_RETURN_VOID( err, "Can't pass ListState validation for the state to be set" );

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
        err = std::make_shared<Error_Custom>( "Undefined ListType received: " + std::string( typeid(ListType).name() ), PLACE(), "Log.txt" );
    }
}


template<class ListType>
inline void rl_manip::SetContent( const std::vector<ListType>& content, RegionsListPtr regList, Error_BasePtr& err )
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
        err = std::make_shared<Error_Custom>( "Undefined ListType received: " + std::string( typeid(ListType).name() ), PLACE(), "Log.txt" );
        return;
    }
    if (it > end) {
        err = std::make_shared<Error_Custom>( "List's iterators are in incorrect state (begin > end)", PLACE(), "Log.txt" );
        return;
    }
    if (content.size() != end - it) {
        err = std::make_shared<Error_Custom>(
            "Size of vector to be set must match exactly with List size is set [begin ... end].\nSize of content to be set is: " +
            std::to_string( content.size() ) + "\n" + std::string( typeid(ListType).name() ) + "-List size is: " + std::to_string( end - it ), PLACE(), "Log.txt" );
        return;
    }
    size_t pos = 0;
    for (; it != end; ++it, ++pos) {
        *it = content[pos];
    }
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
inline std::vector<ListType> rl_manip::GetContent( RegionsListPtr regList, Error_BasePtr& err )
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
        err = std::make_shared<Error_Custom>( "Undefined ListType received: " + std::string( typeid(ListType).name() ), PLACE(), "Log.txt" );
        return std::vector<ListType>();
    }
    if (it > end) {
        err = std::make_shared<Error_Custom>( "List iterators are in a wrong state (begin > end)", PLACE(), "Log.txt" );
        return out;
    }
    for (; it != end; ++it) {
        out.push_back( *it );
    }
    return out;
}


void rl_manip::GetRegionsListDetails( RegionsListPtr regList, TestData &td, Error_BasePtr& err, bool to_resulted )
{
    auto &p_state_target = to_resulted ? td.p_listState_resulted : td.p_listState_initial;
    auto &s_state_target = to_resulted ? td.s_listState_resulted : td.s_listState_initial;
    auto &p_content_target = to_resulted ? td.p_listContent_resulted : td.p_listContent_initial;
    auto &s_content_target = to_resulted ? td.s_listContent_resulted : td.s_listContent_initial;
    p_state_target = GetState<Region_P>( regList );
    s_state_target = GetState<Region_S>( regList );
    p_content_target = GetContent<Region_P>( regList, err );                            TRACE_RETURN_VOID( err, "Can't get P-List content" );
    s_content_target = GetContent<Region_S>( regList, err );                            TRACE_RETURN_VOID( err, "Can't get S-List content" );
}


void rl_manip::SetupRegionsList( RegionsListPtr regList, const TestData& settings, Error_BasePtr& err, bool settings_from_initial )
{
    SetState<Region_P>( settings_from_initial ? settings.p_listState_initial : settings.p_listState_resulted, regList, err );           TRACE_RETURN_VOID( err, "Can't set P-List state");
    SetState<Region_S>( settings_from_initial ? settings.s_listState_initial : settings.s_listState_resulted, regList, err );           TRACE_RETURN_VOID( err, "Can't set S-List state");
    SetContent<Region_P>( settings_from_initial ? settings.p_listContent_initial : settings.p_listContent_resulted, regList, err );     TRACE_RETURN_VOID( err, "Can't set P-List content");
    SetContent<Region_S>( settings_from_initial ? settings.s_listContent_initial : settings.s_listContent_resulted, regList, err );     TRACE_RETURN_VOID( err, "Can't set S-List content");

}


void rl_check::CheckMarginsPurity( RegionsListPtr regList, Error_BasePtr& err )
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
            err = std::make_shared<Error_Custom>( "P-List Left margin isn't clean", PLACE(), "Log.txt" );
            return;
        }
    }
    for (auto it = p_list_mar_r; it != p_list_stop; ++it) {
        if (*it != Region_P{ 0 }) {
            err = std::make_shared<Error_Custom>( "P-List Right margin isn't clean", PLACE(), "Log.txt" );
            return;
        }
    }
    for (auto it = s_list_start; it != s_list_mar_l; ++it) {
        if (*it != Region_S{ 0 }) {
            err = std::make_shared<Error_Custom>( "S-List Left margin isn't clean", PLACE(), "Log.txt" );
            return;
        }
    }
    for (auto it = s_list_mar_r; it != s_list_stop; ++it) {
        if (*it != Region_S{ 0 }) {
            err = std::make_shared<Error_Custom>( "S-List Right margin isn't clean", PLACE(), "Log.txt" );
            return;
        }
    }
}


void rl_check::CheckListsCompliance( const std::vector<Region_P>& p_vec, const std::vector<Region_S>& s_vec, Error_BasePtr& err )
{
    std::vector<Region_S> s_list = utils::SListFromPList( p_vec );

    if (s_list != s_vec) {
        err = std::make_shared<Error_Custom>( "P- and S-Lists are not equivalent:\nP: " + utils::to_string( p_vec ) + "\nS: " + utils::to_string( s_vec ), PLACE(), "Log.txt" );
    }
}


ListState rl_check::Validate_ListState( const ListState& state, Error_BasePtr& err )
{
    std::string errMess( "Wrong relations:\n" );
    bool ok = true;
    if (state.capacity < state.size) {
        errMess += "ListState.capacity < ListState.size\n";
        ok = false;
    }
    if (state.spaceLeft + state.spaceRight + state.size != state.capacity) {
        errMess += "ListState.spaceLeft + ListState.spaceRight + ListState.size != ListState.capacity\n";
        ok = false;
    }
    if (state.begin_pos != state.spaceLeft) {
        errMess += "ListState.begin_pos != ListState.spaceLeft\n";
        ok = false;
    }
    if (state.end_pos != state.spaceLeft + state.size) {
        errMess += "ListState.end_pos != ListState.spaceLeft + ListState.size\n";
        ok = false;
    }
    if (!ok) {
        errMess += "Details:\n" + utils::to_string( state );
        err = std::make_shared<Error_Custom>( errMess, PLACE(), "Log.txt" );
    }
    return state;
}


template<class ListType>
void rl_check::CheckFootprintsVsState( const ListState& state, const ListFootprints& footpr, Error_BasePtr& err )
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
        err = std::make_shared<Error_Custom>( errMess, PLACE(), "Log.txt" );
    }
}


bool rl_check::CheckState( const ListState& expected, const ListState& gained, Error_BasePtr& err )
{
    if (expected != gained) {
        err = std::make_shared<Error_Custom>( "Expected:\n" + utils::to_string( expected ) + "\nGained:\n" + utils::to_string( gained ), PLACE(), "Log.txt" );
        return false;
    }
    return true;
}


template<class ListType>
bool rl_check::CheckContent( const std::vector<ListType>& expected, std::vector<ListType>& gained, Error_BasePtr& err )
{
    if (expected.size() != gained.size()) {
        err = std::make_shared<Error_Custom>( "Expected size: " + std::to_string( expected.size() ) + "\nGained size: " + std::to_string( gained.size() ), PLACE(), "Log.txt" );
        return false;
    }
    if constexpr (std::is_same_v<ListType, Region_P> || std::is_same_v<ListType, Region_S>)
    {
        bool ok = true;
        for (size_t i = 0; i < expected.size(); ++i) {
            ok &= expected[i] == gained[i];
        }
        if (!ok) {
            err = std::make_shared<Error_Custom>( "Expected content: " + utils::to_string( expected ) + "\nGained content:   " + utils::to_string( gained ), PLACE(), "Log.txt" );
        }
        return ok;
    }
    else {
        err = std::make_shared<Error_Custom>( "Undefined ListType received: " + std::string( typeid(ListType).name() ), PLACE(), "Log.txt" );
        return false;
    }
}


template<class ListType>
void rl_check::CheckIfContentOutOfBounds( const std::vector<ListType>& content, Region_P bounds, Error_BasePtr& err )
{
    if constexpr (std::is_same_v<ListType, Region_P> || std::is_same_v<ListType, Region_S>)
    {
        std::string errMess;
        for (const auto& reg : content) {
            if (reg.start < bounds.start || reg.start + reg.size > bounds.start + bounds.size) {
                err = std::make_shared<Error_Custom>(
                    "Bounds:    " + utils::to_string( bounds ) + "\nRegion:    " + utils::to_string( reg ) + "\nCell size: " + std::to_string(sizeof(CELL)) + " Bytes\nContent:   " + utils::to_string( content ), PLACE(), "Log.txt" );
                break;
            }
        }
    }
    else {
        err = std::make_shared<Error_Custom>( "Undefined ListType received: " + std::string( typeid(ListType).name() ), PLACE(), "Log.txt" );
        return false;
    }
}


void rl_check::CheckRegionsList( RegionsListPtr regList, const TestData& expected, Error_BasePtr& err, bool check_state, bool check_content, bool check_bounds, bool check_resulted )
{
    rl_check::CheckMarginsPurity( regList, err );                                                                                                   TRACE_RETURN_VOID( err, "Margins are not clean" );
    auto P_Content = rl_manip::GetContent<Region_P>( regList, err );                                                                                TRACE_RETURN_VOID( err, "Can't get P-List content" );
    auto S_Content = rl_manip::GetContent<Region_S>( regList, err );                                                                                TRACE_RETURN_VOID( err, "Can't get S-List content" );
    rl_check::CheckListsCompliance( P_Content, S_Content, err );                                                                                    TRACE_RETURN_VOID( err, "Lists compliance error" );
    ListState P_State = rl_check::Validate_ListState( rl_manip::GetState<Region_P>( regList ), err );                                               TRACE_RETURN_VOID( err, "Incorrect P-State gained" );
    ListState S_State = rl_check::Validate_ListState( rl_manip::GetState<Region_S>( regList ), err );                                               TRACE_RETURN_VOID( err, "Incorrect S-State gained" );
    rl_check::CheckFootprintsVsState<Region_P>( P_State, rl_manip::GetFootprints<Region_P>( regList ), err );                                       TRACE_RETURN_VOID( err, "Incorrect P-State/P-Footprints combo gained" );
    rl_check::CheckFootprintsVsState<Region_S>( S_State, rl_manip::GetFootprints<Region_S>( regList ), err );                                       TRACE_RETURN_VOID( err, "Incorrect S-State/S-Footprints combo gained" );

    if (check_state) {
        rl_check::CheckState( check_resulted ? expected.p_listState_resulted : expected.p_listState_initial, P_State, err );                        TRACE_RETURN_VOID( err, "Unexpected P-State gained" );
        rl_check::CheckState( check_resulted ? expected.s_listState_resulted : expected.s_listState_initial, S_State, err );                        TRACE_RETURN_VOID( err, "Unexpected S-State gained" );
    }
    if (check_content) {
        rl_check::CheckContent<Region_P>( check_resulted ? expected.p_listContent_resulted : expected.p_listContent_initial, P_Content, err );      TRACE_RETURN_VOID( err, "Unexpected P-List content gained" );
        rl_check::CheckContent<Region_S>( check_resulted ? expected.s_listContent_resulted : expected.s_listContent_initial, S_Content, err );      TRACE_RETURN_VOID( err, "Unexpected S-List content gained" );
    }
    if (check_bounds) {
        rl_check::CheckIfContentOutOfBounds<Region_P>( P_Content, expected.intermediate_reg, err );                                                 TRACE_RETURN_VOID( err, "Element from P-List is out of bounds" );
        rl_check::CheckIfContentOutOfBounds<Region_S>( S_Content, expected.intermediate_reg, err );                                                 TRACE_RETURN_VOID( err, "Element from S-List is out of bounds" );
    }
}


TestData rl_check::CheckRegionsListInitialization( RegionsListPtr checkedRegList, size_t init_capacity, const Region_P& init_region, const Region_P& bounds, Error_BasePtr& err )
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

    rl_check::CheckRegionsList( checkedRegList, td, err );                              TRACE_RETURN( err, "RegionsList is broken after creation", td );

    // Инициализирующая вставка
    err = checkedRegList->ReleaseRegion( init_region );                                 TRACE_RETURN( err, "Error during RegionsList initial insertion", td );

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

    rl_check::CheckRegionsList( checkedRegList, td, err );                              TRACE_RETURN( err, "RegionsList is broken after initial insertion", td );
    return td;
}