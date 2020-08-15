#include "Globals.h"

typedef Globals::Ins_complex Ins_complex;


/* ћеста вставки */
const Ins_complex::InsertionPlace Ins_complex::InsertionPlaces[Regions_Count] = { Start_P1, P1_P2, P2_P3, P3_P4, P4_P5, P5_Finish };


/* —осто€ни€ полей (одинаковые дл€ P-List и S-List. Ўирина пол€ - 1 RegionP или RegionS) */
const Ins_complex::FieldState Ins_complex::FieldStates[FieldStates_Count] = { L0_R0, L0_R1, L1_R0, L1_R1 };


/* “ипы вставок в P-List */
const Ins_complex::P_InsertionType Ins_complex::P_InsertionTypes[P_InsertionTypes_Count] =
{
    Start_P1_Adj_None, Start_P1_Adj_R, P1_P2_Adj_None, P1_P2_Adj_L, P1_P2_Adj_R, P1_P2_Adj_RL, P2_P3_Adj_None, P2_P3_Adj_L, P2_P3_Adj_R, P2_P3_Adj_RL,
    P3_P4_Adj_None, P3_P4_Adj_L, P3_P4_Adj_R, P3_P4_Adj_RL, P4_P5_Adj_None, P4_P5_Adj_L, P4_P5_Adj_R, P4_P5_Adj_RL, P5_Finish_Adj_None, P5_Finish_Adj_L
};


/* “ип операций с S-List (зависит от типа вставки в P-List) */
const Ins_complex::S_ActionType Ins_complex::S_ActionTypes[S_ActionTypes_Count] =
{
    IR, IL, DR_IR, DR_IL, DL_IR, DL_IL, DL_DL_IR, DL_DL_IL, DL_DR_IR, DL_DR_IL, DR_DL_IR, DR_DL_IL, DR_DR_IR, DR_DR_IL,
};


/*_____________________________________________________________________________________________
*       –абочий  участок  пам€ти,  дл€  хранени€  комбинаций 5 равноудалЄнных регионов разной |
*   ширины,  которыми  инициализируетс€  тестируемый  RegionsList.  –ассто€ние между началами |
*   регионов - 6 CELL. ћаксимальна€ ширина региона, соответственно, 5 CELL.                   |
*           | | | | |0|-|-|-|-| |1|-|-|-|-| |2|-|-|-|-| |3|-|-|-|-| |4|-|-|-|-| | | | |       |
*____________________________________________________________________________________________*/
CELL Ins_complex::mem[37];


/* ”казатели на начала 5 равноудалЄнных регионов + указатель на конец пам€ти */
CELL* const Ins_complex::regPtrs[Regions_Count] = {
    mem + (size_t)Bounds::REG_1_START,
    mem + (size_t)Bounds::REG_2_START,
    mem + (size_t)Bounds::REG_3_START,
    mem + (size_t)Bounds::REG_4_START,
    mem + (size_t)Bounds::REG_5_START,
    mem + (size_t)Bounds::MEM_SIZE
};


/*_____________________________________________________________________________________________
*       ’ранилище  строк  состо€ний  P-List  до и после  вставки  в  зависимости от исходного |
*   состо€ни€ полей и типа вставки в P-List. явл€ютс€ частью строкового  представлени€  теста |
*   "ComplexInsertions" (его вариантов).                                                      |
*____________________________________________________________________________________________*/
const std::tuple<std::string, ListState, ListState> Ins_complex::P_States[FieldStates_Count][P_InsertionTypes_Count] =
{
    {
        {"|0|# [0]   [1]   [2]   [3]   [4]  |0|", ListState{5,5,0,0,0,5}, ListState{10,6,1,3,1,7}},
        {"|0| #[0]   [1]   [2]   [3]   [4]  |0|", ListState{5,5,0,0,0,5}, ListState{ 5,5,0,0,0,5}},
        {"|0|  [0] # [1]   [2]   [3]   [4]  |0|", ListState{5,5,0,0,0,5}, ListState{10,6,1,3,1,7}},
        {"|0|  [0]#  [1]   [2]   [3]   [4]  |0|", ListState{5,5,0,0,0,5}, ListState{ 5,5,0,0,0,5}},
        {"|0|  [0]  #[1]   [2]   [3]   [4]  |0|", ListState{5,5,0,0,0,5}, ListState{ 5,5,0,0,0,5}},
        {"|0|  [0]###[1]   [2]   [3]   [4]  |0|", ListState{5,5,0,0,0,5}, ListState{ 5,4,1,0,1,5}},
        {"|0|  [0]   [1] # [2]   [3]   [4]  |0|", ListState{5,5,0,0,0,5}, ListState{10,6,2,2,2,8}},
        {"|0|  [0]   [1]#  [2]   [3]   [4]  |0|", ListState{5,5,0,0,0,5}, ListState{ 5,5,0,0,0,5}},
        {"|0|  [0]   [1]  #[2]   [3]   [4]  |0|", ListState{5,5,0,0,0,5}, ListState{ 5,5,0,0,0,5}},
        {"|0|  [0]   [1]###[2]   [3]   [4]  |0|", ListState{5,5,0,0,0,5}, ListState{ 5,4,0,1,0,4}},
        {"|0|  [0]   [1]   [2] # [3]   [4]  |0|", ListState{5,5,0,0,0,5}, ListState{10,6,2,2,2,8}},
        {"|0|  [0]   [1]   [2]#  [3]   [4]  |0|", ListState{5,5,0,0,0,5}, ListState{ 5,5,0,0,0,5}},
        {"|0|  [0]   [1]   [2]  #[3]   [4]  |0|", ListState{5,5,0,0,0,5}, ListState{ 5,5,0,0,0,5}},
        {"|0|  [0]   [1]   [2]###[3]   [4]  |0|", ListState{5,5,0,0,0,5}, ListState{ 5,4,0,1,0,4}},
        {"|0|  [0]   [1]   [2]   [3] # [4]  |0|", ListState{5,5,0,0,0,5}, ListState{10,6,2,2,2,8}},
        {"|0|  [0]   [1]   [2]   [3]#  [4]  |0|", ListState{5,5,0,0,0,5}, ListState{ 5,5,0,0,0,5}},
        {"|0|  [0]   [1]   [2]   [3]  #[4]  |0|", ListState{5,5,0,0,0,5}, ListState{ 5,5,0,0,0,5}},
        {"|0|  [0]   [1]   [2]   [3]###[4]  |0|", ListState{5,5,0,0,0,5}, ListState{ 5,4,0,1,0,4}},
        {"|0|  [0]   [1]   [2]   [3]   [4] #|0|", ListState{5,5,0,0,0,5}, ListState{10,6,2,2,2,8}},
        {"|0|  [0]   [1]   [2]   [3]   [4]# |0|", ListState{5,5,0,0,0,5}, ListState{ 5,5,0,0,0,5}}
    },
    {
        {"|0|# [0]   [1]   [2]   [3]   [4]  |1|", ListState{6,5,0,1,0,5}, ListState{12,6,2,4,2,8}},
        {"|0| #[0]   [1]   [2]   [3]   [4]  |1|", ListState{6,5,0,1,0,5}, ListState{ 6,5,0,1,0,5}},
        {"|0|  [0] # [1]   [2]   [3]   [4]  |1|", ListState{6,5,0,1,0,5}, ListState{12,6,2,4,2,8}},
        {"|0|  [0]#  [1]   [2]   [3]   [4]  |1|", ListState{6,5,0,1,0,5}, ListState{ 6,5,0,1,0,5}},
        {"|0|  [0]  #[1]   [2]   [3]   [4]  |1|", ListState{6,5,0,1,0,5}, ListState{ 6,5,0,1,0,5}},
        {"|0|  [0]###[1]   [2]   [3]   [4]  |1|", ListState{6,5,0,1,0,5}, ListState{ 6,4,1,1,1,5}},
        {"|0|  [0]   [1] # [2]   [3]   [4]  |1|", ListState{6,5,0,1,0,5}, ListState{ 6,6,0,0,0,6}},
        {"|0|  [0]   [1]#  [2]   [3]   [4]  |1|", ListState{6,5,0,1,0,5}, ListState{ 6,5,0,1,0,5}},
        {"|0|  [0]   [1]  #[2]   [3]   [4]  |1|", ListState{6,5,0,1,0,5}, ListState{ 6,5,0,1,0,5}},
        {"|0|  [0]   [1]###[2]   [3]   [4]  |1|", ListState{6,5,0,1,0,5}, ListState{ 6,4,0,2,0,4}},
        {"|0|  [0]   [1]   [2] # [3]   [4]  |1|", ListState{6,5,0,1,0,5}, ListState{ 6,6,0,0,0,6}},
        {"|0|  [0]   [1]   [2]#  [3]   [4]  |1|", ListState{6,5,0,1,0,5}, ListState{ 6,5,0,1,0,5}},
        {"|0|  [0]   [1]   [2]  #[3]   [4]  |1|", ListState{6,5,0,1,0,5}, ListState{ 6,5,0,1,0,5}},
        {"|0|  [0]   [1]   [2]###[3]   [4]  |1|", ListState{6,5,0,1,0,5}, ListState{ 6,4,0,2,0,4}},
        {"|0|  [0]   [1]   [2]   [3] # [4]  |1|", ListState{6,5,0,1,0,5}, ListState{ 6,6,0,0,0,6}},
        {"|0|  [0]   [1]   [2]   [3]#  [4]  |1|", ListState{6,5,0,1,0,5}, ListState{ 6,5,0,1,0,5}},
        {"|0|  [0]   [1]   [2]   [3]  #[4]  |1|", ListState{6,5,0,1,0,5}, ListState{ 6,5,0,1,0,5}},
        {"|0|  [0]   [1]   [2]   [3]###[4]  |1|", ListState{6,5,0,1,0,5}, ListState{ 6,4,0,2,0,4}},
        {"|0|  [0]   [1]   [2]   [3]   [4] #|1|", ListState{6,5,0,1,0,5}, ListState{ 6,6,0,0,0,6}},
        {"|0|  [0]   [1]   [2]   [3]   [4]# |1|", ListState{6,5,0,1,0,5}, ListState{ 6,5,0,1,0,5}}
    },
    {
        {"|0|# [0]   [1]   [2]   [3]   [4]  |1|", ListState{6,5,0,1,0,5}, ListState{12,6,2,4,2,8}},
        {"|0| #[0]   [1]   [2]   [3]   [4]  |1|", ListState{6,5,0,1,0,5}, ListState{ 6,5,0,1,0,5}},
        {"|0|  [0] # [1]   [2]   [3]   [4]  |1|", ListState{6,5,0,1,0,5}, ListState{12,6,2,4,2,8}},
        {"|0|  [0]#  [1]   [2]   [3]   [4]  |1|", ListState{6,5,0,1,0,5}, ListState{ 6,5,0,1,0,5}},
        {"|0|  [0]  #[1]   [2]   [3]   [4]  |1|", ListState{6,5,0,1,0,5}, ListState{ 6,5,0,1,0,5}},
        {"|0|  [0]###[1]   [2]   [3]   [4]  |1|", ListState{6,5,0,1,0,5}, ListState{ 6,4,1,1,1,5}},
        {"|0|  [0]   [1] # [2]   [3]   [4]  |1|", ListState{6,5,0,1,0,5}, ListState{ 6,6,0,0,0,6}},
        {"|0|  [0]   [1]#  [2]   [3]   [4]  |1|", ListState{6,5,0,1,0,5}, ListState{ 6,5,0,1,0,5}},
        {"|0|  [0]   [1]  #[2]   [3]   [4]  |1|", ListState{6,5,0,1,0,5}, ListState{ 6,5,0,1,0,5}},
        {"|0|  [0]   [1]###[2]   [3]   [4]  |1|", ListState{6,5,0,1,0,5}, ListState{ 6,4,0,2,0,4}},
        {"|0|  [0]   [1]   [2] # [3]   [4]  |1|", ListState{6,5,0,1,0,5}, ListState{ 6,6,0,0,0,6}},
        {"|0|  [0]   [1]   [2]#  [3]   [4]  |1|", ListState{6,5,0,1,0,5}, ListState{ 6,5,0,1,0,5}},
        {"|0|  [0]   [1]   [2]  #[3]   [4]  |1|", ListState{6,5,0,1,0,5}, ListState{ 6,5,0,1,0,5}},
        {"|0|  [0]   [1]   [2]###[3]   [4]  |1|", ListState{6,5,0,1,0,5}, ListState{ 6,4,0,2,0,4}},
        {"|0|  [0]   [1]   [2]   [3] # [4]  |1|", ListState{6,5,0,1,0,5}, ListState{ 6,6,0,0,0,6}},
        {"|0|  [0]   [1]   [2]   [3]#  [4]  |1|", ListState{6,5,0,1,0,5}, ListState{ 6,5,0,1,0,5}},
        {"|0|  [0]   [1]   [2]   [3]  #[4]  |1|", ListState{6,5,0,1,0,5}, ListState{ 6,5,0,1,0,5}},
        {"|0|  [0]   [1]   [2]   [3]###[4]  |1|", ListState{6,5,0,1,0,5}, ListState{ 6,4,0,2,0,4}},
        {"|0|  [0]   [1]   [2]   [3]   [4] #|1|", ListState{6,5,0,1,0,5}, ListState{ 6,6,0,0,0,6}},
        {"|0|  [0]   [1]   [2]   [3]   [4]# |1|", ListState{6,5,0,1,0,5}, ListState{ 6,5,0,1,0,5}}
    },
    {
        {"|1|# [0]   [1]   [2]   [3]   [4]  |1|", ListState{7,5,1,1,1,6}, ListState{ 7,6,0,1,0,6}},
        {"|1| #[0]   [1]   [2]   [3]   [4]  |1|", ListState{7,5,1,1,1,6}, ListState{ 7,5,1,1,1,6}},
        {"|1|  [0] # [1]   [2]   [3]   [4]  |1|", ListState{7,5,1,1,1,6}, ListState{ 7,6,0,1,0,6}},
        {"|1|  [0]#  [1]   [2]   [3]   [4]  |1|", ListState{7,5,1,1,1,6}, ListState{ 7,5,1,1,1,6}},
        {"|1|  [0]  #[1]   [2]   [3]   [4]  |1|", ListState{7,5,1,1,1,6}, ListState{ 7,5,1,1,1,6}},
        {"|1|  [0]###[1]   [2]   [3]   [4]  |1|", ListState{7,5,1,1,1,6}, ListState{ 7,4,2,1,2,6}},
        {"|1|  [0]   [1] # [2]   [3]   [4]  |1|", ListState{7,5,1,1,1,6}, ListState{ 7,6,1,0,1,7}},
        {"|1|  [0]   [1]#  [2]   [3]   [4]  |1|", ListState{7,5,1,1,1,6}, ListState{ 7,5,1,1,1,6}},
        {"|1|  [0]   [1]  #[2]   [3]   [4]  |1|", ListState{7,5,1,1,1,6}, ListState{ 7,5,1,1,1,6}},
        {"|1|  [0]   [1]###[2]   [3]   [4]  |1|", ListState{7,5,1,1,1,6}, ListState{ 7,4,1,2,1,5}},
        {"|1|  [0]   [1]   [2] # [3]   [4]  |1|", ListState{7,5,1,1,1,6}, ListState{ 7,6,1,0,1,7}},
        {"|1|  [0]   [1]   [2]#  [3]   [4]  |1|", ListState{7,5,1,1,1,6}, ListState{ 7,5,1,1,1,6}},
        {"|1|  [0]   [1]   [2]  #[3]   [4]  |1|", ListState{7,5,1,1,1,6}, ListState{ 7,5,1,1,1,6}},
        {"|1|  [0]   [1]   [2]###[3]   [4]  |1|", ListState{7,5,1,1,1,6}, ListState{ 7,4,1,2,1,5}},
        {"|1|  [0]   [1]   [2]   [3] # [4]  |1|", ListState{7,5,1,1,1,6}, ListState{ 7,6,1,0,1,7}},
        {"|1|  [0]   [1]   [2]   [3]#  [4]  |1|", ListState{7,5,1,1,1,6}, ListState{ 7,5,1,1,1,6}},
        {"|1|  [0]   [1]   [2]   [3]  #[4]  |1|", ListState{7,5,1,1,1,6}, ListState{ 7,5,1,1,1,6}},
        {"|1|  [0]   [1]   [2]   [3]###[4]  |1|", ListState{7,5,1,1,1,6}, ListState{ 7,4,1,2,1,5}},
        {"|1|  [0]   [1]   [2]   [3]   [4] #|1|", ListState{7,5,1,1,1,6}, ListState{ 7,6,1,0,1,7}},
        {"|1|  [0]   [1]   [2]   [3]   [4]# |1|", ListState{7,5,1,1,1,6}, ListState{ 7,5,1,1,1,6}}
    }
};


/*____________________________________________________________________________________________
*       ’ранилище  строк  состо€ний  S-List  до и после  вставки  в  зависимости от исходного |
*   состо€ни€ полей и типа операции с S-List. явл€ютс€ частью строкового представлени€  теста |
*   "ComplexInsertions" (его вариантов).                                                      |
*____________________________________________________________________________________________*/
const std::tuple<std::string, ListState, ListState> Ins_complex::S_States[FieldStates_Count][S_ActionTypes_Count] =
{
    {
        {"|0|           I1-> |0|", ListState{5,5,0,0,0,5}, ListState{10,6,2,2,2,8}},
        {"|0|           I1<- |0|", ListState{5,5,0,0,0,5}, ListState{10,6,1,3,1,7}},
        {"|0| D1->      I1-> |0|", ListState{5,5,0,0,0,5}, ListState{ 5,5,0,0,0,5}},
        {"|0| D1->      I1<- |0|", ListState{5,5,0,0,0,5}, ListState{10,5,2,3,2,7}},
        {"|0| D1<-      I1-> |0|", ListState{5,5,0,0,0,5}, ListState{10,5,3,2,3,8}},
        {"|0| D1<-      I1<- |0|", ListState{5,5,0,0,0,5}, ListState{ 5,5,0,0,0,5}},
        {"|0| D1<- D2<- I1-> |0|", ListState{5,5,0,0,0,5}, ListState{10,4,3,3,3,7}},
        {"|0| D1<- D2<- I1<- |0|", ListState{5,5,0,0,0,5}, ListState{ 5,4,1,0,1,5}},
        {"|0| D1<- D2-> I1-> |0|", ListState{5,5,0,0,0,5}, ListState{ 5,4,1,0,1,5}},
        {"|0| D1<- D2-> I1<- |0|", ListState{5,5,0,0,0,5}, ListState{ 5,4,0,1,0,4}},
        {"|0| D1-> D2<- I1-> |0|", ListState{5,5,0,0,0,5}, ListState{ 5,4,1,0,1,5}},
        {"|0| D1-> D2<- I1<- |0|", ListState{5,5,0,0,0,5}, ListState{ 5,4,0,1,0,4}},
        {"|0| D1-> D2-> I1-> |0|", ListState{5,5,0,0,0,5}, ListState{ 5,4,0,1,0,4}},
        {"|0| D1-> D2-> I1<- |0|", ListState{5,5,0,0,0,5}, ListState{10,4,2,4,2,6}}
    },
    {
        {"|0|           I1-> |1|", ListState{6,5,0,1,0,5}, ListState{ 6,6,0,0,0,6}},
        {"|0|           I1<- |1|", ListState{6,5,0,1,0,5}, ListState{12,6,2,4,2,8}},
        {"|0| D1->      I1-> |1|", ListState{6,5,0,1,0,5}, ListState{ 6,5,0,1,0,5}},
        {"|0| D1->      I1<- |1|", ListState{6,5,0,1,0,5}, ListState{12,5,3,4,3,8}},
        {"|0| D1<-      I1-> |1|", ListState{6,5,0,1,0,5}, ListState{ 6,5,1,0,1,6}},
        {"|0| D1<-      I1<- |1|", ListState{6,5,0,1,0,5}, ListState{ 6,5,0,1,0,5}},
        {"|0| D1<- D2<- I1-> |1|", ListState{6,5,0,1,0,5}, ListState{ 6,4,2,0,2,6}},
        {"|0| D1<- D2<- I1<- |1|", ListState{6,5,0,1,0,5}, ListState{ 6,4,1,1,1,5}},
        {"|0| D1<- D2-> I1-> |1|", ListState{6,5,0,1,0,5}, ListState{ 6,4,1,1,1,5}},
        {"|0| D1<- D2-> I1<- |1|", ListState{6,5,0,1,0,5}, ListState{ 6,4,0,2,0,4}},
        {"|0| D1-> D2<- I1-> |1|", ListState{6,5,0,1,0,5}, ListState{ 6,4,1,1,1,5}},
        {"|0| D1-> D2<- I1<- |1|", ListState{6,5,0,1,0,5}, ListState{ 6,4,0,2,0,4}},
        {"|0| D1-> D2-> I1-> |1|", ListState{6,5,0,1,0,5}, ListState{ 6,4,0,2,0,4}},
        {"|0| D1-> D2-> I1<- |1|", ListState{6,5,0,1,0,5}, ListState{12,4,3,5,3,7}}
    },
    {
        {"|1|           I1-> |0|", ListState{6,5,1,0,1,6}, ListState{12,6,3,3,3,9}},
        {"|1|           I1<- |0|", ListState{6,5,1,0,1,6}, ListState{ 6,6,0,0,0,6}},
        {"|1| D1->      I1-> |0|", ListState{6,5,1,0,1,6}, ListState{ 6,5,1,0,1,6}},
        {"|1| D1->      I1<- |0|", ListState{6,5,1,0,1,6}, ListState{ 6,5,0,1,0,5}},
        {"|1| D1<-      I1-> |0|", ListState{6,5,1,0,1,6}, ListState{12,5,4,3,4,9}},
        {"|1| D1<-      I1<- |0|", ListState{6,5,1,0,1,6}, ListState{ 6,5,1,0,1,6}},
        {"|1| D1<- D2<- I1-> |0|", ListState{6,5,1,0,1,6}, ListState{12,4,4,4,4,8}},
        {"|1| D1<- D2<- I1<- |0|", ListState{6,5,1,0,1,6}, ListState{ 6,4,2,0,4,6}},
        {"|1| D1<- D2-> I1-> |0|", ListState{6,5,1,0,1,6}, ListState{ 6,4,2,0,2,6}},
        {"|1| D1<- D2-> I1<- |0|", ListState{6,5,1,0,1,6}, ListState{ 6,4,1,1,1,5}},
        {"|1| D1-> D2<- I1-> |0|", ListState{6,5,1,0,1,6}, ListState{ 6,4,2,0,2,6}},
        {"|1| D1-> D2<- I1<- |0|", ListState{6,5,1,0,1,6}, ListState{ 6,4,1,1,1,5}},
        {"|1| D1-> D2-> I1-> |0|", ListState{6,5,1,0,1,6}, ListState{ 6,4,1,1,1,5}},
        {"|1| D1-> D2-> I1<- |0|", ListState{6,5,1,0,1,6}, ListState{ 6,4,0,2,0,4}}
    },
    {
        {"|1|           I1-> |1|", ListState{7,5,1,1,1,6}, ListState{ 7,6,1,0,1,7}},
        {"|1|           I1<- |1|", ListState{7,5,1,1,1,6}, ListState{ 7,6,0,1,0,6}},
        {"|1| D1->      I1-> |1|", ListState{7,5,1,1,1,6}, ListState{ 7,5,1,1,1,6}},
        {"|1| D1->      I1<- |1|", ListState{7,5,1,0,1,6}, ListState{ 7,5,0,2,0,5}},
        {"|1| D1<-      I1-> |1|", ListState{7,5,1,1,1,6}, ListState{ 7,5,2,0,2,7}},
        {"|1| D1<-      I1<- |1|", ListState{7,5,1,1,1,6}, ListState{ 7,5,1,1,1,6}},
        {"|1| D1<- D2<- I1-> |1|", ListState{7,5,1,1,1,6}, ListState{ 7,4,3,0,3,7}},
        {"|1| D1<- D2<- I1<- |1|", ListState{7,5,1,1,1,6}, ListState{ 7,4,2,1,2,6}},
        {"|1| D1<- D2-> I1-> |1|", ListState{7,5,1,1,1,6}, ListState{ 7,4,2,1,2,6}},
        {"|1| D1<- D2-> I1<- |1|", ListState{7,5,1,1,1,6}, ListState{ 7,4,1,2,1,5}},
        {"|1| D1-> D2<- I1-> |1|", ListState{7,5,1,1,1,6}, ListState{ 7,4,2,1,2,6}},
        {"|1| D1-> D2<- I1<- |1|", ListState{7,5,1,1,1,6}, ListState{ 7,4,1,2,1,5}},
        {"|1| D1-> D2-> I1-> |1|", ListState{7,5,1,1,1,6}, ListState{ 7,4,1,2,1,5}},
        {"|1| D1-> D2-> I1<- |1|", ListState{7,5,1,1,1,6}, ListState{ 7,4,0,3,0,4}}
    }
};


/*_________________________________________________________________________________________________
 *      ќпредел€ет тип проделанной операции с S-List.                                              |
 *________________________________________________________________________________________________*/
Ins_complex::S_ActionType Ins_complex::SListActionDetermination( Side ins, Side del1, Side del2 )
{
    if (del1 == Side_NONE && del2 == Side_NONE && ins == Side_RIGHT)
        return Ins_complex::IR;
    else if (del1 == Side_NONE && del2 == Side_NONE && ins == Side_LEFT)
        return Ins_complex::IL;
    else if (del1 == Side_RIGHT && del2 == Side_NONE && ins == Side_RIGHT)
        return Ins_complex::DR_IR;
    else if (del1 == Side_RIGHT && del2 == Side_NONE && ins == Side_LEFT)
        return Ins_complex::DR_IL;
    else if (del1 == Side_LEFT && del2 == Side_NONE && ins == Side_RIGHT)
        return Ins_complex::DL_IR;
    else if (del1 == Side_LEFT && del2 == Side_NONE && ins == Side_LEFT)
        return Ins_complex::DL_IL;
    else if (del1 == Side_LEFT && del2 == Side_LEFT && ins == Side_RIGHT)
        return Ins_complex::DL_DL_IR;
    else if (del1 == Side_LEFT && del2 == Side_LEFT && ins == Side_LEFT)
        return Ins_complex::DL_DL_IL;
    else if (del1 == Side_LEFT && del2 == Side_RIGHT && ins == Side_RIGHT)
        return Ins_complex::DL_DR_IR;
    else if (del1 == Side_LEFT && del2 == Side_RIGHT && ins == Side_LEFT)
        return Ins_complex::DL_DR_IL;
    else if (del1 == Side_RIGHT && del2 == Side_LEFT && ins == Side_RIGHT)
        return Ins_complex::DR_DL_IR;
    else if (del1 == Side_RIGHT && del2 == Side_LEFT && ins == Side_LEFT)
        return Ins_complex::DR_DL_IL;
    else if (del1 == Side_RIGHT && del2 == Side_RIGHT && ins == Side_RIGHT)
        return Ins_complex::DR_DR_IR;
    else if (del1 == Side_RIGHT && del2 == Side_RIGHT && ins == Side_LEFT)
        return Ins_complex::DR_DR_IL;
    else 
        return Ins_complex::S_ActionTypes_Count;
}