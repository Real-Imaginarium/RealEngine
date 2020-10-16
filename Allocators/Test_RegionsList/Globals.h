#pragma once
#include "Types.h"

#include <stdint.h>
#include <string>
#include <map>
#include <vector>


// Тестовые типы данных для RegionsList
struct TestCell
{
    TestCell( size_t i = 0 )
        : field_1( 0 ), field_2( 0 ), field_3( 0 ), field_4( 0 ), field_5( 0 ), field_6( 0 ), field_7( i )
    {};
    uint32_t    field_1;
    uint32_t    field_2;
    float       field_3;
    uint16_t    field_4;
    char        field_5;
    char        field_6;
    size_t      field_7;

    TestCell& operator+=( size_t r_op ) {
        this->field_7 += r_op;
        return *this;
    }
    TestCell& operator-=( size_t r_op ) {
        this->field_7 -= r_op;
        return *this;
    }
    bool operator==( size_t r_op ) {
        return (this->field_7 == r_op);
    }
    bool operator!=( size_t r_op ) {
        return !(*this == r_op);
    }
};

struct Descriptor
{
    Descriptor( size_t i = 0 )
        : desc_handle( 0 ), index_in_heap( i )
    {};
    uint64_t desc_handle;
    uint64_t index_in_heap;

    Descriptor& operator+=( size_t r_op ) {
        this->index_in_heap += r_op;
        return *this;
    }
    Descriptor& operator-=( size_t r_op ) {
        this->index_in_heap -= r_op;
        return *this;
    }
    bool operator==( size_t r_op ) {
        return (this->index_in_heap == r_op);
    }
    bool operator!=( size_t r_op ) {
        return !(*this == r_op);
    }
};

/*Раскомментировать нужный тип*/
//using CELL = TestCell;
using CELL = Descriptor;
//using CELL = uint8_t;


class Globals
{
public:
    // Типы и данные для генерации и выполнения тесткейсов для "Test_InsertionsComplex()"
    class InsertionsComplex
    {
    public:
        /* Место вставки */
        enum InsertionPlace {
            Start_P1,
            P1_P2,
            P2_P3,
            P3_P4,
            P4_P5,
            P5_Finish,
            Regions_Count,
        };
        static const InsertionPlace InsertionPlaces[Regions_Count];


        /* Состояние полей (одинаковое для P-List и S-List. Ширина поля - 1 RegionP или RegionS) */
        enum FieldState {
            L0_R0,
            L0_R1,
            L1_R0,
            L1_R1,
            FieldStates_Count
        };
        static const FieldState FieldStates[FieldStates_Count];


        /* Тип вставки в P-List */
        enum P_InsertionType
        {
            Start_P1_Adj_None,      // В начало, несмежная
            Start_P1_Adj_R,         // В начало, смежная
            P1_P2_Adj_None,         // Между 1 и 2 регионами, несмежная
            P1_P2_Adj_L,            // Между 1 и 2 регионами, смежная с левым (1)
            P1_P2_Adj_R,            // Между 1 и 2 регионами, смежная с правым (2)
            P1_P2_Adj_RL,           // Между 1 и 2 регионами, смежная с левым и правым (1 и 2)
            P2_P3_Adj_None,         // Между 2 и 3 регионами, несмежная
            P2_P3_Adj_L,            // Между 2 и 3 регионами, смежная с левым (2)
            P2_P3_Adj_R,            // Между 2 и 3 регионами, смежная с правым (3)
            P2_P3_Adj_RL,           // Между 2 и 3 регионами, смежная с левым и правым (2 и 3)
            P3_P4_Adj_None,         // Между 3 и 4 регионами, несмежная
            P3_P4_Adj_L,            // Между 3 и 4 регионами, смежная с левым (3)
            P3_P4_Adj_R,            // Между 3 и 4 регионами, смежная с правым (4)
            P3_P4_Adj_RL,           // Между 3 и 4 регионами, смежная с левым и правым (3 и 4)
            P4_P5_Adj_None,         // Между 4 и 5 регионами, несмежная
            P4_P5_Adj_L,            // Между 4 и 5 регионами, смежная с левым (4)
            P4_P5_Adj_R,            // Между 4 и 5 регионами, смежная с правым (5)
            P4_P5_Adj_RL,           // Между 4 и 5 регионами, смежная с левым и правым (4 и 5)
            P5_Finish_Adj_None,     // В конец, несмежная
            P5_Finish_Adj_L,        // В конец, смежная
            P_InsertionTypes_Count
        };
        static const P_InsertionType P_InsertionTypes[P_InsertionTypes_Count];


        /* Тип операции с S-List (зависит от типа вставки в P-List) */
        enum S_ActionType {
            IR,             // Вставка справа
            IL,             // Вставка слева
            DR_IR,          // Удаление справа, вставка справа
            DR_IL,          // Удаление справа, вставка слева
            DL_IR,          // Удаление слева, вставка справа
            DL_IL,          // Удаление слева, вставка слева
            DL_DL_IR,       // 2 удаления слева, вставка справа
            DL_DL_IL,       // 2 удаления слева, вставка слева
            DL_DR_IR,       // Удаление слева и справа, вставка справа
            DL_DR_IL,       // Удаление слева и справа, вставка слева
            DR_DL_IR,       // Удаление справа и слева, вставка справа
            DR_DL_IL,       // Удаление справа и слева, вставка слева
            DR_DR_IR,       // 2 удаления справа, вставка справа
            DR_DR_IL,       // 2 удаления справа, вставка слева
            S_ActionTypes_Count
        };
        static const S_ActionType S_ActionTypes[S_ActionTypes_Count];


        // Разметка рабочего участка памяти
        enum class Bounds {
            MEM_SIZE = 37,
            REG_1_START = 4,
            REG_2_START = 10,
            REG_3_START = 16,
            REG_4_START = 22,
            REG_5_START = 28,
        };
        // Рабочий участок памяти для хранения комбинаций 5 равноудалённых регионов разной ширины (максимум 5 CELL), которыми инициализируется RegionsList.
        //
        //          [ ][ ][ ][ ][0][#][#][#][#][ ][1][#][#][#][#][ ][2][#][#][#][#][ ][3][#][#][#][#][ ][4][#][#][#][#][ ][ ][ ][ ]
        static CELL mem[(size_t)Bounds::MEM_SIZE];


        // Указатели на начала 5 равноудалённых регионов + указатель на конец памяти
        static CELL* const regPtrs[Regions_Count];


        // Набор исходных и результирующих состояний P- и S-List и мнемоника тесткейса
        static const std::tuple<std::string, ListState, ListState> P_States[FieldStates_Count][P_InsertionTypes_Count];

        static const std::tuple<std::string, ListState, ListState> S_States[FieldStates_Count][S_ActionTypes_Count];


        // Определяет тип проделанной операции с S-List.
        static S_ActionType SListActionDetermination( Side ins, Side del1, Side del2 );
    };

    // Типы и данные для Test_ListsReorganise(); 
    class ReorganizeComplex
    {
    public:
        // Рабочий участок памяти
        static const size_t mem_size = 859;
        static CELL m[mem_size];

        // Регионы участка памяти, для вставки в RegionsList с целью его реорганизации (сдвигов контента или расширения)
        static const size_t regs_count = 40;
        static RegionP<CELL> r[regs_count];

        // Тесткейсы, каждый содержит: исходное и результирующее состояние (одинаковое для P-и-S-List), исходный и результирующий
        // контент P-List (S-List восстанавливается из него в тесте), вставка. Тесткейсы независимы.
        static const std::tuple<ListState, ListState, std::vector<RegionP<CELL>>, std::vector<RegionP<CELL>>, RegionP<CELL>> test_cases[66];

        // Последовательность тесткейсов для многократной реорганизации RegionsList. Каждая содержит: исходное и результирующее состояние (одинаковое для P-и-S-List),
        // набор последовательных вставок и захватов (true - вставка, false - захват, см. сигнатуру) для реорганизации RegionsList и результирующий контент.
        static const std::tuple<ListState, ListState, std::vector<std::tuple<RegionP<CELL>, bool>>, std::vector<RegionP<CELL>>> test_sequences[6];
    };
};