#pragma once

#include "Error_RegionsList.h"
#include "Error_Custom.h"
#include "LogInfo.h"
#include "Types.h"

#include <stdint.h>
#include <vector>

#include "Utilites.hpp"

static const uint8_t g_minimum_capacity = 3;    // �� �������

class rl_manip;

enum class FieldsState: uint8_t {
    NONE,
    R_AND_L_EMPTY,
    R_AND_L_NOT_EMPTY,
    R_EMPTY_L_TOO_SMALL,
    R_EMPTY_L_QUITE_WIDE,
    L_EMPTY_R_TOO_SMALL,
    L_EMPTY_R_QUITE_WIDE
};

template<class T>
class RegionsList
{
public:
    friend class rl_manip;

    RegionsList( size_t capacity = g_minimum_capacity );

    RegionsList( size_t capacity, RegionP<T> managed_reg );

    ~RegionsList();

    Error_BasePtr ReleaseRegion( const RegionP<T> &region );

    Error_BasePtr GrabRegion( size_t size, T **out );


private:
    size_t m_p_list_size;
    size_t m_p_list_capacity;
    size_t m_p_list_spaceLeft;
    size_t m_p_list_spaceRight;
    RegionP<T> *m_p_list;
    RegionP<T> *m_p_list_begin;
    RegionP<T> *m_p_list_end;
    size_t m_s_list_size;
    size_t m_s_list_capacity;
    size_t m_s_list_spaceLeft;
    size_t m_s_list_spaceRight;
    RegionS<T> *m_s_list;
    RegionS<T> *m_s_list_begin;
    RegionS<T> *m_s_list_end;

    template<class ListType>
    Error_BasePtr ReorganizeList();                                         // ���������� ���������������� ������� P- ��� S-List ��� ��������� ���, ����� ����������� ������� ����� ���������

    template<class ListType>
    Error_BasePtr ExpandList();                                             // ��������� capacity P- ��� S-List �� ������� ��������� �������� � �����

    template<class ListType>
    Error_BasePtr ShiftContentLeft( size_t n );                             // �������� ������� P- ��� S-List �� n ������� �����

    template<class ListType>
    Error_BasePtr ShiftContentRight( size_t n );                            // �������� ������� P- ��� S-List �� n ������� ������

    template<class ListType>
    void DelFrom_List( size_t index );                                      // ������� �� ������� ������� �� S- ��� P-List, � ������� ������ ������� �������� = 0

    Error_BasePtr InserTo_S_List( const RegionS<T>& ins, size_t index );    // ��������� �� ������� ������� � S-List, � ������� ������ ������� �������� = 0

    template<class ListType>
    FieldsState GetFieldState();                                            // ���������� ��������� ����� P- ��� S-List
};


template<class T>
RegionsList<T>::RegionsList( size_t capacity, RegionP<T> managed_reg ) : RegionsList( capacity )
{
    Error_BasePtr err = ReleaseRegion( managed_reg );                                                   TRACE_CUSTOM_THR_ERR( err, "RegionsList error during initial Release()" );
}


template<class T>
RegionsList<T>::RegionsList( size_t capacity )
    : m_p_list_size( 0 )
    , m_p_list_capacity( capacity )
    , m_p_list_spaceLeft( 0 )
    , m_p_list_spaceRight( 0 )
    , m_p_list( nullptr )
    , m_p_list_begin( nullptr )
    , m_p_list_end( nullptr )
    , m_s_list_size( 0 )
    , m_s_list_capacity( capacity )
    , m_s_list_spaceLeft( 0 )
    , m_s_list_spaceRight( 0 )
    , m_s_list( nullptr )
    , m_s_list_begin( nullptr )
    , m_s_list_end( nullptr )
{
    // �������� ������ ��� ������� ����������. ������� - ��� ������. ��������������
    if (capacity < g_minimum_capacity) {
        m_p_list_capacity = m_s_list_capacity = g_minimum_capacity;
    }
    Error_BasePtr err = nullptr;
    err = utils::Attempt_calloc<RegionP<T>>( 20, 100, m_p_list_capacity, m_p_list );                    TRACE_REGIONSLIST_THR_ERR( err, ERL_Type::P_LIST_ALLOCATION );
    err = utils::Attempt_calloc<RegionS<T>>( 20, 100, m_s_list_capacity, m_s_list );                    TRACE_REGIONSLIST_THR_ERR( err, ERL_Type::S_LIST_ALLOCATION );

    // �������������� ������ ������/������� ���� � �������
    m_p_list_spaceLeft = m_p_list_capacity / 2;
    m_p_list_spaceRight = m_p_list_capacity - m_p_list_spaceLeft;
    m_s_list_spaceLeft = m_p_list_spaceLeft;
    m_s_list_spaceRight = m_p_list_spaceRight;

    // ������ ������� - � ��������� ���������� �������� (��� ���������� ��������� � ������ � ����� ��� �����������������)
    m_p_list_begin = m_p_list + m_p_list_spaceLeft;
    m_p_list_end = m_p_list_begin;
    m_s_list_begin = m_s_list + m_s_list_spaceLeft;
    m_s_list_end = m_s_list_begin;
}


template<class T>
RegionsList<T>::~RegionsList()
{
    if (m_p_list) { free( m_p_list ); }
    if (m_s_list) { free( m_s_list ); }
}


template<class T>
Error_BasePtr RegionsList<T>::ReleaseRegion( const RegionP<T>& region )
{
    // ���� ������ ����� - ������ ��������� � ��� ��������. ��� ���� ����������� ������������ ������.
    if (!m_p_list_size) {
        *m_p_list_begin = region;
        *m_s_list_begin = { region.start, region.size, 1 };
        ++m_s_list_size;
        ++m_p_list_size;
        --m_p_list_spaceRight;
        --m_s_list_spaceRight;
        ++m_p_list_end;
        ++m_s_list_end;
        return nullptr;
    }

    // ���� � ������� ������ ���� ��������
    if (m_p_list_size == 1)
    {                                           // �������� � Pointers-�������
        // ���� ����� ������ ��� ���� � ������ - ���������� ������
        if (region.start == m_p_list_begin->start) {
            return ERR_REGIONSLIST( ERL_Type::EXISTING_REG_INSERTION, region.start, region.size );
        }
        // ���� �������� ������ (� �����)
        if (region.start > m_p_list_begin->start)
        {
            // ���� ������ ������������� � ����� - ������
            if (m_p_list_begin->start + m_p_list_begin->size > region.start) {
                return ERR_REGIONSLIST( ERL_Type::OVERLAPPED_REG_INSERTION, region.start, region.size );
            }
            // ������ �� ����� �����? ���� �� - �������� �� �����������, � �������������� ��������� (� ����� �������).
            if ((m_p_list_begin->start + m_p_list_begin->size) == region.start) {
                m_p_list_begin->size += region.size;
                m_s_list_begin->size = m_p_list_begin->size;
                return nullptr;
            }
            // ��������� ����� ������ ������. ����� ������ �����������, ����� ������ ���������� ������.
            if (!m_p_list_spaceRight) {
                auto err = ReorganizeList<RegionP<T>>();                            TRACE_CUSTOM_RET_ERR( err, "Can't reorganize P-List (contains 1 element, insertion to the end, R is empty)." );
            }
            *m_p_list_end++ = region;
            ++m_p_list_size;
            --m_p_list_spaceRight;
        }
        // ���� �������� ����� (� ������)
        else
        {
            // ���� ������ ������������� � ������ - ������
            if (region.start + region.size > m_p_list_begin->start) {
                return ERR_REGIONSLIST( ERL_Type::OVERLAPPED_REG_INSERTION, region.start, region.size );
            }
            // ������ �� ������ �����? ���� �� - �������� �� �����������, � �������������� ��������� (� ����� �������).
            if (m_p_list_begin->start == region.start + region.size) {
                m_p_list_begin->start = region.start;
                m_p_list_begin->size += region.size;
                *m_s_list_begin = { m_p_list_begin->start, m_p_list_begin->size, 1 };
                return nullptr;
            }
            // ��������� ����� ������ �����. ����� ����� �����������, ������ ������ ���������� �����.
            if (!m_p_list_spaceLeft) {
                auto err = ReorganizeList<RegionP<T>>();                            TRACE_CUSTOM_RET_ERR( err, "Can't reorganize P-List (contains 1 element, insertion to begin, L is empty)." );
            }
            *(--m_p_list_begin) = region;
            ++m_p_list_size;
            --m_p_list_spaceLeft;
        }
                                                // �������� � Size-�������
        // ���� �������� ������
        if (region.size > m_s_list_begin->size) {
            if (!m_s_list_spaceRight) {
                auto err = ReorganizeList<RegionS<T>>();                            TRACE_CUSTOM_RET_ERR( err, "Can't reorganize S-List (contains 1 element, insertion by size to the end, R is empty)." );
            }
            *m_s_list_end++ = { region.start, region.size, 1 };
            --m_s_list_spaceRight;
        }
        // ���� �������� �����
        else if (region.size < m_s_list_begin->size) {
            if (!m_s_list_spaceLeft) {
                auto err = ReorganizeList<RegionS<T>>();                            TRACE_CUSTOM_RET_ERR( err, "Can't reorganize S-List (contains 1 element, insertion by size to begin, L is empty)." );
            }
            *(--m_s_list_begin) = { region.start, region.size, 1 };
            --m_s_list_spaceLeft;
        }
        // ���� ������ ������������ ��������� ��������� � ������� ��� ���������� - ��������� �� �� ����������.
        else {
            if (region.start > m_s_list_begin->start) {
                if (!m_s_list_spaceRight) {
                    auto err = ReorganizeList<RegionS<T>>();                        TRACE_CUSTOM_RET_ERR( err, "Can't reorganize S-List (contains 1 element, insertion by start to the end, R is empty)." );
                }
                m_s_list_begin->count = 2;
                *m_s_list_end++ = { region.start, region.size, 0 };
                --m_s_list_spaceRight;
            }
            else {
                if (!m_s_list_spaceLeft) {
                    auto err = ReorganizeList<RegionS<T>>();                        TRACE_CUSTOM_RET_ERR( err, "Can't reorganize S-List (contains 1 element, insertion by start to begin, L is empty)." );
                }
                m_s_list_begin->count = 0;
                *(--m_s_list_begin) = { region.start, region.size, 2 };
                --m_s_list_spaceLeft;
            }
        }
        ++m_s_list_size;
    }
    // ���� ���������� �����
    else {
        RegionS<T> regToInsert = { 0 };
        RegionS<T> regsToDelete[2];
        uint8_t delCount = 0;
                                                /* �������� � Pointers-������� */
        // ���������� ������ �������
        bool founded;
        size_t index = utils::lower_bound( m_p_list_begin, m_p_list_size, region.start, founded );

        // ���� ����� ������ ��� ���� � ������ - ���������� ������
        if (founded) {
            return ERR_REGIONSLIST( ERL_Type::EXISTING_REG_INSERTION, region.start, region.size );
        }

        // ���� ������� � ������
        if (index == 0)
        {
            // ���� ������ ������������� � ������ - ������
            if (region.start + region.size > m_p_list_begin->start) {
                return ERR_REGIONSLIST( ERL_Type::OVERLAPPED_REG_INSERTION, region.start, region.size );
            }
            // ������ �� ������ �����?
            if (region.start + region.size == m_p_list_begin->start) {
                delCount = 1;
                regsToDelete[0] = { m_p_list_begin->start, m_p_list_begin->size, 0 };
                m_p_list_begin->start = region.start;
                m_p_list_begin->size += region.size;
                regToInsert = { m_p_list_begin->start, m_p_list_begin->size, 0 };
            }
            else {
                // ����� ����� �� �������?
                if (!m_p_list_spaceLeft) {
                    auto err = ReorganizeList<RegionP<T>>();                        TRACE_CUSTOM_RET_ERR( err, "Can't reorganize P-List (insertion to begin, L is empty)." );
                }
                *(--m_p_list_begin) = region;
                --m_p_list_spaceLeft;
                ++m_p_list_size;
                regToInsert = { region.start, region.size, 0 };
            }
        }
        // ���� ������� � �����
        else if (index == m_p_list_size)
        {
            RegionP<T>* lastReg = (m_p_list_end - 1);

            // ���� ������ ������������� � ����� - ������
            if (lastReg->start + lastReg->size > region.start) {
                return ERR_REGIONSLIST( ERL_Type::OVERLAPPED_REG_INSERTION, region.start, region.size );
            }
            // ������ �� ����� �����?
            if ((lastReg->start + lastReg->size) == region.start)
            {
                delCount = 1;
                regsToDelete[0] = { lastReg->start, lastReg->size, 0 };
                lastReg->size += region.size;
                regToInsert = { lastReg->start, lastReg->size, 0 };
            }
            else
            {
                // ����� ������ �� ������� ��� ������� 1 ��������?
                if (!m_p_list_spaceRight) {
                    auto err = ReorganizeList<RegionP<T>>();                        TRACE_CUSTOM_RET_ERR( err, "Can't reorganize P-List (insertion to the end, R is empty)." );
                }
                *m_p_list_end++ = region;
                --m_p_list_spaceRight;
                ++m_p_list_size;
                regToInsert = { region.start, region.size, 0 };
            }
        }
        // ���� ������� � ��������
        else {
            RegionP<T>* right = m_p_list_begin + index;
            RegionP<T>* left = right - 1;

            // ���� ������ ������������� � ����� ��� ������ - ������
            if (((left->start + left->size) > region.start) || ((region.start + region.size) > right->start)) {
                return ERR_REGIONSLIST( ERL_Type::OVERLAPPED_REG_INSERTION, region.start, region.size );
            }

            // ���� ����� � ������ ������ - �������
            if (((left->start + left->size) == region.start) && ((region.start + region.size) == right->start))
            {
                delCount = 2;
                regsToDelete[0] = { left->start, left->size, 0 };
                regsToDelete[1] = { right->start, right->size, 0 };

                // ������ ������� ����� � ����� ������?
                if (index >= m_p_list_size / 2) {
                    left->size += region.size + right->size;        // ������������ ������ ������ �� size
                    regToInsert = { left->start, left->size, 0 };

                    memmove( right, right + 1, (m_p_list_size - index - 1) * sizeof( RegionP<T> ) );    // �������� ��-��, ������� � ������+1, �� 1 ������� ����� (����������� ������� ������)
                    ++m_p_list_spaceRight;
                    *(--m_p_list_end) = { 0 };
                }
                else {
                    right->start = left->start;                     // ������������ ������� ������ �� start � size
                    right->size += left->size + region.size;
                    regToInsert = { right->start, right->size, 0 };

                    memmove( m_p_list_begin + 1, m_p_list_begin, (index - 1) * sizeof( RegionP<T> ) );  // �������� ��-��, ������� � Begin, �� 1 ���. ������ (����������� ������ ������)
                    ++m_p_list_spaceLeft;
                    *m_p_list_begin++ = { 0 };
                }
                --m_p_list_size;
            }
            // ���� ������ ������ ����� - �������
            else if ((region.start + region.size) == right->start)
            {
                delCount = 1;
                regsToDelete[0] = { right->start, right->size, 0 };
                right->start = region.start;                // ������������ ������� ������ �� start � size
                right->size += region.size;
                regToInsert = { right->start, right->size, 0 };
            }
            // ���� ������ ����� ����� - �������
            else if ((left->start + left->size) == region.start)
            {
                delCount = 1;
                regsToDelete[0] = { left->start, left->size, 0 };
                left->size += region.size;                  // ������������ ������ ������ �� size
                regToInsert = { left->start, left->size, 0 };
            }
            // ���� ��� ������� �������
            else
            {
                // ������ ������� ����� � ����� ������?
                if (index >= m_p_list_size / 2)
                {
                    // ����� ������ �� ������� ��� ������� 1 ��������?
                    if (m_p_list_spaceRight == 0) {
                        auto err = ReorganizeList<RegionP<T>>();                    TRACE_CUSTOM_RET_ERR( err, "Can't reorganize P-List (closer to the end insertion, R is empty)." );
                        right = m_p_list_begin + index;     // right ����� ����������� ������ ���������. �������������� ���.
                    }
                    memmove( right + 1, right, (m_p_list_size - index) * sizeof( RegionP<T> ) );        // �������� ��-��, ������� � ������+1 �� 1 ���. ������ (����������� ����� ��� �������)

                    *right = region;
                    --m_p_list_spaceRight;
                    ++m_p_list_end;
                    regToInsert = { region.start, region.size, 0 };
                }
                else {
                    // ����� ����� �� ������� ��� ������� 1 ��������?
                    if (m_p_list_spaceLeft == 0) {
                        auto err = ReorganizeList<RegionP<T>>();                    TRACE_CUSTOM_RET_ERR( err, "Can't reorganize P-List (closer to begin insertion, L is empty)." );
                        right = m_p_list_begin + index;     // right � left ����� ����������� ������ ���������. �������������� ��.
                        left = right - 1;
                    }
                    memmove( m_p_list_begin - 1, m_p_list_begin, (index + 1) * sizeof( RegionP<T> ) );  // �������� ��-��, ������� � Begin, �� 1 ���. ����� (����������� ����� ��� �������)

                    *left = region;
                    --m_p_list_spaceLeft;
                    --m_p_list_begin;
                    regToInsert = { region.start, region.size, 0 };
                }
                ++m_p_list_size;
            }
        }
                                                                    /* �������� � Size-������� */
        /*   ��������  ���������   */
        for (uint8_t i = 0; i < delCount; ++i)
        {
            // ���������� ������ ���������� ��������
            index = utils::lower_bound( m_s_list_begin, m_s_list_size, regsToDelete[i].size );

            // ������� � ����� size �� ������������?
            if ((m_s_list_begin + index)->count != 1)
            {
                size_t subIndex = utils::lower_bound( m_s_list_begin + index, (m_s_list_begin + index)->count, regsToDelete[i].start );

                // �������� ������?
                if (subIndex) {
                    (m_s_list_begin + index)->count--;
                    index += subIndex;
                }
                else {
                    (m_s_list_begin + index + 1)->count = (m_s_list_begin + index)->count - 1;
                }
            }
            DelFrom_List<RegionS<T>>( index );
        }
        /*   ���������� ��������   */
        index = utils::lower_bound( m_s_list_begin, m_s_list_size, regToInsert.size );      // ���������� ������ �������

        // ������� � ����� size ��� ���� � ������?
        if (index != m_s_list_size && (m_s_list_begin + index)->size == regToInsert.size)
        {
            // ������� � ����� size ���� ������������ � ������?
            if ((m_s_list_begin + index)->count == 1)
            {
                // ������� ������?
                if (regToInsert.start > (m_s_list_begin + index)->start) {
                    (m_s_list_begin + index)->count = 2;
                    index += 1;
                }
                else {
                    (m_s_list_begin + index)->count = 0;
                    regToInsert.count = 2;
                }
            }
            else {
                size_t subindex = utils::lower_bound( m_s_list_begin + index, (m_s_list_begin + index)->count, regToInsert.start );

                // ������� ������?
                if (subindex) {
                    (m_s_list_begin + index)->count++;
                    index += subindex;
                }
                else {
                    regToInsert.count = (m_s_list_begin + index)->count + 1;
                    (m_s_list_begin + index)->count = 0;
                }
            }
        }
        else {
            regToInsert.count = 1;
        }
        auto err = InserTo_S_List( regToInsert, index );
        TRACE_CUSTOM_RET_ERR( err, "Can't insert the element in S-List\nElement: " + utils::to_string( regToInsert ) + "\nInsertion index: " + std::to_string( index ) );
    }
    return nullptr;
}


template<class T>
Error_BasePtr RegionsList<T>::GrabRegion( size_t size, T **out )
{
    // ���� S-List ������ - ������
    if (!m_s_list_size) {
        return ERR_REGIONSLIST( ERL_Type::GRAB_FROM_EMPTY_LIST );
    }
    if( !size ) {
        return ERR_PARAM( 1, "size_t size", "Non zero", "0" );
    }
    // ���� � ������� 1 ������
    else if (m_s_list_size == 1)
    {
        // ���� ������ �������������� ������� ������ ���, ��� ��� - ������
        if (m_s_list_begin->size < size) {
            return ERR_REGIONSLIST( ERL_Type::CONSISTENT_REG_NOTFOUND, size );
        }
        // ���� ������ �������������� ������� ������ ���, ��� ���� - ������������ ������� � P- � S-List
        if (m_s_list_begin->size > size) {
            *out = m_s_list_begin->start;
            m_s_list_begin->start += size;
            m_s_list_begin->size -= size;
            m_p_list_begin->start = m_s_list_begin->start;
            m_p_list_begin->size = m_s_list_begin->size;
        }
        // ���� ������ �������������� ������� ����� ���, ��� ���� - ������� ������� �� P- � S-List
        else {
            *out = m_s_list_begin->start;
            *m_s_list_begin = { 0 };
            m_s_list_end--;
            m_s_list_size--;
            m_s_list_spaceRight++;
            *m_p_list_begin = { 0 };
            m_p_list_end--;
            m_p_list_size--;
            m_p_list_spaceRight++;
        }
        return nullptr;
    }
    // ���� � ������� ����� ��������
    else {
        T* toDel_or_Modify = nullptr;   // RegionP.start ��� �������� ��� ����������� � P-List
        T* newStart = nullptr;          // ����� .start
        size_t newSize;                 // ����� .size
        bool mode = false;              // true = Modify, false = Delete (�� ���������)

        size_t index = utils::lower_bound( m_s_list_begin, m_s_list_size, size );

        // ���� ���������� ������ �� ������ - ������
        if (index == m_s_list_size)	{
            return ERR_REGIONSLIST( ERL_Type::CONSISTENT_REG_NOTFOUND, size );
        }
                                                                    /* �������� � S-List */
        RegionS<T> foundedRegion = *(m_s_list_begin + index);
        *out = foundedRegion.start;

        // �� ��������� ������ �� P-List ����� ����� (�� �������� .start ���������� � S-List �������)
        toDel_or_Modify = foundedRegion.start;

        // ���� ��������� ������ �� ������ ������ �������������� - ������ � P-List ����� ������������� (�� �����)
        if (foundedRegion.size > size) {
            mode = true;
            newStart = toDel_or_Modify + size;
            newSize = foundedRegion.size - size;
        }
        /*   ������� ��������� ������ �� S-List   */
        if (foundedRegion.count != 1) {
            (m_s_list_begin + index + 1)->count = foundedRegion.count - 1;
        }
        DelFrom_List<RegionS<T>>( index );

        if (mode) {
            /*   �������� � S-List ����� ������, � ���������� .start � .size   */
            RegionS<T> toInsert_in_SList = { newStart, newSize, 0 };
            index = utils::lower_bound( m_s_list_begin, m_s_list_size, toInsert_in_SList.size );

            // ������� � ����� size ��� ���� � ������?
            if ((m_s_list_begin + index)->size == toInsert_in_SList.size)
            {
                // ������� � ����� size ���� ������������ � ������?
                if ((m_s_list_begin + index)->count == 1)
                {
                    // ������� ������?
                    if (toInsert_in_SList.start > (m_s_list_begin + index)->start) {
                        (m_s_list_begin + index)->count = 2;
                        index += 1;
                    }
                    else {
                        (m_s_list_begin + index)->count = 0;
                        toInsert_in_SList.count = 2;
                    }
                } else {
                    size_t subindex = utils::lower_bound( m_s_list_begin + index, (m_s_list_begin + index)->count, toInsert_in_SList.start );

                    // ������� ������?
                    if (subindex) {
                        (m_s_list_begin + index)->count++;
                        index += subindex;
                    }
                    else {
                        toInsert_in_SList.count = (m_s_list_begin + index)->count + 1;
                        (m_s_list_begin + index)->count = 0;
                    }
                }
            } else {
                toInsert_in_SList.count = 1;
            }
            auto err = InserTo_S_List( toInsert_in_SList, index );
            TRACE_CUSTOM_RET_ERR( err, "Can't insert the element into S-List\nElement: " + utils::to_string( toInsert_in_SList ) + "\nInsertion index: " + std::to_string( index ) );
        }
                                                                    /* �������� � P-List */
        bool founded;
        index = utils::lower_bound( m_p_list_begin, m_p_list_size, toDel_or_Modify , founded );

        // ���� ������� � ����� .start � P-List �� ������ - ������
        if (!founded) {
            return ERR_REGIONSLIST( ERL_Type::REG_WITH_SUCH_START_NOTFOUND, toDel_or_Modify );
        }
        // ��������� ������ � P-List ��������������?
        if (mode) {
            (m_p_list_begin + index)->start = newStart;
            (m_p_list_begin + index)->size = newSize;
            return nullptr;
        }
        DelFrom_List<RegionP<T>>( index );
        return nullptr;
    }
}


template<class T>
template<class ListType>
Error_BasePtr RegionsList<T>::ReorganizeList()
{
    size_t shift_L, shift_R;
    if constexpr( std::is_same_v<ListType, RegionP<T>> ) {
        shift_L = m_p_list_spaceLeft / 2;
        shift_R = m_p_list_spaceRight / 2;
    }
    else if constexpr( std::is_same_v<ListType, RegionS<T>> ) {
        shift_L = m_s_list_spaceLeft / 2;
        shift_R = m_s_list_spaceRight / 2;
    }
    else {
        return ERR_CUSTOM( "Undefined ListType received: " + std::string( typeid( ListType ).name() ) );
    }

    switch( GetFieldState<ListType>() )
    {
    case FieldsState::R_AND_L_EMPTY: {
        TRACE_CUSTOM_RET_ERR( ExpandList<ListType>(), "Failed to expand List when R and L fields are empty" );
        break;
    }
    case FieldsState::R_EMPTY_L_QUITE_WIDE: {
        TRACE_CUSTOM_RET_ERR( ShiftContentLeft<ListType>( shift_L ), "Failed to shift content for " + std::to_string( shift_L ) + "cells left when R field is empty" );
        break;
    }
    case FieldsState::L_EMPTY_R_QUITE_WIDE: {
        TRACE_CUSTOM_RET_ERR( ShiftContentRight<ListType>( shift_R ), "Failed to shift content for " + std::to_string( shift_R ) + "cells right when L field is empty" );
        break;
    }
    case FieldsState::R_EMPTY_L_TOO_SMALL: {
        TRACE_CUSTOM_RET_ERR( ExpandList<ListType>(), "Failed to expand List when R field is empty and L field too small for shifting" );
        break;
    }
    case FieldsState::L_EMPTY_R_TOO_SMALL: {
        TRACE_CUSTOM_RET_ERR( ExpandList<ListType>(), "Failed to expand List when L field is empty and R field too small for shifting" );
        break;
    }
    case FieldsState::R_AND_L_NOT_EMPTY: {
        break;
    }
    default:
        break;
    }
    return nullptr;
}


template<class T>
template<class ListType>
Error_BasePtr RegionsList<T>::ExpandList()
{
    if constexpr( std::is_same_v<ListType, RegionP<T>> )
    {
        size_t prev_capacity = m_p_list_capacity;
        size_t prev_byStart_left = m_p_list_spaceLeft;
        m_p_list_capacity *= 2;
        size_t N = m_p_list_capacity - prev_capacity;

        m_p_list_spaceLeft = ( m_p_list_spaceLeft + m_p_list_spaceRight + N ) / 2;
        m_p_list_spaceRight = m_p_list_capacity - m_p_list_spaceLeft - m_p_list_size;

        auto err = utils::Attempt_realloc<ListType>( 20, 100, m_p_list_capacity * sizeof( ListType ), m_p_list );                   TRACE_REGIONSLIST_RET_ERR( err, ERL_Type::P_LIST_ALLOCATION );

        memmove( m_p_list + m_p_list_spaceLeft - prev_byStart_left, m_p_list, prev_capacity * sizeof( RegionP<T> ) );
        memset( m_p_list, 0, m_p_list_spaceLeft * sizeof( RegionP<T> ) );
        memset( m_p_list + m_p_list_spaceLeft + m_p_list_size, 0, m_p_list_spaceRight * sizeof( RegionP<T> ) );

        m_p_list_begin = m_p_list + m_p_list_spaceLeft;
        m_p_list_end = m_p_list_begin + m_p_list_size;
        return nullptr;
    }
    else if constexpr( std::is_same_v<ListType, RegionS<T>> )
    {
        size_t prev_capacity = m_s_list_capacity; 
        size_t prev_bySize_left = m_s_list_spaceLeft;
        m_s_list_capacity *= 2;
        size_t N = m_s_list_capacity - prev_capacity;

        m_s_list_spaceLeft = ( m_s_list_spaceLeft + m_s_list_spaceRight + N ) / 2;
        m_s_list_spaceRight = m_s_list_capacity - m_s_list_spaceLeft - m_s_list_size;

        auto err = utils::Attempt_realloc<ListType>( 20, 100, m_s_list_capacity * sizeof( ListType ), m_s_list );                   TRACE_REGIONSLIST_RET_ERR( err, ERL_Type::S_LIST_ALLOCATION );

        memmove( m_s_list + m_s_list_spaceLeft - prev_bySize_left, m_s_list, prev_capacity * sizeof( RegionS<T> ) );
        memset( m_s_list, 0, m_s_list_spaceLeft * sizeof( RegionS<T> ) );
        memset( m_s_list + m_s_list_spaceLeft + m_s_list_size, 0, m_s_list_spaceRight * sizeof( RegionS<T> ) );

        m_s_list_begin = m_s_list + m_s_list_spaceLeft;
        m_s_list_end = m_s_list_begin + m_s_list_size;
        return nullptr;
    }
    else {
        return ERR_CUSTOM( "Undefined ListType received: " + std::string( typeid( ListType ).name() ) );
    }
}


template<class T>
template<class ListType>
Error_BasePtr RegionsList<T>::ShiftContentLeft( size_t n )
{
    if constexpr( std::is_same_v<ListType, RegionP<T>> )
    {
        if( n > m_p_list_spaceLeft ) {
            return ERR_PARAM( 1, "size_t n", "Less or equal P-List's space left (" + std::to_string( m_p_list_spaceLeft ) + " cells)", std::to_string( n ));
        }
        memmove( m_p_list_begin - n, m_p_list_begin, m_p_list_size * sizeof( RegionP<T> ) );
        m_p_list_begin -= n;
        m_p_list_end -= n;
        m_p_list_spaceLeft -= n;
        m_p_list_spaceRight += n;
        memset( m_p_list_end, 0, n * sizeof( RegionP<T> ) );
        return nullptr;
    }
    else if constexpr( std::is_same_v<ListType, RegionS<T>> )
    {
        if( n > m_s_list_spaceLeft ) {
            return ERR_PARAM( 1, "size_t n", "Less or equal S-List's space left (" + std::to_string( m_s_list_spaceLeft ) + " cells)", std::to_string( n ));
        }
        memmove( m_s_list_begin - n, m_s_list_begin, m_s_list_size * sizeof( RegionS<T> ) );
        m_s_list_begin -= n;
        m_s_list_end -= n;
        m_s_list_spaceLeft -= n;
        m_s_list_spaceRight += n;
        memset( m_s_list_end, 0, n * sizeof( RegionS<T> ) );
        return nullptr;
    }
    else {
        return ERR_CUSTOM( "Undefined ListType received: " + std::string( typeid( ListType ).name() ) );
    }
}


template<class T>
template<class ListType>
Error_BasePtr RegionsList<T>::ShiftContentRight( size_t n )
{
    if constexpr( std::is_same_v<ListType, RegionP<T>> )
    {
        if( n > m_p_list_spaceRight ) {
            return ERR_PARAM( 1, "size_t n", "Less or equal P-List's space right (" + std::to_string( m_p_list_spaceRight ) + " cells)", std::to_string( n ) );
        }
        memmove( m_p_list_begin + n, m_p_list_begin, m_p_list_size * sizeof( RegionP<T> ) );
        memset( m_p_list_begin, 0, n * sizeof( RegionP<T> ) );
        m_p_list_begin += n;
        m_p_list_end += n;
        m_p_list_spaceLeft += n;
        m_p_list_spaceRight -= n;
        return nullptr;
    }
    else if constexpr( std::is_same_v<ListType, RegionS<T>> )
    {
        if( n > m_s_list_spaceRight ) {
            return ERR_PARAM( 1, "size_t n", "Less or equal S-List's space right (" + std::to_string( m_s_list_spaceRight ) + " cells)", std::to_string( n ) );
        }
        memmove( m_s_list_begin + n, m_s_list_begin, m_s_list_size * sizeof( RegionS<T> ) );
        memset( m_s_list_begin, 0, n * sizeof( RegionS<T> ) );
        m_s_list_begin += n;
        m_s_list_end += n;
        m_s_list_spaceLeft += n;
        m_s_list_spaceRight -= n;
        return nullptr;
    }
    else {
        return ERR_CUSTOM( "Undefined ListType received: " + std::string( typeid( ListType ).name() ) );
    }
}


template<class T>
template<class ListType>
void RegionsList<T>::DelFrom_List( size_t index )
{
    if constexpr (std::is_same_v<ListType, RegionS<T>>)
    {
        // �������� �� ������?
        if (index == 0) {
            *m_s_list_begin++ = { 0 };
            ++m_s_list_spaceLeft;
        }
        // �������� �� �����?
        else if (index == m_s_list_size - 1) {
            *(--m_s_list_end) = { 0 };
            ++m_s_list_spaceRight;
        }
        // �������� ����� � �����?
        else if (index >= m_s_list_size / 2) {
            memmove( m_s_list_begin + index, m_s_list_begin + index + 1, (m_s_list_size - index - 1) * sizeof( RegionS<T> ) );
            *(--m_s_list_end) = { 0 };
            ++m_s_list_spaceRight;
        }
        // �������� ����� � ������?
        else {
            memmove( m_s_list_begin + 1, m_s_list_begin, index * sizeof( RegionS<T> ) );
            *m_s_list_begin++ = { 0 };
            ++m_s_list_spaceLeft;
        }
        --m_s_list_size;
    }
    else if constexpr (std::is_same_v<ListType, RegionP<T>>)
    {
        // �������� �� ������?
        if (index == 0) {
            *m_p_list_begin++ = { 0 };
            ++m_p_list_spaceLeft;
        }
        // �������� �� �����?
        else if (index == m_p_list_size - 1) {
            *(--m_p_list_end) = { 0 };
            ++m_p_list_spaceRight;
        }
        // �������� ����� � �����?
        else if (index >= m_p_list_size / 2) {
            memmove( m_p_list_begin + index, m_p_list_begin + index + 1, (m_p_list_size - index - 1) * sizeof( RegionP<T> ) );
            *(--m_p_list_end) = { 0 };
            ++m_p_list_spaceRight;
        }
        // �������� ����� � ������?
        else {
            memmove( m_p_list_begin + 1, m_p_list_begin, index * sizeof( RegionP<T> ) );
            *m_p_list_begin++ = { 0 };
            ++m_p_list_spaceLeft;
        }
        --m_p_list_size;
    }
    else {
        return;
    }
}


template<class T>
Error_BasePtr RegionsList<T>::InserTo_S_List( const RegionS<T>& ins, size_t index )
{
    // ������� � ������?
    if (index == 0) {
        if (!m_s_list_spaceLeft) {
            auto err = ReorganizeList<RegionS<T>>();                                        TRACE_CUSTOM_RET_ERR( err, "Can't reorganize S-List (insertion at the beginning, L is empty)." );
        }
        *(--m_s_list_begin) = ins;
        --m_s_list_spaceLeft;
    }
    // ������� � �����?
    else if (index == m_s_list_size) {
        if (!m_s_list_spaceRight) {
            auto err = ReorganizeList<RegionS<T>>();                                        TRACE_CUSTOM_RET_ERR( err, "Can't reorganize S-List (insertion at the end, R is empty)." );
        }
        *m_s_list_end++ = ins;
        --m_s_list_spaceRight;
    }
    // ������� ����� � �����?
    else if (index >= m_s_list_size / 2) {
        if (!m_s_list_spaceRight) {
            auto err = ReorganizeList<RegionS<T>>();                                        TRACE_CUSTOM_RET_ERR( err, "Can't reorganize S-List (closer to end insertion, R is empty)." );
        }
        memmove( m_s_list_begin + index + 1, m_s_list_begin + index, (m_s_list_size - index) * sizeof( RegionS<T> ) );
        *(m_s_list_begin + index) = ins;
        ++m_s_list_end;
        --m_s_list_spaceRight;
    }
    // ������� ����� � ������?
    else {
        if (!m_s_list_spaceLeft) {
            auto err = ReorganizeList<RegionS<T>>();                                        TRACE_CUSTOM_RET_ERR( err, "Can't reorganize S-List (closer to begin insertion, L is empty)." );
        }
        memmove( m_s_list_begin - 1, m_s_list_begin, index * sizeof( RegionS<T> ) );
        --m_s_list_begin;
        *(m_s_list_begin + index) = ins;
        --m_s_list_spaceLeft;
    }
    ++m_s_list_size;
    return nullptr;
}


template<class T>
template<class ListType>
FieldsState RegionsList<T>::GetFieldState()
{
    size_t space_L, space_R, size;
    if constexpr( std::is_same_v<ListType, RegionP<T>> ) {
        space_L = m_p_list_spaceLeft;
        space_R = m_p_list_spaceRight;
        size = m_p_list_size;
    }
    else if constexpr( std::is_same_v<ListType, RegionS<T>> ) {
        space_L = m_s_list_spaceLeft;
        space_R = m_s_list_spaceRight;
        size = m_s_list_size;
    }
    else {
        return FieldsState::NONE;
    }

    if( !( space_L || space_R ) ) {
        return FieldsState::R_AND_L_EMPTY;                  // ���� ��� ���� �����
    }
    else if( !space_R ) {
        if( space_L > size / 2 )
            return FieldsState::R_EMPTY_L_QUITE_WIDE;       // ���� ������ ���� ����� � � ����� ���������� �����
        return FieldsState::R_EMPTY_L_TOO_SMALL;            // ���� ������ ���� ����� � � ����� ������� ���� �����
    }
    else if( !space_L ) {
        if( space_R > size / 2 )
            return FieldsState::L_EMPTY_R_QUITE_WIDE;       // ���� ����� ���� ����� � � ������ ���������� �����
        return FieldsState::L_EMPTY_R_TOO_SMALL;            // ���� ����� ���� ����� � � ������ ������� ���� �����
    }
    else if( space_L && space_R ) {
        return FieldsState::R_AND_L_NOT_EMPTY;              // ���� ��� ���� P-List �� �����
    }
    else {
        return FieldsState::NONE;
    }
}