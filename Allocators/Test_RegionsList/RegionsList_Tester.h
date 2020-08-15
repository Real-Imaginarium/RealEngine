#pragma once

#include "Error_Base.h"
#include "Globals.h"
#include "RegionsList.hpp"

#include <memory>


template<class T> class RegionsList;

using Region_P = RegionP<CELL>;
using Region_S = RegionS<CELL>;
using RegionsListPtr = std::shared_ptr<RegionsList<CELL>>;

struct TestData;

class RegionsList_Tester
{
public:
    // ��������� ������� ��� ������������� ������� � RegionsList
    void Test_DoubleInserion();

    // ��������� ������� �������, ������� ������������� � ������������(�) ��������(���) � PList
    void Test_OverlappedInsertion();

    // ��������� ������, ����� RegionnsList ����
    void Test_GrabbingFromEmptyList();

    // ��������� ������ �������, ��������, ��� ���� � RegionsList
    void Test_GrabbingTooBigRegion();

    // ��������� ������ RegionsList ��� ���������� ������� (ExpandList)
    void Test_ListsExpanding();

    // ��������� ���������������� ������� � RegionsList (����� �� ������ ������ � ������ �� ��������)
    void Test_InitialReleaseRegion();

    // ��������� ������� � RegionsList, ������� ������, ����� �����, ����� �� �������� 1 ������
    void Test_SecondRelease_LeftRightAdj();

    // ��������� ������� ������� ������ ����� � RegionsList, ����� �� �������� 1 ������
    void Test_SecondRelease_Left_Less_Size();

    // ��������� ������� ������� ������ ����� � RegionsList, ����� �� �������� 1 ������
    void Test_SecondRelease_Left_Great_Size();

    // ��������� ������� ������ ������ ����� � RegionsList, ����� �� �������� 1 ������
    void Test_SecondRelease_Left_Equal_Size();

    // ��������� ������� ������� ������ ������ � RegionsList, ����� �� �������� 1 ������
    void Test_SecondRelease_Right_Less_Size();

    // ��������� ������� ������� ������ ������ � RegionsList, ����� �� �������� 1 ������
    void Test_SecondRelease_Right_Great_Size();

    // ��������� ������� ������ ������ ������ � RegionsList, ����� �� �������� 1 ������
    void Test_SecondRelease_Right_Equal_Size();

    // ��������� ������ �������, ����� � RegionsList ���� �������
    void Test_GrabbingFromSingleSizedList();

    // ��������� ������ �� RegionsList �������� ��������� ������ ��� ��������� �������� ���������� RegionsList
    void Test_GrabbingsComplex();

    // ��������� ������� � RegionsList (������ ������, ������� � ��� - �� ��� ������������). ���������� RegionsList ������ 5 ������������� �������� ������ ������
    void Test_InsertionsComplex();

    // ��������� �������� ������������ ������/������� ��/� RegionsList
    void Test_GrabbingsInsertionsRandom();

private:
    void GenerateInsertionsComplex( std::vector<std::tuple<std::string, TestData>> *out_releases );

    void GenerateGrabsComplex( std::vector<TestData>& out_grabbs );
};


class rl_manip
{
public:
    template<class ListType>
    static void SetState( const ListState& state, RegionsListPtr regList, Error_BasePtr& err );

    template<class ListType>
    static void SetContent( const std::vector<ListType>& content, RegionsListPtr regList, Error_BasePtr& err );

    template<class ListType>
    static ListState GetState( RegionsListPtr regList );

    template<class ListType>
    static ListFootprints GetFootprints( RegionsListPtr regList );

    template<class ListType>
    static std::vector<ListType> GetContent( RegionsListPtr regList, Error_BasePtr& err );

    static void GetRegionsListDetails( RegionsListPtr regList, TestData& td, Error_BasePtr& err, bool to_resulted = true );

    static void SetupRegionsList( RegionsListPtr regList, const TestData& settings, Error_BasePtr& err, bool settings_from_initial = true );
};


class rl_check
{
public:
    static void CheckMarginsPurity( RegionsListPtr regList, Error_BasePtr& err );

    static void CheckListsCompliance( const std::vector<Region_P>& p_vec, const std::vector<Region_S>& s_vec, Error_BasePtr& err );

    static ListState Validate_ListState( const ListState& state, Error_BasePtr& err );

    template<class ListType>
    static void CheckFootprintsVsState( const ListState& state, const ListFootprints& footpr, Error_BasePtr& err );

    static bool CheckState( const ListState& expected, const ListState& gained, Error_BasePtr& err );

    template<class ListType>
    static bool CheckContent( const std::vector<ListType>& expected, std::vector<ListType>& gained, Error_BasePtr& err );

    template<class ListType>
    static void CheckIfContentOutOfBounds( const std::vector<ListType>& content, Region_P bounds, Error_BasePtr& err );

    static void CheckRegionsList( RegionsListPtr regList, const TestData& expected, Error_BasePtr &err, bool check_state = true, bool check_content = true, bool check_bounds = true, bool check_resulted = true );

    static TestData CheckRegionsListInitialization( RegionsListPtr checkedRegList, size_t init_capacity, const Region_P& init_region, const Region_P& bounds, Error_BasePtr& err );

};

static void log_transactions( std::list<std::pair<std::vector<TestData>, std::vector<TestData>>> t_list, Log* l );

struct TestData
{
    /* �������� � �������������� ��������� P- � S-List */
    ListState p_listState_initial;
    ListState s_listState_initial;
    ListState p_listState_resulted;
    ListState s_listState_resulted;

    /* �������� � �������������� ������� P- � S-List */
    std::vector<Region_P> p_listContent_initial;
    std::vector<Region_S> s_listContent_initial;
    std::vector<Region_P> p_listContent_resulted;
    std::vector<Region_S> s_listContent_resulted;

    Region_P intermediate_reg;      // ���������������� ��-�������

    void Swap_Initial_and_Resulted()
    {
        ListState p_listState_temp = p_listState_initial;
        ListState s_listState_temp = s_listState_initial;
        auto p_listContent_temp = p_listContent_initial;
        auto s_listContent_temp = s_listContent_initial;
        p_listState_initial = p_listState_resulted;
        s_listState_initial = s_listState_resulted;
        p_listContent_initial = p_listContent_resulted;
        s_listContent_initial = s_listContent_resulted;
        p_listState_resulted = p_listState_temp;
        s_listState_resulted = s_listState_temp;
        p_listContent_resulted = p_listContent_temp;
        s_listContent_resulted = s_listContent_temp;
    }

    std::string to_String(uint8_t tab = 0) const {
        return
            std::string( tab, ' ' ) + "Intermediate  reg: " + utils::to_string( intermediate_reg ) + "\n" +
            std::string( tab, ' ' ) + "P-State   Initial: " + utils::to_string( p_listState_initial ) + "\n" +
            std::string( tab, ' ' ) + "         Resulted: " + utils::to_string( p_listState_resulted ) + "\n" +
            std::string( tab, ' ' ) + "S-State   Initial: " + utils::to_string( s_listState_initial ) + "\n" +
            std::string( tab, ' ' ) + "         Resulted: " + utils::to_string( s_listState_resulted ) + "\n" +
            std::string( tab, ' ' ) + "P-Content Initial: " + utils::to_string( p_listContent_initial ) + "\n" +
            std::string( tab, ' ' ) + "         Resulted: " + utils::to_string( p_listContent_resulted ) + "\n" +
            std::string( tab, ' ' ) + "S-Content Initial: " + utils::to_string( s_listContent_initial ) + "\n" +
            std::string( tab, ' ' ) + "         Resulted: " + utils::to_string( s_listContent_resulted );
    }
};