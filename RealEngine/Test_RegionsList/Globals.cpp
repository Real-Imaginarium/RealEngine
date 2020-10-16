#include "Globals.h"

typedef Globals::InsertionsComplex InsertionsComplex;
typedef Globals::ReorganizeComplex ReorganizeComplex;

/* ћеста вставки */
const InsertionsComplex::InsertionPlace InsertionsComplex::InsertionPlaces[Regions_Count] = { Start_P1, P1_P2, P2_P3, P3_P4, P4_P5, P5_Finish };

/* —осто€ни€ полей (одинаковые дл€ P-List и S-List. Ўирина пол€ - 1 RegionP или RegionS) */
const InsertionsComplex::FieldState InsertionsComplex::FieldStates[FieldStates_Count] = { L0_R0, L0_R1, L1_R0, L1_R1 };

/* “ипы вставок в P-List */
const InsertionsComplex::P_InsertionType InsertionsComplex::P_InsertionTypes[P_InsertionTypes_Count] =
{
    Start_P1_Adj_None, Start_P1_Adj_R, P1_P2_Adj_None, P1_P2_Adj_L, P1_P2_Adj_R, P1_P2_Adj_RL, P2_P3_Adj_None, P2_P3_Adj_L, P2_P3_Adj_R, P2_P3_Adj_RL,
    P3_P4_Adj_None, P3_P4_Adj_L, P3_P4_Adj_R, P3_P4_Adj_RL, P4_P5_Adj_None, P4_P5_Adj_L, P4_P5_Adj_R, P4_P5_Adj_RL, P5_Finish_Adj_None, P5_Finish_Adj_L
};


/* “ип операций с S-List (зависит от типа вставки в P-List) */
const InsertionsComplex::S_ActionType InsertionsComplex::S_ActionTypes[S_ActionTypes_Count] =
{
    IR, IL, DR_IR, DR_IL, DL_IR, DL_IL, DL_DL_IR, DL_DL_IL, DL_DR_IR, DL_DR_IL, DR_DL_IR, DR_DL_IL, DR_DR_IR, DR_DR_IL,
};


CELL InsertionsComplex::mem[(size_t)Bounds::MEM_SIZE];


/* ”казатели на начала 5 равноудалЄнных регионов + указатель на конец пам€ти */
CELL* const InsertionsComplex::regPtrs[Regions_Count] = {
    mem + (size_t)Bounds::REG_1_START,
    mem + (size_t)Bounds::REG_2_START,
    mem + (size_t)Bounds::REG_3_START,
    mem + (size_t)Bounds::REG_4_START,
    mem + (size_t)Bounds::REG_5_START,
    mem + (size_t)Bounds::MEM_SIZE
};


// Ќабор исходных и результирующих состо€ний P- и S-List и мнемоника тесткейса
const std::tuple<std::string, ListState, ListState> InsertionsComplex::P_States[FieldStates_Count][P_InsertionTypes_Count] =
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


const std::tuple<std::string, ListState, ListState> InsertionsComplex::S_States[FieldStates_Count][S_ActionTypes_Count] =
{
    {
        {"|0|           I1-> |0|", ListState{5,5,0,0,0,5}, ListState{10,6,2,2,2,8}},
        {"|0|           I1<- |0|", ListState{5,5,0,0,0,5}, ListState{10,6,1,3,1,7}},
        {"|0| D1->      I1-> |0|", ListState{5,5,0,0,0,5}, ListState{ 5,5,0,0,0,5}},
        {"|0| D1->      I1<- |0|", ListState{5,5,0,0,0,5}, ListState{10,5,2,3,2,7}},
        {"|0| D1<-      I1-> |0|", ListState{5,5,0,0,0,5}, ListState{10,5,3,2,3,8}},
        {"|0| D1<-      I1<- |0|", ListState{5,5,0,0,0,5}, ListState{ 5,5,0,0,0,5}},
        {"|0| D1<- D2<- I1-> |0|", ListState{5,5,0,0,0,5}, ListState{ 5,4,1,0,1,5}},
        {"|0| D1<- D2<- I1<- |0|", ListState{5,5,0,0,0,5}, ListState{ 5,4,1,0,1,5}},
        {"|0| D1<- D2-> I1-> |0|", ListState{5,5,0,0,0,5}, ListState{ 5,4,1,0,1,5}},
        {"|0| D1<- D2-> I1<- |0|", ListState{5,5,0,0,0,5}, ListState{ 5,4,0,1,0,4}},
        {"|0| D1-> D2<- I1-> |0|", ListState{5,5,0,0,0,5}, ListState{ 5,4,1,0,1,5}},
        {"|0| D1-> D2<- I1<- |0|", ListState{5,5,0,0,0,5}, ListState{ 5,4,0,1,0,4}},
        {"|0| D1-> D2-> I1-> |0|", ListState{5,5,0,0,0,5}, ListState{ 5,4,0,1,0,4}},
        {"|0| D1-> D2-> I1<- |0|", ListState{5,5,0,0,0,5}, ListState{ 5,4,0,1,0,4}}
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
        {"|0| D1-> D2-> I1<- |1|", ListState{6,5,0,1,0,5}, ListState{ 6,4,0,2,0,4}}
    },
    {
        {"|1|           I1-> |0|", ListState{6,5,1,0,1,6}, ListState{12,6,3,3,3,9}},
        {"|1|           I1<- |0|", ListState{6,5,1,0,1,6}, ListState{ 6,6,0,0,0,6}},
        {"|1| D1->      I1-> |0|", ListState{6,5,1,0,1,6}, ListState{ 6,5,1,0,1,6}},
        {"|1| D1->      I1<- |0|", ListState{6,5,1,0,1,6}, ListState{ 6,5,0,1,0,5}},
        {"|1| D1<-      I1-> |0|", ListState{6,5,1,0,1,6}, ListState{12,5,4,3,4,9}},
        {"|1| D1<-      I1<- |0|", ListState{6,5,1,0,1,6}, ListState{ 6,5,1,0,1,6}},
        {"|1| D1<- D2<- I1-> |0|", ListState{6,5,1,0,1,6}, ListState{ 6,4,2,0,2,6}},
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


// ќпредел€ет тип проделанной операции с S-List.
InsertionsComplex::S_ActionType InsertionsComplex::SListActionDetermination( Side ins, Side del1, Side del2 )
{
    if (del1 == Side_NONE && del2 == Side_NONE && ins == Side_RIGHT)
        return InsertionsComplex::IR;
    else if (del1 == Side_NONE && del2 == Side_NONE && ins == Side_LEFT)
        return InsertionsComplex::IL;
    else if (del1 == Side_RIGHT && del2 == Side_NONE && ins == Side_RIGHT)
        return InsertionsComplex::DR_IR;
    else if (del1 == Side_RIGHT && del2 == Side_NONE && ins == Side_LEFT)
        return InsertionsComplex::DR_IL;
    else if (del1 == Side_LEFT && del2 == Side_NONE && ins == Side_RIGHT)
        return InsertionsComplex::DL_IR;
    else if (del1 == Side_LEFT && del2 == Side_NONE && ins == Side_LEFT)
        return InsertionsComplex::DL_IL;
    else if (del1 == Side_LEFT && del2 == Side_LEFT && ins == Side_RIGHT)
        return InsertionsComplex::DL_DL_IR;
    else if (del1 == Side_LEFT && del2 == Side_LEFT && ins == Side_LEFT)
        return InsertionsComplex::DL_DL_IL;
    else if (del1 == Side_LEFT && del2 == Side_RIGHT && ins == Side_RIGHT)
        return InsertionsComplex::DL_DR_IR;
    else if (del1 == Side_LEFT && del2 == Side_RIGHT && ins == Side_LEFT)
        return InsertionsComplex::DL_DR_IL;
    else if (del1 == Side_RIGHT && del2 == Side_LEFT && ins == Side_RIGHT)
        return InsertionsComplex::DR_DL_IR;
    else if (del1 == Side_RIGHT && del2 == Side_LEFT && ins == Side_LEFT)
        return InsertionsComplex::DR_DL_IL;
    else if (del1 == Side_RIGHT && del2 == Side_RIGHT && ins == Side_RIGHT)
        return InsertionsComplex::DR_DR_IR;
    else if (del1 == Side_RIGHT && del2 == Side_RIGHT && ins == Side_LEFT)
        return InsertionsComplex::DR_DR_IL;
    else 
        return InsertionsComplex::S_ActionTypes_Count;
}


//  усок пам€ти и регионы в нЄм, дл€ вставки в RegionsList с целью его реорганизации (сдвигов контента или расширени€)
CELL ReorganizeComplex::m[mem_size];

RegionP<CELL> ReorganizeComplex::r[regs_count] =
{
{m+0,    1}, {m+2,    2}, {m+5,    3}, {m+9,    4}, {m+14,   5}, {m+20,   6}, {m+27,   7}, {m+35,   8}, {m+44,   9}, {m+54, 10},
{m+65,  11}, {m+77,  12}, {m+90,  13}, {m+104, 14}, {m+119, 15}, {m+135, 16}, {m+152, 17}, {m+170, 18}, {m+189, 19}, {m+209,20},
{m+230, 21}, {m+252, 22}, {m+275, 23}, {m+299, 24}, {m+324, 25}, {m+350, 26}, {m+377, 27}, {m+405, 28}, {m+434, 29}, {m+464,30},
{m+495, 31}, {m+527, 32}, {m+560, 33}, {m+594, 34}, {m+629, 35}, {m+665, 36}, {m+702, 37}, {m+740, 38}, {m+779, 39}, {m+819,40}
};


// “есткейсы, каждый содержит: исходное и результирующее состо€ние (одинаковое дл€ P-и-S-List), исходный и результирующий
// контент P-List (S-List восстанавливаетс€ из него в тесте), вставка. “есткейсы независимы.
const std::tuple<ListState, ListState, std::vector<RegionP<CELL>>, std::vector<RegionP<CELL>>, RegionP<CELL>> ReorganizeComplex::test_cases[66] =
{
    // “ест вставки справа, когда R пусто, а L - разной ширины (ExpandList или ShiftContentLeft)
    {{3,1,2,0,2,3}, {3, 2,1,0,1,3 }, std::vector<RegionP<CELL>>( r+20, r+21 ), std::vector<RegionP<CELL>>( r+20, r+22 ), r[21]}, // [ ][ ][#]                => [ ][#][X]
    {{3,2,1,0,1,3}, {6, 3,2,1,2,5 }, std::vector<RegionP<CELL>>( r+20, r+22 ), std::vector<RegionP<CELL>>( r+20, r+23 ), r[22]}, // [ ][#][#]                => [ ][ ][#][#][X][ ]
    {{3,3,0,0,0,3}, {6, 4,1,1,1,5 }, std::vector<RegionP<CELL>>( r+20, r+23 ), std::vector<RegionP<CELL>>( r+20, r+24 ), r[23]}, // [#][#][#]                => [ ][#][#][#][X][ ]
    {{4,1,3,0,3,4}, {4, 2,2,0,2,4 }, std::vector<RegionP<CELL>>( r+20, r+21 ), std::vector<RegionP<CELL>>( r+20, r+22 ), r[21]}, // [ ][ ][ ][#]             => [ ][ ][#][X]
    {{4,2,2,0,2,4}, {4, 3,1,0,1,4 }, std::vector<RegionP<CELL>>( r+20, r+22 ), std::vector<RegionP<CELL>>( r+20, r+23 ), r[22]}, // [ ][ ][#][#]             => [ ][#][#][X]
    {{4,3,1,0,1,4}, {8, 4,2,2,2,6 }, std::vector<RegionP<CELL>>( r+20, r+23 ), std::vector<RegionP<CELL>>( r+20, r+24 ), r[23]}, // [ ][#][#][#]             => [ ][ ][#][#][#][X][ ][ ]
    {{4,4,0,0,0,4}, {8, 5,2,1,2,7 }, std::vector<RegionP<CELL>>( r+20, r+24 ), std::vector<RegionP<CELL>>( r+20, r+25 ), r[24]}, // [#][#][#][#]             => [ ][ ][#][#][#][#][X][ ]
    {{5,1,4,0,4,5}, {5, 2,2,1,2,4 }, std::vector<RegionP<CELL>>( r+20, r+21 ), std::vector<RegionP<CELL>>( r+20, r+22 ), r[21]}, // [ ][ ][ ][ ][#]          => [ ][ ][#][X][ ]
    {{5,2,3,0,3,5}, {5, 3,2,0,2,5 }, std::vector<RegionP<CELL>>( r+20, r+22 ), std::vector<RegionP<CELL>>( r+20, r+23 ), r[22]}, // [ ][ ][ ][#][#]          => [ ][ ][#][#][X]
    {{5,3,2,0,2,5}, {5, 4,1,0,1,5 }, std::vector<RegionP<CELL>>( r+20, r+23 ), std::vector<RegionP<CELL>>( r+20, r+24 ), r[23]}, // [ ][ ][#][#][#]          => [ ][#][#][#][X]
    {{5,4,1,0,1,5}, {10,5,3,2,3,8 }, std::vector<RegionP<CELL>>( r+20, r+24 ), std::vector<RegionP<CELL>>( r+20, r+25 ), r[24]}, // [ ][#][#][#][#]          => [ ][ ][ ][#][#][#][#][X][ ][ ]
    {{5,5,0,0,0,5}, {10,6,2,2,2,8 }, std::vector<RegionP<CELL>>( r+20, r+25 ), std::vector<RegionP<CELL>>( r+20, r+26 ), r[25]}, // [#][#][#][#][#]          => [ ][ ][#][#][#][#][#][X][ ][ ]
    {{6,1,5,0,5,6}, {6, 2,3,1,3,5 }, std::vector<RegionP<CELL>>( r+20, r+21 ), std::vector<RegionP<CELL>>( r+20, r+22 ), r[21]}, // [ ][ ][ ][ ][ ][#]       => [ ][ ][ ][#][X][ ]
    {{6,2,4,0,4,6}, {6, 3,2,1,2,5 }, std::vector<RegionP<CELL>>( r+20, r+22 ), std::vector<RegionP<CELL>>( r+20, r+23 ), r[22]}, // [ ][ ][ ][ ][#][#]       => [ ][ ][#][#][X][ ]
    {{6,3,3,0,3,6}, {6, 4,2,0,2,6 }, std::vector<RegionP<CELL>>( r+20, r+23 ), std::vector<RegionP<CELL>>( r+20, r+24 ), r[23]}, // [ ][ ][ ][#][#][#]       => [ ][ ][#][#][#][X]
    {{6,4,2,0,2,6}, {12,5,4,3,4,9 }, std::vector<RegionP<CELL>>( r+20, r+24 ), std::vector<RegionP<CELL>>( r+20, r+25 ), r[24]}, // [ ][ ][#][#][#][#]       => [ ][ ][ ][ ][#][#][#][#][X][ ][ ][ ]
    {{6,5,1,0,1,6}, {12,6,3,3,3,9 }, std::vector<RegionP<CELL>>( r+20, r+25 ), std::vector<RegionP<CELL>>( r+20, r+26 ), r[25]}, // [ ][#][#][#][#][#]       => [ ][ ][ ][#][#][#][#][#][X][ ][ ][ ]
    {{6,6,0,0,0,6}, {12,7,3,2,3,10}, std::vector<RegionP<CELL>>( r+20, r+26 ), std::vector<RegionP<CELL>>( r+20, r+27 ), r[26]}, // [#][#][#][#][#][#]       => [ ][ ][ ][#][#][#][#][#][#][X][ ][ ]
    {{7,1,6,0,6,7}, {7, 2,3,2,3,5 }, std::vector<RegionP<CELL>>( r+20, r+21 ), std::vector<RegionP<CELL>>( r+20, r+22 ), r[21]}, // [ ][ ][ ][ ][ ][ ][#]    => [ ][ ][ ][#][X][ ][ ]
    {{7,2,5,0,5,7}, {7, 3,3,1,3,6 }, std::vector<RegionP<CELL>>( r+20, r+22 ), std::vector<RegionP<CELL>>( r+20, r+23 ), r[22]}, // [ ][ ][ ][ ][ ][#][#]    => [ ][ ][ ][#][#][X][ ]
    {{7,3,4,0,4,7}, {7, 4,2,1,2,6 }, std::vector<RegionP<CELL>>( r+20, r+23 ), std::vector<RegionP<CELL>>( r+20, r+24 ), r[23]}, // [ ][ ][ ][ ][#][#][#]    => [ ][ ][#][#][#][X][ ]
    {{7,4,3,0,3,7}, {7, 5,2,0,2,7 }, std::vector<RegionP<CELL>>( r+20, r+24 ), std::vector<RegionP<CELL>>( r+20, r+25 ), r[24]}, // [ ][ ][ ][#][#][#][#]    => [ ][ ][#][#][#][#][X]
    {{7,5,2,0,2,7}, {14,6,4,4,4,10}, std::vector<RegionP<CELL>>( r+20, r+25 ), std::vector<RegionP<CELL>>( r+20, r+26 ), r[25]}, // [ ][ ][#][#][#][#][#]    => [ ][ ][ ][ ][#][#][#][#][#][X][ ][ ][ ][ ]
    {{7,6,1,0,1,7}, {14,7,4,3,4,11}, std::vector<RegionP<CELL>>( r+20, r+26 ), std::vector<RegionP<CELL>>( r+20, r+27 ), r[26]}, // [ ][#][#][#][#][#][#]    => [ ][ ][ ][ ][#][#][#][#][#][#][X][ ][ ][ ]
    {{7,7,0,0,0,7}, {14,8,3,3,3,11}, std::vector<RegionP<CELL>>( r+20, r+27 ), std::vector<RegionP<CELL>>( r+20, r+28 ), r[27]}, // [#][#][#][#][#][#][#]    => [ ][ ][ ][#][#][#][#][#][#][#][X][ ][ ][ ]
    {{8,1,7,0,7,8}, {8, 2,4,2,4,6 }, std::vector<RegionP<CELL>>( r+20, r+21 ), std::vector<RegionP<CELL>>( r+20, r+22 ), r[21]}, // [ ][ ][ ][ ][ ][ ][ ][#] => [ ][ ][ ][ ][#][X][ ][ ]
    {{8,2,6,0,6,8}, {8, 3,3,2,3,6 }, std::vector<RegionP<CELL>>( r+20, r+22 ), std::vector<RegionP<CELL>>( r+20, r+23 ), r[22]}, // [ ][ ][ ][ ][ ][ ][#][#] => [ ][ ][ ][#][#][X][ ][ ]
    {{8,3,5,0,5,8}, {8, 4,3,1,3,7 }, std::vector<RegionP<CELL>>( r+20, r+23 ), std::vector<RegionP<CELL>>( r+20, r+24 ), r[23]}, // [ ][ ][ ][ ][ ][#][#][#] => [ ][ ][ ][#][#][#][X][ ]
    {{8,4,4,0,4,8}, {8, 5,2,1,2,7 }, std::vector<RegionP<CELL>>( r+20, r+24 ), std::vector<RegionP<CELL>>( r+20, r+25 ), r[24]}, // [ ][ ][ ][ ][#][#][#][#] => [ ][ ][#][#][#][#][X][ ]
    {{8,5,3,0,3,8}, {8, 6,2,0,2,8 }, std::vector<RegionP<CELL>>( r+20, r+25 ), std::vector<RegionP<CELL>>( r+20, r+26 ), r[25]}, // [ ][ ][ ][#][#][#][#][#] => [ ][ ][#][#][#][#][#][X]
    {{8,6,2,0,2,8}, {16,7,5,4,5,12}, std::vector<RegionP<CELL>>( r+20, r+26 ), std::vector<RegionP<CELL>>( r+20, r+27 ), r[26]}, // [ ][ ][#][#][#][#][#][#] => [ ][ ][ ][ ][ ][#][#][#][#][#][#][X][ ][ ][ ][ ]
    {{8,7,1,0,1,8}, {16,8,4,4,4,12}, std::vector<RegionP<CELL>>( r+20, r+27 ), std::vector<RegionP<CELL>>( r+20, r+28 ), r[27]}, // [ ][#][#][#][#][#][#][#] => [ ][ ][ ][ ][#][#][#][#][#][#][#][X][ ][ ][ ][ ]
    {{8,8,0,0,0,8}, {16,9,4,3,4,13}, std::vector<RegionP<CELL>>( r+20, r+28 ), std::vector<RegionP<CELL>>( r+20, r+29 ), r[28]}, // [#][#][#][#][#][#][#][#] => [ ][ ][ ][ ][#][#][#][#][#][#][#][#][X][ ][ ][ ]

    // “ест вставки слева, когда L пусто, а R - разной ширины (ExpandList или ShiftContentRight)
    {{3,1,0,2,0,1}, {3, 2,0,1,0,2 }, std::vector<RegionP<CELL>>( r+21, r+22 ), std::vector<RegionP<CELL>>( r+20, r+22 ), r[20]}, // [#][ ][ ]                => [X][#][ ]
    {{3,2,0,1,0,2}, {6, 3,1,2,1,4 }, std::vector<RegionP<CELL>>( r+21, r+23 ), std::vector<RegionP<CELL>>( r+20, r+23 ), r[20]}, // [#][#][ ]                => [ ][X][#][#][ ][ ]
    {{3,3,0,0,0,3}, {6, 4,0,2,0,4 }, std::vector<RegionP<CELL>>( r+21, r+24 ), std::vector<RegionP<CELL>>( r+20, r+24 ), r[20]}, // [#][#][#]                => [X][#][#][#][ ][ ]
    {{4,1,0,3,0,1}, {4, 2,0,2,0,2 }, std::vector<RegionP<CELL>>( r+21, r+22 ), std::vector<RegionP<CELL>>( r+20, r+22 ), r[20]}, // [#][ ][ ][ ]             => [X][#][ ][ ]
    {{4,2,0,2,0,2}, {4, 3,0,1,0,3 }, std::vector<RegionP<CELL>>( r+21, r+23 ), std::vector<RegionP<CELL>>( r+20, r+23 ), r[20]}, // [#][#][ ][ ]             => [X][#][#][ ]
    {{4,3,0,1,0,3}, {8, 4,1,3,1,5 }, std::vector<RegionP<CELL>>( r+21, r+24 ), std::vector<RegionP<CELL>>( r+20, r+24 ), r[20]}, // [#][#][#][ ]             => [ ][X][#][#][#][ ][ ][ ]
    {{4,4,0,0,0,4}, {8, 5,1,2,1,6 }, std::vector<RegionP<CELL>>( r+21, r+25 ), std::vector<RegionP<CELL>>( r+20, r+25 ), r[20]}, // [#][#][#][#]             => [ ][X][#][#][#][#][ ][ ]
    {{5,1,0,4,0,1}, {5, 2,1,2,1,3 }, std::vector<RegionP<CELL>>( r+21, r+22 ), std::vector<RegionP<CELL>>( r+20, r+22 ), r[20]}, // [#][ ][ ][ ][ ]          => [ ][X][#][ ][ ]
    {{5,2,0,3,0,2}, {5, 3,0,2,0,3 }, std::vector<RegionP<CELL>>( r+21, r+23 ), std::vector<RegionP<CELL>>( r+20, r+23 ), r[20]}, // [#][#][ ][ ][ ]          => [X][#][#][ ][ ]
    {{5,3,0,2,0,3}, {5, 4,0,1,0,4 }, std::vector<RegionP<CELL>>( r+21, r+24 ), std::vector<RegionP<CELL>>( r+20, r+24 ), r[20]}, // [#][#][#][ ][ ]          => [X][#][#][#][ ]
    {{5,4,0,1,0,4}, {10,5,2,3,2,7 }, std::vector<RegionP<CELL>>( r+21, r+25 ), std::vector<RegionP<CELL>>( r+20, r+25 ), r[20]}, // [#][#][#][#][ ]          => [ ][ ][X][#][#][#][#][ ][ ][ ]
    {{5,5,0,0,0,5}, {10,6,1,3,1,7 }, std::vector<RegionP<CELL>>( r+21, r+26 ), std::vector<RegionP<CELL>>( r+20, r+26 ), r[20]}, // [#][#][#][#][#]          => [ ][X][#][#][#][#][#][ ][ ][ ]
    {{6,1,0,5,0,1}, {6, 2,1,3,1,3 }, std::vector<RegionP<CELL>>( r+21, r+22 ), std::vector<RegionP<CELL>>( r+20, r+22 ), r[20]}, // [#][ ][ ][ ][ ][ ]       => [ ][X][#][ ][ ][ ]
    {{6,2,0,4,0,2}, {6, 3,1,2,1,4 }, std::vector<RegionP<CELL>>( r+21, r+23 ), std::vector<RegionP<CELL>>( r+20, r+23 ), r[20]}, // [#][#][ ][ ][ ][ ]       => [ ][X][#][#][ ][ ]
    {{6,3,0,3,0,3}, {6, 4,0,2,0,4 }, std::vector<RegionP<CELL>>( r+21, r+24 ), std::vector<RegionP<CELL>>( r+20, r+24 ), r[20]}, // [#][#][#][ ][ ][ ]       => [X][#][#][#][ ][ ]
    {{6,4,0,2,0,4}, {12,5,3,4,3,8 }, std::vector<RegionP<CELL>>( r+21, r+25 ), std::vector<RegionP<CELL>>( r+20, r+25 ), r[20]}, // [#][#][#][#][ ][ ]       => [ ][ ][ ][X][#][#][#][#][ ][ ][ ][ ]
    {{6,5,0,1,0,5}, {12,6,2,4,2,8 }, std::vector<RegionP<CELL>>( r+21, r+26 ), std::vector<RegionP<CELL>>( r+20, r+26 ), r[20]}, // [#][#][#][#][#][ ]       => [ ][ ][X][#][#][#][#][#][ ][ ][ ][ ]
    {{6,6,0,0,0,6}, {12,7,2,3,2,9 }, std::vector<RegionP<CELL>>( r+21, r+27 ), std::vector<RegionP<CELL>>( r+20, r+27 ), r[20]}, // [#][#][#][#][#][#]       => [ ][ ][X][#][#][#][#][#][#][ ][ ][ ]
    {{7,1,0,6,0,1}, {7, 2,2,3,2,4 }, std::vector<RegionP<CELL>>( r+21, r+22 ), std::vector<RegionP<CELL>>( r+20, r+22 ), r[20]}, // [#][ ][ ][ ][ ][ ][ ]    => [ ][X][#][#][ ][ ][ ]
    {{7,2,0,5,0,2}, {7, 3,1,3,1,4 }, std::vector<RegionP<CELL>>( r+21, r+23 ), std::vector<RegionP<CELL>>( r+20, r+23 ), r[20]}, // [#][#][ ][ ][ ][ ][ ]    => [ ][X][#][#][ ][ ][ ]
    {{7,3,0,4,0,3}, {7, 4,1,2,1,5 }, std::vector<RegionP<CELL>>( r+21, r+24 ), std::vector<RegionP<CELL>>( r+20, r+24 ), r[20]}, // [#][#][#][ ][ ][ ][ ]    => [X][#][#][#][#][ ][ ]
    {{7,4,0,3,0,4}, {7, 5,0,2,0,5 }, std::vector<RegionP<CELL>>( r+21, r+25 ), std::vector<RegionP<CELL>>( r+20, r+25 ), r[20]}, // [#][#][#][#][ ][ ][ ]    => [ ][ ][X][#][#][ ][ ]
    {{7,5,0,2,0,5}, {14,6,3,5,3,9 }, std::vector<RegionP<CELL>>( r+21, r+26 ), std::vector<RegionP<CELL>>( r+20, r+26 ), r[20]}, // [#][#][#][#][#][ ][ ]    => [ ][ ][ ][X][#][#][#][#][#][ ][ ][ ][ ][ ]
    {{7,6,0,1,0,6}, {14,7,3,4,3,10}, std::vector<RegionP<CELL>>( r+21, r+27 ), std::vector<RegionP<CELL>>( r+20, r+27 ), r[20]}, // [#][#][#][#][#][#][ ]    => [ ][ ][ ][X][#][#][#][#][#][#][ ][ ][ ][ ]
    {{7,7,0,0,0,7}, {14,8,2,4,2,10}, std::vector<RegionP<CELL>>( r+21, r+28 ), std::vector<RegionP<CELL>>( r+20, r+28 ), r[20]}, // [#][#][#][#][#][#][#]    => [ ][ ][X][#][#][#][#][#][#][#][ ][ ][ ][ ]
    {{8,1,0,7,0,1}, {8, 2,2,4,2,4 }, std::vector<RegionP<CELL>>( r+21, r+22 ), std::vector<RegionP<CELL>>( r+20, r+22 ), r[20]}, // [#][ ][ ][ ][ ][ ][ ][ ] => [ ][ ][X][#][ ][ ][ ][ ]
    {{8,2,0,6,0,2}, {8, 3,2,3,2,5 }, std::vector<RegionP<CELL>>( r+21, r+23 ), std::vector<RegionP<CELL>>( r+20, r+23 ), r[20]}, // [#][#][ ][ ][ ][ ][ ][ ] => [ ][ ][X][#][#][ ][ ][ ]
    {{8,3,0,5,0,3}, {8, 4,1,3,1,5 }, std::vector<RegionP<CELL>>( r+21, r+24 ), std::vector<RegionP<CELL>>( r+20, r+24 ), r[20]}, // [#][#][#][ ][ ][ ][ ][ ] => [ ][X][#][#][#][ ][ ][ ]
    {{8,4,0,4,0,4}, {8, 5,1,2,1,6 }, std::vector<RegionP<CELL>>( r+21, r+25 ), std::vector<RegionP<CELL>>( r+20, r+25 ), r[20]}, // [#][#][#][#][ ][ ][ ][ ] => [ ][X][#][#][#][#][ ][ ]
    {{8,5,0,3,0,5}, {8, 6,0,2,0,6 }, std::vector<RegionP<CELL>>( r+21, r+26 ), std::vector<RegionP<CELL>>( r+20, r+26 ), r[20]}, // [#][#][#][#][#][ ][ ][ ] => [X][#][#][#][#][#][ ][ ]
    {{8,6,0,2,0,6}, {16,7,4,5,4,11}, std::vector<RegionP<CELL>>( r+21, r+27 ), std::vector<RegionP<CELL>>( r+20, r+27 ), r[20]}, // [#][#][#][#][#][#][ ][ ] => [ ][ ][ ][ ][X][#][#][#][#][#][#][ ][ ][ ][ ][ ]
    {{8,7,0,1,0,7}, {16,8,3,5,3,11}, std::vector<RegionP<CELL>>( r+21, r+28 ), std::vector<RegionP<CELL>>( r+20, r+28 ), r[20]}, // [#][#][#][#][#][#][#][ ] => [ ][ ][ ][X][#][#][#][#][#][#][#][ ][ ][ ][ ][ ]
    {{8,8,0,0,0,8}, {16,9,3,4,3,12}, std::vector<RegionP<CELL>>( r+21, r+29 ), std::vector<RegionP<CELL>>( r+20, r+29 ), r[20]}, // [#][#][#][#][#][#][#][#] => [ ][ ][ ][X][#][#][#][#][#][#][#][#][ ][ ][ ][ ]
};


// ѕоследовательность тесткейсов дл€ многократной реорганизации RegionsList.  ажда€ содержит: исходное и результирующее состо€ние (одинаковое дл€ P-и-S-List),
// набор последовательных вставок и захватов (true - вставка, false - захват, см. сигнатуру) дл€ реорганизации RegionsList и результирующий контент.
const std::tuple<ListState, ListState, std::vector<std::tuple<RegionP<CELL>, bool>>, std::vector<RegionP<CELL>>> ReorganizeComplex::test_sequences[6] =
{
    {
        {3,0,1,2,1,1}, {48,32,9,7,9,41}, 
        {
            {r[14], true}, {r[15], true}, {r[14],false}, {r[16], true}, {r[17], true}, {r[17],false}, {r[16],false}, {r[14], true}, {r[13], true}, {r[12], true},
            {r[11], true}, {r[12],false}, {r[11],false}, {r[16], true}, {r[17], true}, {r[18], true}, {r[19], true}, {r[20], true}, {r[21], true}, {r[22], true},
            {r[22],false}, {r[21],false}, {r[12], true}, {r[11], true}, {r[10], true}, {r[9],  true}, {r[8],  true}, {r[7],  true}, {r[6],  true}, {r[5],  true},
            {r[4],  true}, {r[3],  true}, {r[2],  true}, {r[1],  true}, {r[4], false}, {r[3], false}, {r[2], false}, {r[1], false}, {r[21], true}, {r[22], true},
            {r[23], true}, {r[24], true}, {r[25], true}, {r[26], true}, {r[27], true}, {r[28], true}, {r[29], true}, {r[30], true}, {r[31], true}, {r[32], true},
            {r[33], true}, {r[34], true}, {r[35], true}, {r[36], true}
        },
        std::vector<RegionP<CELL>>( r+5, r+37 )
    },
    {
        {3,0,1,2,1,1}, {48,32,7,9,7,39},
        {
            {r[21], true}, {r[20], true}, {r[21],false}, {r[19], true}, {r[18], true}, {r[18],false}, {r[19],false}, {r[21], true}, {r[22], true}, {r[23], true},
            {r[24], true}, {r[24],false}, {r[23],false}, {r[19], true}, {r[18], true}, {r[17], true}, {r[16], true}, {r[15], true}, {r[14], true}, {r[13], true},
            {r[13],false}, {r[14],false}, {r[15],false}, {r[23], true}, {r[24], true}, {r[25], true}, {r[26], true}, {r[27], true}, {r[28], true}, {r[29], true},
            {r[30], true}, {r[31], true}, {r[32], true}, {r[33], true}, {r[34], true}, {r[35], true}, {r[35],false}, {r[34],false}, {r[33],false}, {r[15], true},
            {r[14], true}, {r[13], true}, {r[12], true}, {r[11], true}, {r[10], true}, {r[9],  true}, {r[8],  true}, {r[7],  true}, {r[6],  true}, {r[5],  true},
            {r[4],  true}, {r[3],  true}, {r[2],  true}, {r[1],  true}
        },
        std::vector<RegionP<CELL>>( r+1, r+33 )
    },
    {
        {4,0,2,2,2,2}, {32,22,6,4,6,28},
        {
            {r[17], true}, {r[18], true}, {r[19], true}, {r[20], true}, {r[20],false}, {r[16], true}, {r[15], true}, {r[14], true}, {r[13], true}, {r[19],false},
            {r[12], true}, {r[11], true}, {r[10], true}, {r[9],  true}, {r[8],  true}, {r[7],  true}, {r[6],  true}, {r[5],  true}, {r[5], false}, {r[6], false},
            {r[7], false}, {r[19], true}, {r[20], true}, {r[21], true}, {r[22], true}, {r[23], true}, {r[24], true}, {r[25], true}, {r[26], true}, {r[27], true},
            {r[28], true}, {r[29], true}
        },
        std::vector<RegionP<CELL>>( r+8, r+30 )
    },
    {
        {4,0,2,2,2,2}, {32,22,4,6,4,26},
        {
            {r[18], true}, {r[17], true}, {r[16], true}, {r[15], true}, {r[15],false}, {r[16],false}, {r[19], true}, {r[20], true}, {r[21], true}, {r[22], true},
            {r[23], true}, {r[17],false}, {r[24], true}, {r[25], true}, {r[26], true}, {r[27], true}, {r[28], true}, {r[29], true}, {r[30], true}, {r[31], true},
            {r[31],false}, {r[30],false}, {r[17], true}, {r[16], true}, {r[15], true}, {r[14], true}, {r[13], true}, {r[12], true}, {r[11], true}, {r[10], true},
            {r[9],  true}, {r[8],  true}
        },
        std::vector<RegionP<CELL>>( r+8, r+30 )
    },
    {
        {5,0,2,3,2,2}, {40,27,6,7,6,33},
        {
            {r[17], true}, {r[18], true}, {r[19], true}, {r[20], true}, {r[21], true}, {r[21],false}, {r[20],false}, {r[16], true}, {r[15], true}, {r[14], true},
            {r[13], true}, {r[12], true}, {r[11], true}, {r[11],false}, {r[12],false}, {r[20], true}, {r[21], true}, {r[22], true}, {r[23], true}, {r[24], true},
            {r[25], true}, {r[26], true}, {r[27], true}, {r[28], true}, {r[29], true}, {r[29],false}, {r[28],false}, {r[27],false}, {r[12], true}, {r[11], true},
            {r[10], true}, {r[9],  true}, {r[8],  true}, {r[7],  true}, {r[6],  true}, {r[5],  true}, {r[4],  true}, {r[3],  true}, {r[2],  true}, {r[1],  true},
            {r[0],  true}
        },
        std::vector<RegionP<CELL>>( r+0, r+27 )
    },
    {
        {5,0,2,3,2,2}, {40,27,7,6,7,34},
        {
            {r[19], true}, {r[18], true}, {r[17], true}, {r[16], true}, {r[15], true}, {r[15],false}, {r[16],false}, {r[20], true}, {r[21], true}, {r[22], true},
            {r[23], true}, {r[24], true}, {r[25], true}, {r[25],false}, {r[24],false}, {r[16], true}, {r[15], true}, {r[14], true}, {r[13], true}, {r[12], true},
            {r[11], true}, {r[10], true}, {r[9],  true}, {r[8],  true}, {r[7],  true}, {r[7], false}, {r[8], false}, {r[9], false}, {r[24], true}, {r[25], true},
            {r[26], true}, {r[27], true}, {r[28], true}, {r[29], true}, {r[30], true}, {r[31], true}, {r[32], true}, {r[33], true}, {r[34], true}, {r[35], true},
            {r[36], true}
        },
        std::vector<RegionP<CELL>>( r+10, r+37 )
    }
};
