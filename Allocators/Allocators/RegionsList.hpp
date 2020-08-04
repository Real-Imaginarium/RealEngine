#pragma once

#include "Error_RegionsList.h"
#include "LogInfo.h"
#include "Types.h"

#include <stdint.h>
#include <vector>

#include "Utilites.hpp"

static const uint8_t g_minimum_capacity = 3; // Не трогать

class rl_manip;

template<class T>
class RegionsList
{
public:
    friend class rl_manip;

    RegionsList( size_t capacity );

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
    void ExpandList();

    template<class ListType>
    void DelFrom_List( size_t index );                              // Удаляет по индексу элемент из S- или P-List, в котором индекс первого элемента = 0

    void InserTo_S_List( const RegionS<T>& ins, size_t index );     // Добавляет по индексу элемент в S-List, в котором индекс первого элемента = 0
};




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
    , m_s_list_capacity( 0 )
    , m_s_list_spaceLeft( 0 )
    , m_s_list_spaceRight( 0 )
    , m_s_list( nullptr )
    , m_s_list_begin( nullptr )
    , m_s_list_end( nullptr )
{
    // Выделяем память для списков фрагментов. Минимум - для четырёх. Инициализируем
    if (m_p_list_capacity < g_minimum_capacity) {
        m_p_list_capacity = g_minimum_capacity;
    }
    m_s_list_capacity = m_p_list_capacity;

    m_p_list = reinterpret_cast<RegionP<T>*>(calloc( m_p_list_capacity, sizeof( RegionP<T> ) ));
    m_s_list = reinterpret_cast<RegionS<T>*>(calloc( m_s_list_capacity, sizeof( RegionS<T> ) ));
    memset( (void*)m_p_list, 0, m_p_list_capacity * sizeof( RegionP<T> ) );
    memset( (void*)m_s_list, 0, m_s_list_capacity * sizeof( RegionS<T> ) );

    // Инициализируем ширину левого/правого поля в списках
    m_p_list_spaceLeft = m_p_list_capacity / 2;
    m_p_list_spaceRight = m_p_list_capacity - m_p_list_spaceLeft;
    m_s_list_spaceLeft = m_p_list_spaceLeft;
    m_s_list_spaceRight = m_p_list_spaceRight;

    // Начало списков - в серединах выделенных областей (для добавления элементов в начало и конец без переаллоцирования)
    m_p_list_begin = m_p_list + m_p_list_spaceLeft;
    m_p_list_end = m_p_list_begin;
    m_s_list_begin = m_s_list + m_s_list_spaceLeft;
    m_s_list_end = m_s_list_begin;
}


template<class T>
RegionsList<T>::~RegionsList()
{
    if (m_p_list) {
        free( m_p_list );
    }
    if (m_s_list) {
        free( m_s_list );
    }
}


template<class T>
Error_BasePtr RegionsList<T>::ReleaseRegion( const RegionP<T>& region )
{
    // Если списки пусты - просто добавляем в них фрагмент. При этом уменьшаются пространства справа.
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

    // Если в списках только один фрагмент
    if (m_p_list_size == 1)
    {                                           // Работаем с Pointers-списком
        // Если такой регион уже есть в списке - возвращаем ошибку
        if (region.start == m_p_list_begin->start) {
            return std::make_shared<Error_RegionsList>( RegList_ErrType::ERR_EXISTING_REG_INSERTION, (size_t)region.start, region.size, PLACE(), "Log.txt" );
        }
        // Если попадаем правее (в конец)
        if (region.start > m_p_list_begin->start)
        {
            // Если регион перекрывается с левым - ошибка
            if (m_p_list_begin->start + m_p_list_begin->size > region.start) {
                return std::make_shared<Error_RegionsList>( RegList_ErrType::ERR_OVERLAPPED_REG_INSERTION, (size_t)region.start, region.size, PLACE(), "Log.txt" );
            }
            // Смежен ли левый сосед? Если да - фрагмент не добавляется, а модифицируется имеющийся (в обоих списках).
            if ((m_p_list_begin->start + m_p_list_begin->size) == region.start) {
                m_p_list_begin->size += region.size;
                m_s_list_begin->size = m_p_list_begin->size;
                return nullptr;
            }
            // Добавляем новый регион правее. Место справа уменьшается, конец списка сдвигается вправо.
            if (!m_p_list_spaceRight) {
                ExpandList<RegionP<T>>();
            }
            *m_p_list_end++ = region;
            ++m_p_list_size;
            --m_p_list_spaceRight;
        }
        // Если попадаем левее (в начало)
        else
        {
            // Если регион перекрывается с правым - ошибка
            if (region.start + region.size > m_p_list_begin->start) {
                return std::make_shared<Error_RegionsList>( RegList_ErrType::ERR_OVERLAPPED_REG_INSERTION, (size_t)region.start, region.size, PLACE(), "Log.txt" );
            }
            // Смежен ли правый сосед? Если да - фрагмент не добавляется, а модифицируется имеющийся (в обоих списках).
            if (m_p_list_begin->start == region.start + region.size) {
                m_p_list_begin->start = region.start;
                m_p_list_begin->size += region.size;
                *m_s_list_begin = { m_p_list_begin->start, m_p_list_begin->size, 1 };
                return nullptr;
            }
            // Добавляем новый регион левее. Место слева уменьшается, начало списка сдвигается влево.
            if (!m_p_list_spaceLeft) {
                ExpandList<RegionP<T>>();
            }
            *(--m_p_list_begin) = region;
            ++m_p_list_size;
            --m_p_list_spaceLeft;
        }
                                                // Работаем с Size-списком
        // Если попадаем правее
        if (region.size > m_s_list_begin->size) {
            if (!m_s_list_spaceRight) {
                ExpandList<RegionS<T>>();
            }
            *m_s_list_end++ = { region.start, region.size, 1 };
            --m_s_list_spaceRight;
        }
        // Если попадаем левее
        else if (region.size < m_s_list_begin->size) {
            if (!m_s_list_spaceLeft) {
                ExpandList<RegionS<T>>();
            }
            *(--m_s_list_begin) = { region.start, region.size, 1 };
            --m_s_list_spaceLeft;
        }
        // Если ширина добавляемого фрагмента совпадает с шириной уже имеющегося - сортируем их по указателям.
        else {
            if (region.start > m_s_list_begin->start) {
                if (!m_s_list_spaceRight) {
                    ExpandList<RegionS<T>>();
                }
                m_s_list_begin->count = 2;
                *m_s_list_end++ = { region.start, region.size, 0 };
                --m_s_list_spaceRight;
            }
            else {
                if (!m_s_list_spaceLeft) {
                    ExpandList<RegionS<T>>();
                }
                m_s_list_begin->count = 0;
                *(--m_s_list_begin) = { region.start, region.size, 2 };
                --m_s_list_spaceLeft;
            }
        }
        ++m_s_list_size;
    }
    // Если фрагментов много
    else {
        RegionS<T> regToInsert = { 0 };
        RegionS<T> regsToDelete[2];
        uint8_t delCount = 0;
                                                /* Работаем с Pointers-списком */
        // Определяем индекс вставки
        bool founded;
        size_t index = utils::lower_bound( m_p_list_begin, m_p_list_size, region.start, founded );

        // Если такой регион уже есть в списке - возвращаем ошибку
        if (founded) {
            return std::make_shared<Error_RegionsList>( RegList_ErrType::ERR_EXISTING_REG_INSERTION, (size_t)region.start, region.size, PLACE(), "Log.txt" );
        }

        // Если вставка в начало
        if (index == 0)
        {
            // Если регион перекрывается с правым - ошибка
            if (region.start + region.size > m_p_list_begin->start) {
                return std::make_shared<Error_RegionsList>( RegList_ErrType::ERR_OVERLAPPED_REG_INSERTION, (size_t)region.start, region.size, PLACE(), "Log.txt" );
            }
            // Смежен ли правый сосед?
            if (region.start + region.size == m_p_list_begin->start) {
                delCount = 1;
                regsToDelete[0] = { m_p_list_begin->start, m_p_list_begin->size, 0 };
                m_p_list_begin->start = region.start;
                m_p_list_begin->size += region.size;
                regToInsert = { m_p_list_begin->start, m_p_list_begin->size, 0 };
            }
            else {
                // Места слева не хватает?
                if (!m_p_list_spaceLeft) {
                    ExpandList<RegionP<T>>();
                }
                *(--m_p_list_begin) = region;
                --m_p_list_spaceLeft;
                ++m_p_list_size;
                regToInsert = { region.start, region.size, 0 };
            }
        }
        // Если вставка в конец
        else if (index == m_p_list_size)
        {
            RegionP<T>* lastReg = (m_p_list_end - 1);

            // Если регион перекрывается с левым - ошибка
            if (lastReg->start + lastReg->size > region.start) {
                return std::make_shared<Error_RegionsList>( RegList_ErrType::ERR_OVERLAPPED_REG_INSERTION, (size_t)region.start, region.size, PLACE(), "Log.txt" );
            }
            // Смежен ли левый сосед?
            if ((lastReg->start + lastReg->size) == region.start)
            {
                delCount = 1;
                regsToDelete[0] = { lastReg->start, lastReg->size, 0 };
                lastReg->size += region.size;
                regToInsert = { lastReg->start, lastReg->size, 0 };
            }
            else
            {
                // Места справа не хватает для вставки 1 элемента?
                if (!m_p_list_spaceRight) {
                    ExpandList<RegionP<T>>();
                }
                *m_p_list_end++ = region;
                --m_p_list_spaceRight;
                ++m_p_list_size;
                regToInsert = { region.start, region.size, 0 };
            }
        }
        // Если вставка в середину
        else {
            RegionP<T>* right = m_p_list_begin + index;
            RegionP<T>* left = right - 1;

            // Если регион перекрывается с левым или правым - ошибка
            if (((left->start + left->size) > region.start) || ((region.start + region.size) > right->start)) {
                return std::make_shared<Error_RegionsList>( RegList_ErrType::ERR_OVERLAPPED_REG_INSERTION, (size_t)region.start, region.size, PLACE(), "Log.txt" );
            }

            // Если левый и правый соседи - смежные
            if (((left->start + left->size) == region.start) && ((region.start + region.size) == right->start))
            {
                delCount = 2;
                regsToDelete[0] = { left->start, left->size, 0 };
                regsToDelete[1] = { right->start, right->size, 0 };

                // Индекс вставки ближе к концу списка?
                if (index >= m_p_list_size / 2) {
                    left->size += region.size + right->size;        // Модифицируем левого соседа по size
                    regToInsert = { left->start, left->size, 0 };

                    memmove( right, right + 1, (m_p_list_size - index - 1) * sizeof( RegionP<T> ) );    // Сдвигаем эл-ты, начиная с Правый+1, на 1 позицию влево (перекрываем правого соседа)
                    ++m_p_list_spaceRight;
                    *(--m_p_list_end) = { 0 };
                }
                else {
                    right->start = left->start;                     // Модифицируем правого соседа по start и size
                    right->size += left->size + region.size;
                    regToInsert = { right->start, right->size, 0 };

                    memmove( m_p_list_begin + 1, m_p_list_begin, (index - 1) * sizeof( RegionP<T> ) );  // Сдвигаем эл-ты, начиная с Begin, на 1 поз. вправо (перекрываем левого соседа)
                    ++m_p_list_spaceLeft;
                    *m_p_list_begin++ = { 0 };
                }
                --m_p_list_size;
            }
            // Если только правый сосед - смежный
            else if ((region.start + region.size) == right->start)
            {
                delCount = 1;
                regsToDelete[0] = { right->start, right->size, 0 };
                right->start = region.start;                // Модифицируем правого соседа по start и size
                right->size += region.size;
                regToInsert = { right->start, right->size, 0 };
            }
            // Если только левый сосед - смежный
            else if ((left->start + left->size) == region.start)
            {
                delCount = 1;
                regsToDelete[0] = { left->start, left->size, 0 };
                left->size += region.size;                  // Модифицируем левого соседа по size
                regToInsert = { left->start, left->size, 0 };
            }
            // Если нет смежных соседей
            else
            {
                // Индекс вставки ближе к концу списка?
                if (index >= m_p_list_size / 2)
                {
                    // Места справа не хватает для вставки 1 элемента?
                    if (m_p_list_spaceRight == 0) {
                        ExpandList<RegionP<T>>();
                        right = m_p_list_begin + index;     // right после реаллокации списка невалиден. Переопределяем его.
                    }
                    memmove( right + 1, right, (m_p_list_size - index) * sizeof( RegionP<T> ) );        // Сдвигаем эл-ты, начиная с Правый+1 на 1 поз. вправо (освобождаем место для вставки)

                    *right = region;
                    --m_p_list_spaceRight;
                    ++m_p_list_end;
                    regToInsert = { region.start, region.size, 0 };
                }
                else {
                    // Места слева не хватает для вставки 1 элемента?
                    if (m_p_list_spaceLeft == 0) {
                        ExpandList<RegionP<T>>();
                        right = m_p_list_begin + index;     // right и left после реаллокации списка невалидны. Переопределяем их.
                        left = right - 1;
                    }
                    memmove( m_p_list_begin - 1, m_p_list_begin, (index + 1) * sizeof( RegionP<T> ) );  // Сдвигаем эл-ты, начиная с Begin, на 1 поз. влево (освобождаем место для вставки)

                    *left = region;
                    --m_p_list_spaceLeft;
                    --m_p_list_begin;
                    regToInsert = { region.start, region.size, 0 };
                }
                ++m_p_list_size;
            }
        }
                                                                    /* Работаем с Size-списком */
        /*   Удаление  элементов   */
        for (uint8_t i = 0; i < delCount; ++i)
        {
            // Определяем индекс удаляемого элемента
            index = utils::lower_bound( m_s_list_begin, m_s_list_size, regsToDelete[i].size );

            // Элемент с таким size не единственный?
            if ((m_s_list_begin + index)->count != 1)
            {
                size_t subIndex = utils::lower_bound( m_s_list_begin + index, (m_s_list_begin + index)->count, regsToDelete[i].start );

                // Удаление правее?
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
        /*   Добавление элемента   */
        index = utils::lower_bound( m_s_list_begin, m_s_list_size, regToInsert.size );      // Определяем индекс вставки

        // Элемент с таким size уже есть в списке?
        if (index != m_s_list_size && (m_s_list_begin + index)->size == regToInsert.size)
        {
            // Элемент с таким size пока единственный в списке?
            if ((m_s_list_begin + index)->count == 1)
            {
                // Вставка правее?
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

                // Вставка правее?
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
        InserTo_S_List( regToInsert, index );
    }
    return nullptr;
}


template<class T>
Error_BasePtr RegionsList<T>::GrabRegion( size_t size, T **out )
{
    // Если S-List пустой - ошибка
    if (!m_s_list_size) {
        return std::make_shared<Error_RegionsList>( RegList_ErrType::ERR_GRAB_FROM_EMPTY_LIST, PLACE(), "Log.txt" );
    }
    // Если в списках 1 регион
    else if (m_s_list_size == 1)
    {
        // Если ширина запрашиваемого региона больше той, что есь - ошибка
        if (m_s_list_begin->size < size) {
            return std::make_shared<Error_RegionsList>( RegList_ErrType::ERR_CONSISTENT_REG_NOTFOUND, size, PLACE(), "Log.txt" );
        }
        // Если ширина запрашиваемого региона меньше той, что есть - модифицируем регионы в P- и S-List
        if (m_s_list_begin->size > size) {
            *out = m_s_list_begin->start;
            m_s_list_begin->start += size;
            m_s_list_begin->size -= size;
            m_p_list_begin->start = m_s_list_begin->start;
            m_p_list_begin->size = m_s_list_begin->size;
        }
        // Если ширина запрашиваемого региона равна той, что есть - удаляем регионы из P- и S-List
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
    // Если в списках много регионов
    else {
        T* toDel_or_Modify = nullptr;   // RegionP.start для удаления или модификации в P-List
        T* newStart = nullptr;          // Новый .start
        size_t newSize;                 // Новая .size
        bool mode = false;              // true = Modify, false = Delete (по умолчанию)

        size_t index = utils::lower_bound( m_s_list_begin, m_s_list_size, size );

        // Если подходящий регион не найден - ошибка
        if (index == m_s_list_size)	{
            return std::make_shared<Error_RegionsList>( RegList_ErrType::ERR_CONSISTENT_REG_NOTFOUND, PLACE(), "Log.txt" );
        }
                                                                    /* Работаем с S-List */
        RegionS<T> foundedRegion = *(m_s_list_begin + index);
        *out = foundedRegion.start;

        // По умолчанию регион из P-List будет удалён (по значению .start найденного в S-List региона)
        toDel_or_Modify = foundedRegion.start;

        // Если найденный регион по ширине больше запрашиваемого - регион в P-List будет модифицирован (не удалён)
        if (foundedRegion.size > size) {
            mode = true;
            newStart = toDel_or_Modify + size;
            newSize = foundedRegion.size - size;
        }
        /*   Удаляем найденный регион из S-List   */
        if (foundedRegion.count != 1) {
            (m_s_list_begin + index + 1)->count = foundedRegion.count - 1;
        }
        DelFrom_List<RegionS<T>>( index );

        if (mode) {
            /*   Инсертим в S-List новый регион, с изменёнными .start и .size   */
            RegionS<T> toInsert_in_SList = { newStart, newSize, 0 };
            index = utils::lower_bound( m_s_list_begin, m_s_list_size, toInsert_in_SList.size );

            // Элемент с таким size уже есть в списке?
            if ((m_s_list_begin + index)->size == toInsert_in_SList.size)
            {
                // Элемент с таким size пока единственный в списке?
                if ((m_s_list_begin + index)->count == 1)
                {
                    // Вставка правее?
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

                    // Вставка правее?
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
            InserTo_S_List( toInsert_in_SList, index );
        }
                                                                    /* Работаем с P-List */
        bool founded;
        index = utils::lower_bound( m_p_list_begin, m_p_list_size, toDel_or_Modify, founded );

        // Если элемент с таким .start в P-List не найден - ошибка
        if (!founded) {
            return std::make_shared<Error_RegionsList>( RegList_ErrType::ERR_REG_WITH_SUCH_START_NOTFOUND, (size_t)toDel_or_Modify, 0, PLACE(), "Log.txt" );
        }
        // Найденный регион в P-List модифицируется?
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
void RegionsList<T>::ExpandList()
{
    if constexpr (std::is_same_v<ListType, RegionP<T>>)
    {
        size_t prev_capacity = m_p_list_capacity;
        size_t prev_byStart_left = m_p_list_spaceLeft;
        m_p_list_capacity *= 2;
        size_t N = m_p_list_capacity - prev_capacity;

        m_p_list_spaceLeft = (m_p_list_spaceLeft + m_p_list_spaceRight + N) / 2;
        m_p_list_spaceRight = m_p_list_capacity - m_p_list_spaceLeft - m_p_list_size;

        m_p_list = reinterpret_cast<RegionP<T>*>(realloc( m_p_list, m_p_list_capacity * sizeof( RegionP<T> ) ));
        memmove( m_p_list + m_p_list_spaceLeft - prev_byStart_left, m_p_list, prev_capacity * sizeof( RegionP<T> ) );
        memset( m_p_list, 0, m_p_list_spaceLeft * sizeof( RegionP<T> ) );
        memset( m_p_list + m_p_list_spaceLeft + m_p_list_size, 0, m_p_list_spaceRight * sizeof( RegionP<T> ) );

        m_p_list_begin = m_p_list + m_p_list_spaceLeft;
        m_p_list_end = m_p_list_begin + m_p_list_size;
    }
    else if constexpr (std::is_same_v<ListType, RegionS<T>>)
    {
        size_t prev_capacity = m_s_list_capacity;
        size_t prev_bySize_left = m_s_list_spaceLeft;
        m_s_list_capacity *= 2;
        size_t N = m_s_list_capacity - prev_capacity;

        m_s_list_spaceLeft = (m_s_list_spaceLeft + m_s_list_spaceRight + N) / 2;
        m_s_list_spaceRight = m_s_list_capacity - m_s_list_spaceLeft - m_s_list_size;

        m_s_list = reinterpret_cast<RegionS<T>*>(realloc( m_s_list, m_s_list_capacity * sizeof( RegionS<T> ) ));
        memmove( m_s_list + m_s_list_spaceLeft - prev_bySize_left, m_s_list, prev_capacity * sizeof( RegionS<T> ) );
        memset( m_s_list, 0, m_s_list_spaceLeft * sizeof( RegionS<T> ) );
        memset( m_s_list + m_s_list_spaceLeft + m_s_list_size, 0, m_s_list_spaceRight * sizeof( RegionS<T> ) );

        m_s_list_begin = m_s_list + m_s_list_spaceLeft;
        m_s_list_end = m_s_list_begin + m_s_list_size;
    }
    else {
        return;
    }
}


template<class T>
template<class ListType>
void RegionsList<T>::DelFrom_List( size_t index )
{
    if constexpr (std::is_same_v<ListType, RegionS<T>>)
    {
        // Удаление из начала?
        if (index == 0) {
            *m_s_list_begin++ = { 0 };
            ++m_s_list_spaceLeft;
        }
        // Удаление из конца?
        else if (index == m_s_list_size - 1) {
            *(--m_s_list_end) = { 0 };
            ++m_s_list_spaceRight;
        }
        // Удаление ближе к концу?
        else if (index >= m_s_list_size / 2) {
            memmove( m_s_list_begin + index, m_s_list_begin + index + 1, (m_s_list_size - index - 1) * sizeof( RegionS<T> ) );
            *(--m_s_list_end) = { 0 };
            ++m_s_list_spaceRight;
        }
        // Удаление ближе к началу?
        else {
            memmove( m_s_list_begin + 1, m_s_list_begin, index * sizeof( RegionS<T> ) );
            *m_s_list_begin++ = { 0 };
            ++m_s_list_spaceLeft;
        }
        --m_s_list_size;
    }
    else if constexpr (std::is_same_v<ListType, RegionP<T>>)
    {
        // Удаление из начала?
        if (index == 0) {
            *m_p_list_begin++ = { 0 };
            ++m_p_list_spaceLeft;
        }
        // Удаление из конца?
        else if (index == m_p_list_size - 1) {
            *(--m_p_list_end) = { 0 };
            ++m_p_list_spaceRight;
        }
        // Удаление ближе к концу?
        else if (index >= m_p_list_size / 2) {
            memmove( m_p_list_begin + index, m_p_list_begin + index + 1, (m_p_list_size - index - 1) * sizeof( RegionP<T> ) );
            *(--m_p_list_end) = { 0 };
            ++m_p_list_spaceRight;
        }
        // Удаление ближе к началу?
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
void RegionsList<T>::InserTo_S_List( const RegionS<T>& ins, size_t index )
{
    // Вставка в начало?
    if (index == 0) {
        if (!m_s_list_spaceLeft) {
            ExpandList<RegionS<T>>();
        }
        *(--m_s_list_begin) = ins;
        --m_s_list_spaceLeft;
    }
    // Вставка в конец?
    else if (index == m_s_list_size) {
        if (!m_s_list_spaceRight) {
            ExpandList<RegionS<T>>();
        }
        *m_s_list_end++ = ins;
        --m_s_list_spaceRight;
    }
    // Вставка ближе к концу?
    else if (index >= m_s_list_size / 2) {
        if (!m_s_list_spaceRight) {
            ExpandList<RegionS<T>>();
        }
        memmove( m_s_list_begin + index + 1, m_s_list_begin + index, (m_s_list_size - index) * sizeof( RegionS<T> ) );
        *(m_s_list_begin + index) = ins;
        ++m_s_list_end;
        --m_s_list_spaceRight;
    }
    // Вставка ближе к началу?
    else {
        if (!m_s_list_spaceLeft) {
            ExpandList<RegionS<T>>();
        }
        memmove( m_s_list_begin - 1, m_s_list_begin, index * sizeof( RegionS<T> ) );
        --m_s_list_begin;
        *(m_s_list_begin + index) = ins;
        --m_s_list_spaceLeft;
    }
    ++m_s_list_size;
}