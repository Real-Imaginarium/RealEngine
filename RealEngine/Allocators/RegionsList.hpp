#pragma once

#include "Error_RegionsList.h"
#include "Error_Custom.h"
#include "LogInfo.h"
#include "Types.h"

#include <bitset>
#include <condition_variable>
#include <future>
#include <stdint.h>
#include <thread>
#include <vector>

#include "Utilites.hpp"


static const uint8_t g_minimum_capacity = 3;    // Не трогать

class rl_manip;

enum class FieldsState : uint8_t {
    NONE,
    R_AND_L_EMPTY,
    R_AND_L_NOT_EMPTY,
    R_EMPTY_L_TOO_SMALL,
    R_EMPTY_L_QUITE_WIDE,
    L_EMPTY_R_TOO_SMALL,
    L_EMPTY_R_QUITE_WIDE
};

//
//template<class T>
//class BasicList
//{
//public:
//    void RemoveL()
//    {
//        *m_begin = {};
//        m_begin++;
//        m_space_L++;
//        m_size--;
//    }
//
//    void RemoveR()
//    {
//        m_end--;
//        *m_end = {};
//        m_space_R++;
//        m_size--;
//    }
//
//    BasicList( size_t capacity );
//
//    size_t Size() const { return m_size; }
//
////protected:
//    void ResetState();
//
//    void ReorganizeIfNecessary( size_t field_size );
//
//    void ReorganizeList();
//
//    void ExpandList();
//
//    void ShiftContentLeft( size_t n );
//
//    void ShiftContentRight( size_t n );
//
//    FieldsState GetFieldState() const;
//
//    size_t m_size;
//    size_t m_capacity;
//    size_t m_space_L;
//    size_t m_space_R;
//    T *m_list;
//    T *m_begin;
//    T *m_end;
//};
//
//
//template<class T>
//BasicList<T>::BasicList( size_t capacity ) : m_size( 0 ), m_capacity( capacity ), m_space_L( 0 ), m_space_R( 0 ), m_list( nullptr ), m_begin( nullptr ), m_end( nullptr )
//{
//    // Выделяем память для минимум для трёх элементов. Инициализируем ширину левого/правого поля, ставим begin в середину выделенной области
//    if( m_capacity < g_minimum_capacity ) {
//        m_capacity = g_minimum_capacity;
//    }
//    utils::Attempt_calloc<T>( 20, 100, m_capacity, m_list );
//
//    m_space_L = m_capacity / 2;
//    m_space_R = m_capacity - m_space_L;
//    m_begin = m_list + m_space_L;
//    m_end = m_begin;
//}
//
//template<class T>
//void BasicList<T>::ResetState()
//{
//    m_space_L = m_capacity / 2;
//    m_space_R = m_capacity - m_space_L;
//    m_begin = m_list + m_space_L;
//    m_end = m_begin;
//}
//
//template<class T>
//void BasicList<T>::ReorganizeIfNecessary( size_t field_size )
//{
//    if( !field_size ) {
//        ReorganizeList();
//    }
//}
//
//template<class T>
//void BasicList<T>::ReorganizeList()
//{
//    switch( GetFieldState() )
//    {
//    case FieldsState::R_AND_L_EMPTY:
//    case FieldsState::R_EMPTY_L_TOO_SMALL:
//    case FieldsState::L_EMPTY_R_TOO_SMALL: { ExpandList(); break; }
//    case FieldsState::R_EMPTY_L_QUITE_WIDE: { ShiftContentLeft( m_space_L / 2 ); break; }
//    case FieldsState::L_EMPTY_R_QUITE_WIDE: { ShiftContentRight( m_space_R / 2 ); break; }
//    case FieldsState::R_AND_L_NOT_EMPTY: { break; }
//    default:
//        break;
//    }
//}
//
//template<class T>
//void BasicList<T>::ExpandList()
//{
//    size_t prev_capacity = m_capacity;
//    size_t prev_byStart_left = m_space_L;
//    m_capacity *= 2;
//    size_t N = m_capacity - prev_capacity;
//
//    m_space_L = ( m_space_L + m_space_R + N ) / 2;
//    m_space_R = m_capacity - m_space_L - m_size;
//
//    utils::Attempt_realloc<T>( 20, 100, m_capacity * sizeof( T ), m_list );
//
//    memmove( m_list + m_space_L - prev_byStart_left, m_list, prev_capacity * sizeof(T));
//    memset( m_list, 0, m_space_L * sizeof(T));
//    memset( m_list + m_space_L + m_size, 0, m_space_R * sizeof(T));
//
//    m_begin = m_list + m_space_L;
//    m_end = m_begin + m_size;
//}
//
//template<class T>
//void BasicList<T>::ShiftContentLeft( size_t n )
//{
//    memmove( m_begin - n, m_begin, m_size * sizeof(T));
//    m_begin -= n;
//    m_end -= n;
//    m_space_L -= n;
//    m_space_R += n;
//    memset( m_end, 0, n * sizeof(T));
//}
//
//template<class T>
//void BasicList<T>::ShiftContentRight( size_t n )
//{
//    memmove( m_begin + n, m_begin, m_size * sizeof(T));
//    memset( m_begin, 0, n * sizeof(T));
//    m_begin += n;
//    m_end += n;
//    m_space_L += n;
//    m_space_R -= n;
//}
//
//template<class T>
//FieldsState BasicList<T>::GetFieldState() const
//{
//    if( !( m_space_L || m_space_R ) ) {
//        return FieldsState::R_AND_L_EMPTY;                  // Если оба поля пусты
//    }
//    else if( !m_space_R ) {
//        if( m_space_L > m_size / 2 )
//            return FieldsState::R_EMPTY_L_QUITE_WIDE;       // Если правое поле пусто и в левом достаточно места
//        return FieldsState::R_EMPTY_L_TOO_SMALL;            // Если правое поле пусто и в левом слишком мало места
//    }
//    else if( !m_space_L ) {
//        if( m_space_R > m_size / 2 )
//            return FieldsState::L_EMPTY_R_QUITE_WIDE;       // Если левое поле пусто и в правом достаточно места
//        return FieldsState::L_EMPTY_R_TOO_SMALL;            // Если левое поле пусто и в правом слишком мало места
//    }
//    else if( m_space_L && m_space_R ) {
//        return FieldsState::R_AND_L_NOT_EMPTY;              // Если оба поля P-List не пусты
//    }
//    else {
//        return FieldsState::NONE;
//    }
//}
//
//
//// Список для хранения регионов различной .size, отсортированных по уникальным .start. Допускает вставку смежных регионов (см. InsertElement). Проверяет
//// валидность вставки (попытка вставки существующего или перекрывающегося слева/справа региона).
//template<class T>
//class PList : public BasicList<RegionP<T>>
//{
//public:
//    using BasicList<RegionP<T>>::m_size;
//    using BasicList<RegionP<T>>::m_capacity;
//    using BasicList<RegionP<T>>::m_space_L;
//    using BasicList<RegionP<T>>::m_space_R;
//    using BasicList<RegionP<T>>::m_list;
//    using BasicList<RegionP<T>>::m_begin;
//    using BasicList<RegionP<T>>::m_end;
//    using BasicList<RegionP<T>>::ResetState;
//    using BasicList<RegionP<T>>::ReorganizeList;
//    using BasicList<RegionP<T>>::ReorganizeIfNecessary;
//
//    PList( size_t capacity ) : BasicList<RegionP<T>>( capacity ) {}
//
//    // Добавляет регион в пустой список (перед добавлением - убедиться, что список пуст)
//    void InsertWhenEmpty( const RegionP<T> &ins )
//    {
//        *m_end++ = ins;
//        --m_space_R;
//        ++m_size;
//    }
//
//    // Добавляет регион в список, определяет его смежность с соседями, пишет соотв. знач. в вых. парамаетр ( 0 - не смежный, 1 - левый смежный,
//    // 2 - правый смежный, 3 - левый и правый смежные ), а также значение смежного(ных) и результирующего регионов (если смежен).
//    void InsertElement( const RegionP<T> &ins, uint8_t &adj, RegionP<T> &adj_L, RegionP<T> &adj_R, RegionP<T> &rslt);
//
//    // Удаляет регион с указанным .start из списка
//    void RemoveElement( const RegionP<T> &del );
//
//    // Сужает регион с указанным .start на .size
//    void NarrowDownElement( T *start, size_t size );
//
//    void Show()
//    {
//        std::cout << "Print PList:" << std::endl;
//        for( size_t i = 0; i < m_capacity; ++i )
//        {
//            if( !m_list[i].start ) {
//                std::cout << "[-]";
//            }
//            else {
//                std::cout << utils::to_string( m_list[i] );
//            }
//        }
//        std::cout << std::endl;
//    }
//};
//
//
//// Список для хранения регионов одинаковой .size, отсортированных по уникальным .start. Ожидает вставку .start НЕ смежных регионов (см. InsertElement).
//// НЕ проверяет валидность вставки.
//template<class T>
//class SubSList : public BasicList<T*>
//{
//public:
//    using BasicList<T*>::m_size;
//    using BasicList<T*>::m_capacity;
//    using BasicList<T*>::m_space_L;
//    using BasicList<T*>::m_space_R;
//    using BasicList<T*>::m_list;
//    using BasicList<T*>::m_begin;
//    using BasicList<T*>::m_end;
//    using BasicList<T*>::ResetState;
//    using BasicList<T*>::ReorganizeList;
//    using BasicList<T*>::ReorganizeIfNecessary;
//
//    SubSList( size_t capacity, size_t sz_represent )
//        : BasicList<T*>( capacity )
//        , m_size_represented( sz_represent )
//    {}
//
//    // Добавляет .start региона в список
//    void InsertElement( T* const &start );
//
//    // Удаляет .start региона из списка
//    void RemoveElement( T* const &start );
//
//    void Show()
//    {
//        //std::cout << "Print SubSList:" << std::endl;
//        for( size_t i = 0; i < m_capacity; ++i )
//        {
//            if( !m_list[i] ) {
//                std::cout << "[-]";
//            }
//            else {
//                std::cout << utils::to_string( RegionP<T>{m_list[i], m_size_represented} );
//            }
//        }
//        std::cout << std::endl;
//    }
//
//    size_t m_size_represented;                      // Ширина регионов, содержащихся в данном списке
//};
//
//
//// Пул для хранения готовых к использованию SubSlist*-элементов, используемых в DynamicSList.
//template<class T>
//class SubSListPull
//{
//public:
//    SubSListPull( size_t capacity );
//
//    // Выталкивает и возвращает верхний элемент SubSList<T>* из пула. Если пул пустой - создаёт SubSList<T>* на лету.
//    SubSList<T> *Take( size_t size_represented );
//
//    // Кладёт элемент SubSList<T>* в пул. Если места нет - увеличивает ёмкость пула.
//    void Put( SubSList<T> *obj );
//
//    size_t m_size;
//    size_t m_capacity;
//    size_t m_space_T;
//    SubSList<T> **m_pull;
//    SubSList<T> **m_pull_top;
//
//    void Show()
//    {
//        std::cout << "Print SubSListPull:" << std::endl;
//        for( size_t i = 0; i < m_capacity; ++i )
//        {
//            if( !m_pull[i] ) {
//                std::cout << "[o]" << std::endl;
//                continue;
//            }
//            m_pull[i]->Show();
//        }
//    }
//};
//
//
//// Битовая map, каждый бит соответствует .size из диапазона min_size...max_size (см. конструктор). Предоставляет возможность устанавливать и очищать,
//// а также искать бит, соответствующий определённой .size.
//class SizesBitMap
//{
//    // [0] = 0b10000000000000000000000000000000, [15] = 0b00000000000000010000000000000000, [31] = 0b00000000000000000000000000000001
//    uint32_t bit_set_mask[32] = {
//        0x80000000, 0x40000000, 0x20000000, 0x10000000, 0x08000000, 0x04000000, 0x02000000, 0x01000000, 0x00800000, 0x00400000, 0x00200000, 0x00100000, 0x00080000, 0x00040000, 0x00020000, 0x00010000,
//        0x00008000, 0x00004000, 0x00002000, 0x00001000, 0x00000800, 0x00000400, 0x00000200, 0x00000100, 0x00000080, 0x00000040, 0x00000020, 0x00000010, 0x00000008, 0x00000004, 0x00000002, 0x00000001
//    };
//    // [0] = 0b01111111111111111111111111111111, [15] = 0b11111111111111101111111111111111, [31] = 0b11111111111111111111111111111110
//    uint32_t bit_clear_mask[32] = {
//        0x7FFFFFFF, 0xBFFFFFFF, 0xDFFFFFFF, 0xEFFFFFFF, 0xF7FFFFFF, 0xFBFFFFFF, 0xFDFFFFFF, 0xFEFFFFFF, 0xFF7FFFFF, 0xFFBFFFFF, 0xFFDFFFFF, 0xFFEFFFFF, 0xFFF7FFFF, 0xFFFBFFFF, 0xFFFDFFFF, 0xFFFEFFFF,
//        0xFFFF7FFF, 0xFFFFBFFF, 0xFFFFDFFF, 0xFFFFEFFF, 0xFFFFF7FF, 0xFFFFFBFF, 0xFFFFFDFF, 0xFFFFFEFF, 0xFFFFFF7F, 0xFFFFFFBF, 0xFFFFFFDF, 0xFFFFFFEF, 0xFFFFFFF7, 0xFFFFFFFB, 0xFFFFFFFD, 0xFFFFFFFE
//    };
//    // [0] = 0b11111111111111111111111111111111, [15] = 0b00000000000000001111111111111111, [31] = 0b00000000000000000000000000000001
//    uint32_t bits_cut_mask[32] = {
//        0xFFFFFFFF, 0x7FFFFFFF, 0x3FFFFFFF, 0x1FFFFFFF, 0x0FFFFFFF, 0x07FFFFFF, 0x03FFFFFF, 0x01FFFFFF, 0x00FFFFFF, 0x007FFFFF, 0x003FFFFF, 0x001FFFFF, 0x000FFFFF, 0x0007FFFF, 0x0003FFFF, 0x0001FFFF,
//        0x0000FFFF, 0x00007FFF, 0x00003FFF, 0x00001FFF, 0x00000FFF, 0x000007FF, 0x000003FF, 0x000001FF, 0x000000FF, 0x0000007F, 0x0000003F, 0x0000001F, 0x0000000F, 0x00000007, 0x00000003, 0x00000001,
//    };
//public:
//    SizesBitMap( size_t min_size, size_t max_size )
//        : m_min_size( min_size )
//        , m_max_size( max_size )
//        , m_frames_map( nullptr )
//        , m_founded_frame( nullptr )
//    {
//        m_diapazone = max_size - min_size + 1;
//        m_frames_num = m_diapazone / 32;
//        if( m_diapazone < 32 || m_diapazone % 32 ) {
//            m_frames_num++;
//        }
//        m_frames_map = (uint32_t *)calloc( m_frames_num, sizeof( uint32_t ) );
//        m_founded_frame = (uint32_t *)malloc( 4 );
//    }
//
//    // Устанавливает бит с индексом index (перед установкой - проверить, что index валиден)
//    void Set_Bit( size_t index )
//    {
//        m_frames_map[index / 32] |= bit_set_mask[index % 32];
//    }
//
//    // Сбрасывает битбит с индексом index (перед сбросом - проверить, что index валиден)
//    void ClearBit( size_t index )
//    {
//        m_frames_map[index / 32] &= bit_clear_mask[index % 32];
//    }
//
//    // Ищет первый установленный бит с индексом >= index (выполняет lower_bound)
//    size_t LowerBoundBit( size_t index )
//    {
//        size_t start_frame = index / 32;        // фрейм, с которого начинается поиск
//
//        // Первый фрейм сканируем не весь, а начиная с бита (index % 32), соответствующего запрашиваемой .size
//        *m_founded_frame = m_frames_map[start_frame] & bits_cut_mask[index % 32];
//
//        // Если в первом ничего нет - листаем остальные
//        if( *m_founded_frame == 0 )
//        {
//            start_frame++;
//            for( ; start_frame < m_frames_num; start_frame++ )
//            {
//                if( m_frames_map[start_frame] )
//                {
//                    *m_founded_frame = m_frames_map[start_frame];
//                    break;
//                }
//            }
//        }
//        // Если нашёлся фрейм с подходящей .size - вычисляем старший бит
//        if( *m_founded_frame != 0 )
//        {
//            if( *m_founded_frame & 0xFFFF0000 ) {
//                if( *m_founded_frame & 0xFF000000 ) {
//                    if( *m_founded_frame & 0xF0000000 ) {
//                        if( *m_founded_frame & 0xC0000000 ) {
//                            if( *m_founded_frame & 0x80000000 ) return start_frame * 32;        //0x80000000
//                            return start_frame * 32 + 1;    //0x40000000
//                        }
//                        else {
//                            if( *m_founded_frame & 0x20000000 ) return start_frame * 32 + 2;    //0x20000000
//                            return start_frame * 32 + 3;    //0x10000000
//                        }
//                    }
//                    else {
//                        if( *m_founded_frame & 0x0C000000 ) {
//                            if( *m_founded_frame & 0x08000000 ) return start_frame * 32 + 4;    //0x08000000
//                            return start_frame * 32 + 5;    //0x04000000
//                        }
//                        else {
//                            if( *m_founded_frame & 0x02000000 ) return start_frame * 32 + 6;    //0x02000000
//                            return start_frame * 32 + 7;    //0x01000000
//                        }
//                    }
//                }
//                else {
//                    if( *m_founded_frame & 0x00F00000 ) {
//                        if( *m_founded_frame & 0x00C00000 ) {
//                            if( *m_founded_frame & 0x00800000 ) return start_frame * 32 + 8;    //0x00800000
//                            return start_frame * 32 + 9;    //0x00400000
//                        }
//                        else {
//                            if( *m_founded_frame & 0x00200000 ) return start_frame * 32 + 10;   //0x00200000
//                            return start_frame * 32 + 11;   //0x00100000
//                        }
//                    }
//                    else {
//                        if( *m_founded_frame & 0x000C0000 ) {
//                            if( *m_founded_frame & 0x00080000 ) return start_frame * 32 + 12;   //0x00080000
//                            return start_frame * 32 + 13;   //0x00040000
//                        }
//                        else {
//                            if( *m_founded_frame & 0x00020000 ) return start_frame * 32 + 14;   //0x00020000
//                            return start_frame * 32 + 15;   //0x00010000
//                        }
//                    }
//                }
//            }
//            else {
//                if( *m_founded_frame & 0x0000FF00 ) {
//                    if( *m_founded_frame & 0x0000F000 ) {
//                        if( *m_founded_frame & 0x0000C000 ) {
//                            if( *m_founded_frame & 0x00008000 ) return start_frame * 32 + 16;   //0x00008000
//                            return start_frame * 32 + 17;   //0x00004000
//                        }
//                        else {
//                            if( *m_founded_frame & 0x00002000 ) return start_frame * 32 + 18;   //0x00002000
//                            return start_frame * 32 + 19;   //0x00001000
//                        }
//                    }
//                    else {
//                        if( *m_founded_frame & 0x00000C00 ) {
//                            if( *m_founded_frame & 0x00000800 ) return start_frame * 32 + 20;   //0x00000800
//                            return start_frame * 32 + 21;   //0x00000400
//                        }
//                        else {
//                            if( *m_founded_frame & 0x00000200 ) return start_frame * 32 + 22;   //0x00000200
//                            return start_frame * 32 + 23;   //0x00000100
//                        }
//                    }
//                }
//                else {
//                    if( *m_founded_frame & 0x000000F0 ) {
//                        if( *m_founded_frame & 0x000000C0 ) {
//                            if( *m_founded_frame & 0x00000080 ) return start_frame * 32 + 24;   //0x00000080
//                            return start_frame * 32 + 25;   //0x00000040
//                        }
//                        else {
//                            if( *m_founded_frame & 0x00000020 ) return start_frame * 32 + 26;   //0x00000020
//                            return start_frame * 32 + 27;   //0x00000010
//                        }
//                    }
//                    else {
//                        if( *m_founded_frame & 0x0000000C ) {
//                            if( *m_founded_frame & 0x00000008 ) return start_frame * 32 + 28;   //0x00000008
//                            return start_frame * 32 + 29;   //0x00000004
//                        }
//                        else {
//                            if( *m_founded_frame & 0x00000002 ) return start_frame * 32 + 30;   //0x00000002
//                            return start_frame * 32 + 31;   //0x00000001
//                        }
//                    }
//                }
//            }
//        }
//        return m_diapazone;
//    }
//
//    size_t Diapazone() const { return m_diapazone; }
//
//    void Show()
//    {
//        for( int i = 0; i < m_frames_num; ++i ) {
//            std::cout << std::bitset<sizeof( m_frames_map[i] ) * CHAR_BIT>( m_frames_map[i] ) << " ";
//        }
//        std::cout << std::endl;
//    }
//private:
//    size_t m_min_size;
//    size_t m_max_size;
//    size_t m_diapazone;
//    size_t m_frames_num;
//    uint32_t *m_frames_map;
//    uint32_t *m_founded_frame;
//};
//
//
//// Список для хранения SubSlist*-элементов (см. выше), отсортированных по .size, которые они представляют.
//template<class T>
//class DynamicSList : public BasicList<SubSList<T>*>
//{
//public:
//    using BasicList<SubSList<T>*>::m_size;
//    using BasicList<SubSList<T>*>::m_capacity;
//    using BasicList<SubSList<T>*>::m_space_L;
//    using BasicList<SubSList<T>*>::m_space_R;
//    using BasicList<SubSList<T>*>::m_list;
//    using BasicList<SubSList<T>*>::m_begin;
//    using BasicList<SubSList<T>*>::m_end;
//    using BasicList<SubSList<T>*>::ResetState;
//    using BasicList<SubSList<T>*>::ReorganizeList;
//    using BasicList<SubSList<T>*>::ReorganizeIfNecessary;
//
//    DynamicSList( size_t capacity ) : BasicList<SubSList<T>*>( capacity ) {}
//
//    // Находит индекс первого элемента, .size которого >= заданной (lower_bound)
//    size_t FindIndexBySize( size_t size );
//
//    // Находит индекс первого элемента, .size которого >= заданной (lower_bound). Если .size == заданной, устанавливает "hit" = true.
//    size_t FindIndexBySize( size_t size, bool &hit );
//
//    // Добавляет *подмассив SubSlist, когда список пуст (перед вставкой - проверить, что список реально пуст)
//    void InsertWhenEmpty( SubSList<T>* const &sub_list );
//
//    // Добавляет *подмассив SubSlist, когда в списке 1 элемент (перед вставкой - проверить, что в списке реально 1 элемент)
//    void InsertWhenSingle( SubSList<T>* const &sub_list );
//
//    // Добавляет *подмассив SubSlist в список по индексу (индекс вставки не должен нарушать сортировку)
//    void InsertByIndex( SubSList<T>* const &sub_list, size_t index );
//
//    // Удаляет и возвращает *подмассив SubSlist из списка, когда в списке 1 элемент (перед удалением - проверить, что в списке реально 1 элемент)
//    SubSList<T>* RemoveWhenSingle();
//
//    // Удаляет и возвращает *подмассив SubSlist из списка по индексу (перед удалением - убедиться, что индекс не выходит за пределы списка)
//    SubSList<T>* RemoveByIndex( size_t index );
//
//    void Show()
//    {
//        std::cout << "Print DynamicSList:" << std::endl;
//        for( size_t i = 0; i < m_capacity; ++i )
//        {
//            if( !m_list[i] ) {
//                std::cout << "[o]" << std::endl;
//            }
//            else {
//                for( size_t k = 0; k < m_list[i]->m_capacity; ++k )
//                {
//                    if( !m_list[i]->m_list[k] ) {
//                        std::cout << "[-]";
//                    }
//                    else
//                    {
//                        std::cout << utils::to_string( RegionP<T>{m_list[i]->m_list[k], m_list[i]->m_size_represented} );
//                    }
//                }
//                std::cout << std::endl;
//            }
//        }
//        std::cout << std::endl;
//    }
//};
//
//
//template<class T>
//class StaticSList : public BasicList<SubSList<T>>
//{
//public:
//    using BasicList<SubSList<T>>::m_size;
//    using BasicList<SubSList<T>>::m_capacity;
//    using BasicList<SubSList<T>>::m_space_L;
//    using BasicList<SubSList<T>>::m_space_R;
//    using BasicList<SubSList<T>>::m_list;
//    using BasicList<SubSList<T>>::m_begin;
//    using BasicList<SubSList<T>>::m_end;
//
//    StaticSList( size_t min_represent_size, size_t max_represent_size, size_t subslist_cap )
//        : BasicList<SubSList<T>>( max_represent_size - min_represent_size + 1 )
//        , m_min_represent_size( min_represent_size )
//        , m_max_represent_size( max_represent_size )
//        , m_sizes_map( min_represent_size, max_represent_size )
//    {
//        // BasicList устанавливает begin и end на середину списка, в данном случае это не подходит. Сдвигаем их в начало.
//        m_begin = m_end = m_list;
//        m_space_L = 0;
//        m_space_R = m_size;
//
//        // Набиваем список элементами SubSList<T>
//        for( size_t i = 0; i < m_capacity; ++i )
//        {
//            m_begin[i] = SubSList<T>( subslist_cap, min_represent_size + i );
//        }
//    }
//
//    // Находит и возвращает индекс первого не пустого SubSList<T> с индексом >= index. Если подходящий SubSList<T> не найден - возвращает m_capacity (это max index + 1)
//    size_t FindNotEmptySubSList( size_t index ) { return m_sizes_map.LowerBoundBit( index ); }
//
//    // Добавляет .start региона в SubSList с индексом index (ширина, которую SubSList представляет = m_min_represent_size + index)
//    void InsertElement( T *const &start, size_t index );
//
//    // Удаляет .start региона из SubSList с индексом index (ширина, которую SubSList представляет = m_min_represent_size + index)
//    void RemoveElement( T *const &start, size_t index );
//
//    // Удаляет и возвращает первый элемент (.start) из SubSList с индексом index. Если SubSList по этому индексу пуст - возвращает nullptr.
//    T* PopElement( size_t index );
//
//    void Show()
//    {
//        std::cout << "Print StaticSList:" << std::endl;
//        for( size_t i = 0; i < m_capacity; ++i )
//        {
//            m_begin[i].Show();
//        }
//    }
//
//    size_t m_min_represent_size;
//    size_t m_max_represent_size;
//    SizesBitMap m_sizes_map;
//};
//
//
//template<class T>
//void PList<T>::InsertElement( const RegionP<T> &ins, uint8_t &adj, RegionP<T> &adj_L, RegionP<T> &adj_R, RegionP<T> &rslt )
//{
//    // Если в списке 1 элемент
//    if( m_size == 1 )
//    {
//        // Если такой регион уже есть в списке - возвращаем ошибку
//        if( ins.start == m_begin->start ) {
//            return;
//        }
//        // Если попадаем правее (в конец)
//        if( ins.start > m_begin->start )
//        {
//            // Если регион перекрывается с левым - ошибка
//            if( m_begin->start + m_begin->size > ins.start ) {
//                return;
//            }
//            // Смежен ли левый сосед? Если да - фрагмент не добавляется, а модифицируется имеющийся.
//            if( ( m_begin->start + m_begin->size ) == ins.start ) {
//                adj = 1;
//                adj_L = *m_begin;
//                m_begin->size += ins.size;
//                rslt = *m_begin;
//                return;
//            }
//            // Добавляем новый регион правее. Место справа уменьшается, конец списка сдвигается вправо.
//            ReorganizeIfNecessary( m_space_R );
//            *m_end++ = ins;
//            --m_space_R;
//        }
//        // Если попадаем левее (в начало)
//        else
//        {
//            // Если регион перекрывается с правым - ошибка
//            if( ins.start + ins.size > m_begin->start ) {
//                return;
//            }
//            // Смежен ли правый сосед? Если да - фрагмент не добавляется, а модифицируется имеющийся.
//            if( m_begin->start == ins.start + ins.size ) {
//                adj = 2;
//                adj_R = *m_begin;
//                m_begin->start = ins.start;
//                m_begin->size += ins.size;
//                rslt = *m_begin;
//                return;
//            }
//            // Добавляем новый регион левее. Место слева уменьшается, начало списка сдвигается влево.
//            ReorganizeIfNecessary( m_space_L );
//            *( --m_begin ) = ins;
//            --m_space_L;
//        }
//        ++m_size;
//        adj = 0;
//    }
//    // Если в списке много элементов
//    else
//    {
//        size_t index = utils::lower_bound( m_begin, m_size, ins.start );
//        // Если вставка в начало
//        if( !index )
//        {
//            // Если регион перекрывается с правым - ошибка
//            if( ins.start + ins.size > m_begin->start ) {
//                return;
//            }
//            // Смежен ли правый сосед?
//            if( ins.start + ins.size == m_begin->start ) {
//                adj = 2;
//                adj_R = *m_begin;
//                m_begin->start = ins.start;
//                m_begin->size += ins.size;
//                rslt = *m_begin;
//            }
//            else {
//                ReorganizeIfNecessary( m_space_L );
//                *( --m_begin ) = ins;
//                --m_space_L;
//                ++m_size;
//                adj = 0;
//            }
//        }
//        // Если вставка в конец
//        else if( index == m_size )
//        {
//            RegionP<T> *lastReg = ( m_end - 1 );
//
//            // Если регион перекрывается с левым - ошибка
//            if( lastReg->start + lastReg->size > ins.start ) {
//                return;
//            }
//            // Смежен ли левый сосед?
//            if( lastReg->start + lastReg->size == ins.start ) {
//                adj = 1;
//                adj_L = *lastReg;
//                lastReg->size += ins.size;
//                rslt = *lastReg;
//            }
//            else {
//                ReorganizeIfNecessary( m_space_R );
//                *m_end++ = ins;
//                --m_space_R;
//                ++m_size;
//                adj = 0;
//            }
//        }
//        // Если вставка в середину
//        else {
//            RegionP<T> *R = m_begin + index;
//            RegionP<T> *L = R - 1;
//
//            // Если регион перекрывается с левым или правым - ошибка
//            if( ( L->start + L->size > ins.start ) || ( ins.start + ins.size > R->start ) ) {
//                return;
//            }
//            // Если левый и правый соседи - смежные
//            if( ( L->start + L->size == ins.start ) && ( ins.start + ins.size == R->start ) )
//            {
//                adj = 3;
//                adj_L = *L;
//                adj_R = *R;
//
//                // Если вставка ближе к началу
//                if( index < m_size / 2 )
//                {
//                    R->start = L->start;                    // Модифицируем правого соседа по start и size. Сдвигаем эл-ты, начиная с Begin, на 1 поз. вправо (перекрываем левого соседа)
//                    R->size += L->size + ins.size;
//                    rslt = *R;
//                    memmove( m_begin + 1, m_begin, ( index - 1 ) * sizeof( RegionP<T> ) );
//                    ++m_space_L;
//                    *m_begin++ = { 0 };
//                }
//                // Если вставка ближе к концу
//                else
//                {
//                    L->size += ins.size + R->size;          // Модифицируем левого соседа по size. Сдвигаем эл-ты, начиная с Правый+1, на 1 позицию влево (перекрываем правого соседа)
//                    rslt = *L;
//                    memmove( R, R + 1, ( m_size - index - 1 ) * sizeof( RegionP<T> ) );
//                    ++m_space_R;
//                    *( --m_end ) = { 0 };
//                }
//                --m_size;
//            }
//            // Если только правый сосед смежный - модифицируем правого соседа по start и size
//            else if( ins.start + ins.size == R->start )
//            {
//                adj = 2;
//                adj_R = *R;
//                R->start = ins.start;
//                R->size += ins.size;
//                rslt = *R;
//            }
//            // Если только левый сосед смежный - модифицируем левого соседа по size
//            else if( L->start + L->size == ins.start )
//            {
//                adj = 1;
//                adj_L = *L;
//                L->size += ins.size;
//                rslt = *L;
//            }
//            // Если нет смежных соседей
//            else
//            {
//                // Если вставка ближе к началу - сдвигаем эл-ты, начиная с Begin, на 1 поз. влево (освобождаем место для вставки), если надо - реорганизуем
//                if( index < m_size / 2 )
//                {
//                    if( m_space_L == 0 ) {
//                        ReorganizeList();
//                        R = m_begin + index;                // R и L теперь невалидны, переопределяем их.
//                        L = R - 1;
//                    }
//                    memmove( m_begin - 1, m_begin, ( index + 1 ) * sizeof( RegionP<T> ) );
//                    *L = ins;
//                    --m_space_L;
//                    --m_begin;
//                }
//                // Если вставка ближе к концу сдвигаем эл-ты, начиная с Правый+1 на 1 поз. вправо (освобождаем место для вставки), если надо - реорганизуем
//                else
//                {
//                    if( m_space_R == 0 ) {
//                        ReorganizeList();
//                        R = m_begin + index;                // R и L теперь невалидны, переопределяем только R, т.к. L дальше не нужен.
//                    }
//                    memmove( R + 1, R, ( m_size - index ) * sizeof( RegionP<T> ) );
//                    *R = ins;
//                    --m_space_R;
//                    ++m_end;
//                }
//                ++m_size;
//                adj = 0;
//            }
//        }
//    }
//}
//
//template<class T>
//void PList<T>::RemoveElement( const RegionP<T> &del )
//{
//    // Удаление из начала?
//    if( m_begin->start == del.start )
//    {
//        *m_begin++ = { 0 };
//        ++m_space_L;
//    }
//    // Удаление из конца?
//    else if( (m_end-1)->start == del.start )
//    {
//        *(--m_end) = { 0 };
//        ++m_space_R;
//    }
//    else
//    {
//        size_t index = utils::lower_bound( m_begin, m_size, del.start );
//
//        // Удаление ближе к концу?
//        if( index >= m_size / 2 )
//        {
//            memmove( m_begin + index, m_begin + index + 1, (m_size - index - 1) * sizeof(RegionP<T>));
//            *(--m_end) = { 0 };
//            ++m_space_R;
//        }
//        // Удаление ближе к началу?
//        else
//        {
//            memmove( m_begin + 1, m_begin, index * sizeof( RegionP<T> ));
//            *m_begin++ = { 0 };
//            ++m_space_L;
//        }
//    }
//    // Если список опустел - возвращаем begin/end в середину
//    if( --m_size == 0 )
//    {
//        ResetState();
//    }
//}
//
//template<class T>
//void PList<T>::NarrowDownElement( T *start, size_t size )
//{
//    // Модифицируем 1-й элемент?
//    if( m_begin->start == start )
//    {
//        m_begin->start += size;
//        m_begin->size -= size;
//    }
//    // Модифицируется последний элемент?
//    else if( ( m_end - 1 )->start == start )
//    {
//        ( m_end - 1 )->start += size;
//        ( m_end - 1 )->size -= size;
//    }
//    else
//    {
//        size_t index = utils::lower_bound( m_begin, m_size, start );
//        m_begin[index].start += size;
//        m_begin[index].size -= size;
//    }
//}
//
//template<class T>
//void SubSList<T>::InsertElement( T* const &start )
//{
//    // Если список пуст
//    if( m_size == 0 )
//    {
//        *m_end++ = start;
//        --m_space_R;
//        ++m_size;
//    }
//    // Если в списке 1 элемент
//    else if( m_size == 1 )
//    {
//        // Если попадаем правее (в конец) - реорганизуем, если надо. Место справа уменьшается, конец списка сдвигается вправо.
//        if( start > *m_begin )
//        {
//            ReorganizeIfNecessary( m_space_R );
//            *m_end++ = start;
//            --m_space_R;
//        }
//        // Если попадаем левее (в начало) - реорганизуем, если надо. Место слева уменьшается, начало списка сдвигается влево.
//        else
//        {
//            ReorganizeIfNecessary( m_space_L );
//            *( --m_begin ) = start;
//            --m_space_L;
//        }
//        ++m_size;
//    }
//    // Если в списке много элементов
//    else
//    {
//        size_t index = utils::lower_bound( m_begin, m_size, start );
//        // Если вставка в начало
//        if( !index )
//        {
//            ReorganizeIfNecessary( m_space_L );
//            *( --m_begin ) = start;
//            --m_space_L;
//            ++m_size;
//        }
//        // Если вставка в конец
//        else if( index == m_size )
//        {
//            ReorganizeIfNecessary( m_space_R );
//            *m_end++ = start;
//            --m_space_R;
//            ++m_size;
//        }
//        // Если вставка в середину
//        else {
//            T **R = m_begin + index;
//            T **L = R - 1;
//
//            // Если вставка ближе к началу - сдвигаем эл-ты, начиная с Begin, на 1 поз. влево (освобождаем место для вставки), если надо - реорганизуем
//            if( index < m_size / 2 )
//            {
//                if( m_space_L == 0 ) {
//                    ReorganizeList();
//                    R = m_begin + index;                // R и L теперь невалидны, переопределяем их.
//                    L = R - 1;
//                }
//                memmove( m_begin - 1, m_begin, ( index + 1 ) * sizeof( T * ) );
//                *L = start;
//                --m_space_L;
//                --m_begin;
//            }
//            // Если вставка ближе к концу сдвигаем эл-ты, начиная с Правый+1 на 1 поз. вправо (освобождаем место для вставки), если надо - реорганизуем
//            else
//            {
//                if( m_space_R == 0 ) {
//                    ReorganizeList();
//                    R = m_begin + index;                // R и L теперь невалидны, переопределяем только R, т.к. L дальше не нужен.
//                }
//                memmove( R + 1, R, ( m_size - index ) * sizeof( T * ) );
//                *R = start;
//                --m_space_R;
//                ++m_end;
//            }
//            ++m_size;
//        }
//    }
//}
//
//template<class T>
//void SubSList<T>::RemoveElement( T* const &start )
//{
//    // Удаление из начала?
//    if( *m_begin == start )
//    {
//        *m_begin++ = nullptr;
//        ++m_space_L;
//    }
//    // Удаление из конца?
//    else if( *(m_end - 1) == start )
//    {
//        *(--m_end) = nullptr;
//        ++m_space_R;
//    }
//    else
//    {
//        size_t index = utils::lower_bound( m_begin, m_size, start );
//
//        // Удаление ближе к концу?
//        if( index >= m_size / 2 )
//        {
//            memmove( m_begin + index, m_begin + index + 1, ( m_size - index - 1 ) * sizeof(T*));
//            *(--m_end) = nullptr;
//            ++m_space_R;
//        }
//        // Удаление ближе к началу?
//        else
//        {
//            memmove( m_begin + 1, m_begin, index * sizeof(T*));
//            *m_begin++ = nullptr;
//            ++m_space_L;
//        }
//    }
//    // Если список опустел - возвращаем begin/end в середину
//    if( --m_size == 0 )
//    {
//        ResetState();
//    }
//}
//
//template<class T>
//SubSListPull<T>::SubSListPull( size_t capacity ) : m_size( 0 ), m_capacity( capacity ), m_space_T( 0 ), m_pull( nullptr ), m_pull_top( nullptr )
//{
//    utils::Attempt_calloc<SubSList<T>*>( 20, 100, m_capacity, m_pull );
//    m_pull_top = m_pull;
//}
//
//template<class T>
//SubSList<T> *SubSListPull<T>::Take( size_t size_represented )
//{
//    if( m_pull_top == m_pull )
//    {
//        return new SubSList<T>( 5, size_represented );
//    }
//    auto test = *(m_pull_top - 1);
//    SubSList<T> *temp = *--m_pull_top;
//    if( !temp->m_capacity )
//    {
//        return new SubSList<T>( 5, size_represented );
//    }
//    *m_pull_top = { 0 };
//    temp->m_size_represented = size_represented;
//    return temp;
//}
//
//template<class T>
//void SubSListPull<T>::Put( SubSList<T> *obj )
//{
//    if( m_pull_top - m_pull == m_capacity )
//    {
//        m_capacity *= 2;
//        utils::Attempt_realloc<SubSList<T> *>( 20, 100, m_capacity * sizeof( SubSList<T> * ), m_pull );
//        m_pull_top = m_pull + m_capacity / 2;
//        memset( m_pull_top, 0, m_capacity / 2 * sizeof( SubSList<T> * ) );
//    }
//    *m_pull_top++ = obj;
//}
//
//template<class T>
//size_t DynamicSList<T>::FindIndexBySize( size_t size )
//{
//    size_t index = 0;
//    size_t h = m_size;
//    size_t mid;
//    while( index < h ) {
//        mid = ( index + h ) / 2;
//        if( size <= m_begin[mid]->m_size_represented ) {
//            h = mid;
//        }
//        else {
//            index = mid + 1;
//        }
//    }
//    return index;
//}
//
//template<class T>
//size_t DynamicSList<T>::FindIndexBySize( size_t size, bool &hit )
//{
//    size_t index = FindIndexBySize( size );
//    hit = ( index < m_size ) && ( m_begin[index]->m_size_represented == size );
//    return index;
//}
//
//template<class T>
//void DynamicSList<T>::InsertWhenEmpty( SubSList<T> *const &sub_list )
//{
//    *m_end++ = sub_list;
//    --m_space_R;
//    ++m_size;                                                                                                   // TESTED
//}
//
//template<class T>
//void DynamicSList<T>::InsertWhenSingle( SubSList<T>* const &sub_list )
//{
//    // Если попадаем правее (в конец) - реорганизуем, если надо. Место справа уменьшается, конец списка сдвигается вправо.
//    if( sub_list->m_size_represented > (*m_begin)->m_size_represented )
//    {
//        ReorganizeIfNecessary( m_space_R );                                                                     // TESTED
//        *m_end++ = sub_list;
//        --m_space_R;                                                                                            // TESTED
//    }
//    // Если попадаем левее (в начало) - реорганизуем, если надо. Место слева уменьшается, начало списка сдвигается влево.
//    else
//    {
//        ReorganizeIfNecessary( m_space_L );                                                                     // TESTED
//        *( --m_begin ) = sub_list;
//        --m_space_L;                                                                                            // TESTED
//    }
//    ++m_size;
//}
//
//template<class T>
//void DynamicSList<T>::InsertByIndex( SubSList<T> *const &sub_list, size_t index )
//{
//    // Если вставка в начало
//    if( !index )
//    {
//        ReorganizeIfNecessary( m_space_L );                                                                     // TESTED
//        *( --m_begin ) = sub_list;
//        --m_space_L;
//        ++m_size;                                                                                               // TESTED
//    }
//    // Если вставка в конец
//    else if( index == m_size )
//    {
//        ReorganizeIfNecessary( m_space_R );                                                                     // TESTED
//        *m_end++ = sub_list;
//        --m_space_R;
//        ++m_size;                                                                                               // TESTED
//    }
//    // Если вставка в середину
//    else {
//        SubSList<T> **R = m_begin + index;
//        SubSList<T> **L = R - 1;
//
//        // Если вставка ближе к началу - сдвигаем эл-ты, начиная с Begin, на 1 поз. влево (освобождаем место для вставки), если надо - реорганизуем
//        if( index < m_size / 2 )
//        {
//            if( m_space_L == 0 ) {
//                ReorganizeList();                                                                               // TESTED
//                R = m_begin + index;                // R и L теперь невалидны, переопределяем их.
//                L = R - 1;
//            }
//            memmove( m_begin - 1, m_begin, ( index + 1 ) * sizeof(SubSList<T>*));
//            *L = sub_list;
//            --m_space_L;
//            --m_begin;                                                                                          // TESTED
//        }
//        // Если вставка ближе к концу сдвигаем эл-ты, начиная с Правый+1 на 1 поз. вправо (освобождаем место для вставки), если надо - реорганизуем
//        else
//        {
//            if( m_space_R == 0 ) {
//                ReorganizeList();                                                                               // TESTED
//                R = m_begin + index;                // R и L теперь невалидны, переопределяем только R, т.к. L дальше не нужен.
//            }
//            memmove( R + 1, R, ( m_size - index ) * sizeof(SubSList<T>*));
//            *R = sub_list;
//            --m_space_R;
//            ++m_end;                                                                                            // TESTED
//        }
//        ++m_size;
//    }
//}
//
//template<class T>
//SubSList<T>* DynamicSList<T>::RemoveWhenSingle()
//{
//    SubSList<T> *ret = *m_begin;
//    *m_begin++ = { 0 };
//    --m_size;
//    ResetState();
//    return ret;                                                                                                 // TESTED
//}
//
//template<class T>
//SubSList<T>* DynamicSList<T>::RemoveByIndex( size_t index )
//{
//    SubSList<T> *ret = *(m_begin + index);
//
//    // Удаление из начала?
//    if( index == 0 ) {
//        *m_begin++ = { 0 };
//        ++m_space_L;                                                                                            // TESTED
//    }
//    // Удаление из конца?
//    else if( index == m_size - 1 ) {
//        *(--m_end) = { 0 };
//        ++m_space_R;                                                                                            // TESTED
//    }
//    // Удаление ближе к концу?
//    else if( index >= m_size / 2 ) {
//        memmove( m_begin + index, m_begin + index + 1, ( m_size - index - 1 ) * sizeof( SubSList<T>*));
//        *(--m_end) = { 0 };
//        ++m_space_R;                                                                                            // TESTED
//    }
//    // Удаление ближе к началу?
//    else {
//        memmove( m_begin + 1, m_begin, index * sizeof( SubSList<T>* ));
//        *m_begin++ = { 0 };
//        ++m_space_L;                                                                                            // TESTED
//    }
//    --m_size;
//    return ret;
//}
//
//template<class T>
//void StaticSList<T>::InsertElement( T *const &start, size_t index )
//{
//    if( index >= m_capacity ) {
//        return;
//    }
//    if( m_begin[index].m_size == 0 ) {
//        m_sizes_map.Set_Bit( index );
//    }
//    m_begin[index].InsertElement( start );
//    m_size++;
//}
//
//template<class T>
//void StaticSList<T>::RemoveElement( T *const &start, size_t index )
//{
//    if( index >= m_capacity || !m_begin[index].m_size ) {
//        return;
//    }
//    if( m_begin[index].m_size == 1 ) {
//        m_sizes_map.ClearBit( index );
//    }
//    m_begin[index].RemoveElement( start );
//    m_size--;
//}
//
//template<class T>
//T* StaticSList<T>::PopElement( size_t index )
//{
//    if( index >= m_capacity || !m_begin[index].m_size ) {
//        return nullptr;
//    }
//    if( m_begin[index].m_size == 1 ) {
//        m_sizes_map.ClearBit( index );
//    }
//    T *temp = m_begin[index].m_begin[0];
//    *( m_begin[index].m_begin++ ) = nullptr;
//    m_begin[index].m_space_L++;
//    m_begin[index].m_size--;
//    m_size--;
//    return temp;
//}
//
//
//template<class T>
//class RegionsTrackerStrict
//{
//public:
//    RegionsTrackerStrict( size_t min_reg_size, size_t max_reg_size, size_t p_capacity = g_minimum_capacity, size_t s_capacity = g_minimum_capacity, size_t ds_capacity = g_minimum_capacity );
//
//    RegionsTrackerStrict( size_t min_reg_size, size_t max_reg_size, size_t p_capacity, size_t s_capacity, size_t ds_capacity, RegionP<T> managed_reg );
//
//    ~RegionsTrackerStrict();
//
//    Error_BasePtr ReleaseRegion( const RegionP<T> &region );
//
//    void GrabRegion( size_t size, T **out );
//
//    inline Error_BasePtr ReorganizeIfNecessary( size_t field_size );                // Реорганизует P- или S-List, если field_size == 0
//
//    inline Error_BasePtr ReorganizeList();                                          // Оптимально перераспределяет контент P- или S-List или расширяет его, давая возможность вставки новых элементов
//
//    inline Error_BasePtr ExpandList();                                              // Удваивает m_capacity P- или S-List со сдвигом исходного контента в центр
//
//    inline Error_BasePtr ShiftContentLeft( size_t n );                              // Сдвигает контент P- или S-List на n позиций влево
//
//    inline Error_BasePtr ShiftContentRight( size_t n );                             // Сдвигает контент P- или S-List на n позиций вправо
//
//    inline FieldsState GetFieldState();
//
//    inline void DelFrom_List( size_t index );
//
//    size_t m_min_reg_size;
//    size_t m_max_reg_size;
//    bool m_initialized;
//
//    PList<T> m_plist;
//    StaticSList<T> m_ss_list;
//    DynamicSList<T> m_ds_list;
//    SubSListPull<T> m_ds_pull;
//
//    RegionP<T> m_temp_adj_L;
//    RegionP<T> m_temp_adj_R;
//    RegionP<T> m_temp_rslt;
//    uint8_t m_temp_adj;
//};
//
//
//template<class T>
//RegionsTrackerStrict<T>::RegionsTrackerStrict( size_t min_reg_size, size_t max_reg_size, size_t p_capacity, size_t s_capacity, size_t ds_capacity, RegionP<T> managed_reg )
//    : RegionsTrackerStrict( min_reg_size, max_reg_size, p_capacity, s_capacity, ds_capacity )
//{
//    Error_BasePtr err = ReleaseRegion( managed_reg );                                               TRACE_CUSTOM_THR_ERR( err, "RegionsList error during initial Release()" );
//}
//
//
//template<class T>
//RegionsTrackerStrict<T>::RegionsTrackerStrict( size_t min_reg_size, size_t max_reg_size, size_t p_capacity, size_t s_capacity, size_t ds_capacity )
//    : m_min_reg_size( min_reg_size )
//    , m_max_reg_size( max_reg_size )
//    , m_initialized( false )
//    , m_plist( p_capacity )
//    , m_ss_list( min_reg_size, max_reg_size, s_capacity )
//    , m_ds_list( ds_capacity )
//    , m_ds_pull( ds_capacity )
//{
//    for( size_t i = 0; i < m_ds_pull.m_capacity; ++i )
//    {
//        SubSList<T> *sl = ( SubSList<T> * )malloc( sizeof( SubSList<T> ) );
//        *sl = SubSList<T>( s_capacity, 0 );
//        m_ds_pull.Put( sl );
//    }
//}
//
//
//template<class T>
//RegionsTrackerStrict<T>::~RegionsTrackerStrict()
//{}
//
//
//template<class T>
//void RegionsTrackerStrict<T>::GrabRegion( size_t size, T **out )
//{
//    // Если список пуст или запрашиваемый регион по ширине выходит за пределы - ошибка
//    if( m_plist.m_size == 0 || size > m_max_reg_size || size < m_min_reg_size ) {
//        return;
//    }
//    //// Если в списке 1 элемент
//    //if( m_plist.m_size == 1 )
//    //{
//    //    // Если ширина запрашиваемого региона больше той что есть - ошибка
//    //    if( m_plist.m_begin[0].size < size ) {
//    //        return;
//    //    }
//    //    // Если ширина запрашиваемого региона совпадает с той что есть - удаляем имеющийся элемент из P- и S-List
//    //    else if( m_plist.m_begin[0].size == size )
//    //    {
//    //        // Чистим P-List и статический S-List
//    //        *out = m_plist.m_begin[0].start;
//    //        *m_plist.m_begin++ = {};
//    //        m_plist.m_space_L++;
//    //        m_plist.m_size--;
//    //        m_ss_list.RemoveElement( *out, size - m_min_reg_size );
//    //        return;
//    //    }
//    //    // Если ширина запрашиваемого региона меньше той что есть - модифицируем имеющийся в P- и S-List
//    //    else
//    //    {
//    //        size_t new_size = m_plist.m_begin[0].size - size;
//    //        T *new_start = m_plist.m_begin[0].start + size;
//
//    //        // Удаляем и добавляем новый .start в статический S-List или модифицируем .start в динамическом S-List - ERROR: остаточный регион в динамическом S-List может оказаться меньше максимального
//    //        if( m_ss_list.m_size != 0 )
//    //        {
//    //            *out = m_ss_list.PopElement( m_plist.m_begin[0].size - m_min_reg_size );
//    //            m_ss_list.InsertElement( new_start, new_size - m_min_reg_size );
//    //        }
//    //        else
//    //        {
//    //            *out = m_ds_list.m_begin[0]->m_begin[0];
//    //            m_ds_list.m_begin[0]->m_begin[0] = nullptr;
//    //            m_ds_list.m_begin[0]->m_begin++;
//    //            m_ds_list.m_begin[0]->m_space_L++;
//    //            m_ds_list.m_begin[0]->m_size--;
//
//    //            if( new_size > m_max_reg_size )
//    //            {
//    //                m_ds_list.m_begin[0]->m_size_represented = new_size;
//    //                m_ds_list.m_begin[0]->m_begin[0] = new_start;
//    //            }
//    //            else
//    //            {
//    //                m_ds_pull.Put( m_ds_list.RemoveByIndex( 0 ) );
//    //                m_ss_list.InsertElement( new_start, new_size );
//    //            }
//    //        }
//    //        // Модифицируем регион в P-List
//    //        m_plist.m_begin[0].start += size;
//    //        m_plist.m_begin[0].size -= size;
//    //    }
//    //}
//    // Если в списке много элементов
//    else
//    {
//        // Определяем, в статическом или динамическом S-List находится подходящий регион. Сначала проверяем статический.
//        size_t index = 0;
//        if( m_ss_list.m_size && ( index = m_ss_list.FindNotEmptySubSList( size - m_min_reg_size ) != m_ss_list.m_capacity ) )
//        {
//            *out = m_ss_list.PopElement( index );                                       // В любом случае удаляем найденный регион из S-List
//
//            // Если ширина найденного региона оказалась больше запрашиваемой - добавляем остаток найденного региона в S-List и модифицируем соотв. регион в P-List
//            if( index > ( size - m_min_reg_size ) )
//            {
//                m_ss_list.InsertElement( *out + size, index - ( size - m_min_reg_size ) );
//                m_plist.NarrowDownElement( *out, size );
//            }
//            // Если ширина найденного региона совпала с запрашиваемой - удаляем регион из P-List
//            else {
//                m_plist.RemoveElement( RegionP<T>{*out, size} );
//            }
//        }
//        // Если динамический S-List не пустой
//        else if( m_ds_list.m_size != 0 )
//        {
//            // В динамическом S-List регион в любом случае шире запрашиваемого, поэтому удаляем из него найденный регион и добавляем остаток от него, потом модифицируем соотв. регион в P-List
//            *out = m_ds_list.m_begin[0]->m_begin[0];
//            m_ds_list.m_begin[0]->m_begin[0] = nullptr;
//            m_ds_list.m_begin[0]->m_begin++;
//            m_ds_list.m_begin[0]->m_space_L++;
//            m_ds_list.m_begin[0]->m_size--;
//
//            size_t new_size = m_ds_list.m_begin[0]->m_size_represented - size;
//            T *new_start = *out + size;
//
//            if( m_ds_list.m_begin[0]->m_size == 0 )
//            {
//                if( new_size > m_max_reg_size )
//                {
//                    m_ds_list.m_begin[0]->m_size_represented = new_size;
//                    m_ds_list.m_begin[0]->InsertElement( new_start );
//                }
//                else
//                {
//                    m_ds_pull.Put( m_ds_list.RemoveByIndex( 0 ) );
//                    m_ss_list.InsertElement( new_start, new_size );
//                }
//            }
//            else
//            {
//                if( new_size > m_max_reg_size )
//                {
//                    auto sub_slist = m_ds_pull.Take( new_size );
//                    if( !sub_slist->m_capacity )
//                    {
//                        std::cout << "catcha!" << std::endl;
//                    }
//                    sub_slist->InsertElement( new_start );
//                    m_ds_list.InsertByIndex( sub_slist, 0 );
//                }
//                else {
//                    m_ss_list.InsertElement( new_start, new_size );
//                }
//            }
//            m_plist.NarrowDownElement( *out, size );
//        }
//        // Подходящий регион не найден
//        else {
//            return;
//        }
//    }
//}
//
//
//template<class T>
//Error_BasePtr RegionsTrackerStrict<T>::ReleaseRegion( const RegionP<T> &region )
//{
//    // Если списки пусты
//    if( m_plist.m_size == 0 )
//    {
//        // Если ширина региона не выходит за установленные рамки
//        if( region.size <= m_max_reg_size && region.size >= m_min_reg_size )
//        {
//            m_plist.InsertWhenEmpty( region );
//            m_ss_list.InsertElement( region.start, region.size - m_min_reg_size );
//        }
//        // Если ширина региона больше допустимой и это инициализирующая вставка
//        else if( region.size > m_max_reg_size && !m_initialized )
//        {
//            m_plist.InsertWhenEmpty( region );
//            m_ds_list.InsertWhenEmpty( m_ds_pull.Take( region.size ) );
//            (*m_ds_list.m_begin)->InsertElement( region.start );
//        }
//        // Если недопустимая ширина региона
//        else
//        {
//            return nullptr;
//        }
//        if( !m_initialized ) {
//            m_initialized = true;
//        }
//        return nullptr;
//    }
//    // Если в списах 1 и более элементов
//    else
//    {
//        // Если ширина добавляемого региона не выходит за установленные рамки
//        if( region.size <= m_max_reg_size && region.size >= m_min_reg_size )
//        {
//            m_plist.InsertElement( region, m_temp_adj, m_temp_adj_L, m_temp_adj_R, m_temp_rslt );
//
//            RegionP<T> to_del[2];
//            uint8_t delCount = 0;
//            switch( m_temp_adj ) {
//            case 0: delCount = 0; break;
//            case 1: delCount = 1; to_del[0] = m_temp_adj_L; break;
//            case 2: delCount = 1; to_del[0] = m_temp_adj_R; break;
//            case 3: delCount = 2; to_del[0] = m_temp_adj_L; to_del[1] = m_temp_adj_R; break;
//            }
//            
//            if( delCount == 0)
//            {
//                m_ss_list.InsertElement( region.start, region.size - m_min_reg_size );
//            }
//            else
//            {
//                for( uint8_t i = 0; i < delCount; ++i )
//                {
//                    if( to_del[i].size <= m_max_reg_size )
//                    {
//                        m_ss_list.RemoveElement( to_del[i].start, to_del[i].size - m_min_reg_size );
//                    }
//                    else
//                    {
//                        size_t index = m_ds_list.FindIndexBySize( to_del[i].size );
//                        auto sub_slist = m_ds_list.m_begin[index];
//                        sub_slist->RemoveElement( to_del[i].start );
//                        if( sub_slist->m_size == 0 )
//                        {
//                            m_ds_pull.Put( m_ds_list.RemoveByIndex( index ) );
//                        }
//                    }
//                }
//                if( m_temp_rslt.size <= m_max_reg_size )
//                {
//                    m_ss_list.InsertElement( m_temp_rslt.start, m_temp_rslt.size - m_min_reg_size );
//                }
//                else
//                {
//                    bool founded = false;
//                    size_t index = m_ds_list.FindIndexBySize( m_temp_rslt.size, founded );
//                    if( !founded )
//                    {
//                        auto sub_slist = m_ds_pull.Take( m_temp_rslt.size );
//                        sub_slist->InsertElement( m_temp_rslt.start );
//                        switch( m_ds_list.m_size ) {
//                            case 0: m_ds_list.InsertWhenEmpty( sub_slist ); break;
//                            case 1: m_ds_list.InsertWhenSingle( sub_slist ); break;
//                            default: m_ds_list.InsertByIndex( sub_slist, index ); break;
//                        }
//                    }
//                    else
//                    {
//                        auto sub_slist = m_ds_list.m_begin[index];
//                        sub_slist->InsertElement( m_temp_rslt.start );
//                    }
//                }
//            }
//        }
//        else
//        {
//            return nullptr;
//        }
//    }
//    return nullptr;
//}



template<class T>
class RegionsList
{
public:
    friend class rl_manip;

    RegionsList( size_t m_capacity = g_minimum_capacity );

    RegionsList( size_t m_capacity, RegionP<T> managed_reg );

    ~RegionsList();

    Error_BasePtr ReleaseRegion( const RegionP<T> &region );

    Error_BasePtr GrabRegion( size_t size, T **out );

//private:
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

    inline Error_BasePtr ReleaseInEmptyList( const RegionP<T> &region );            // Вставка, когда RegionsList пуст

    inline Error_BasePtr ReleaseInSingleSizedList( const RegionP<T> &region );      // Вставка, когда в RegionsList 1 элемент

    inline Error_BasePtr ReleaseIntoBeginning( const RegionP<T> &region, RegionS<T> &ins, RegionS<T> &del, uint8_t &del_cnt );      // Вставка в начало P-List, в котором более чем 1 элемент

    inline Error_BasePtr ReleaseIntoEnd( const RegionP<T> &region, RegionS<T> &ins, RegionS<T> &del, uint8_t &del_cnt );            // Вставка в конец P-List, в котором более чем 1 элемент

    inline Error_BasePtr ReleaseIntoMiddle( const RegionP<T> &region, size_t index, RegionS<T> &ins, RegionS<T> &del1, RegionS<T> &del2, uint8_t &del_cnt );     // Вставка в середину P-List, в котором более чем 1 элемент

    inline Error_BasePtr GrabFromSingleSizedList( size_t size, T **out );                                                           // Захват, когда в RegionsList 1 элемент

    inline Error_BasePtr GrabFromSList( size_t size, T **out, T **to_del_or_mod, bool &mode, T **new_start, size_t &new_size );     // Захватывает и ремувит подходящий по size регион из S-List

    inline Error_BasePtr GrabFromPList( T *to_del_or_mod, bool &mode, T *new_start, size_t new_size );                              // Ремувит или модифицирует заданный регион в P-List

    inline size_t FindIndexAndPrepareSListForInsertion( RegionS<T> &to_ins );                           // Находит индекс вставки в S-List региона заданной ширины. Подготавливает S-List или сам регион к вставке.

    inline Error_BasePtr FindIndexAndPrepareSListForDeletion( RegionS<T> &to_del, size_t &index );      // Находит индекс удаляемого региона. Подготавливает S-List к удалению региона.

    template<class ListType>
    inline Error_BasePtr ReorganizeIfNecessary( size_t field_size );                // Реорганизует P- или S-List, если field_size == 0

    template<class ListType>
    inline Error_BasePtr ReorganizeList();                                          // Оптимально перераспределяет контент P- или S-List или расширяет его, давая возможность вставки новых элементов

    template<class ListType>
    inline Error_BasePtr ExpandList();                                              // Удваивает m_capacity P- или S-List со сдвигом исходного контента в центр

    template<class ListType>
    inline Error_BasePtr ShiftContentLeft( size_t n );                              // Сдвигает контент P- или S-List на n позиций влево

    template<class ListType>
    inline Error_BasePtr ShiftContentRight( size_t n );                             // Сдвигает контент P- или S-List на n позиций вправо

    template<class ListType>
    inline void DelFrom_List( size_t index );                                       // Удаляет по индексу элемент из S- или P-List, в котором индекс первого элемента = 0

    inline Error_BasePtr InserTo_S_List( const RegionS<T>& ins, size_t index );     // Добавляет по индексу элемент в S-List, в котором индекс первого элемента = 0

    template<class ListType>
    inline FieldsState GetFieldState();                                             // Определяет состояние полей P- или S-List

    size_t ins_begin_R_Adj = 0;
    size_t ins_begin_NoAdj = 0;
    size_t ins_end_L_Adj = 0;
    size_t ins_end_NoAdj = 0;
    size_t ins_middle_RL_Adj = 0;
    size_t ins_middle_R_Adj = 0;
    size_t ins_middle_L_Adj = 0;
    size_t ins_middle_NoAdj = 0;
    size_t ins_in_single = 0;
    size_t ins_in_empty = 0;

    void ResetStat() { ins_begin_R_Adj = ins_begin_NoAdj = ins_end_L_Adj = ins_end_NoAdj = ins_middle_RL_Adj = ins_middle_R_Adj = ins_middle_L_Adj = ins_middle_NoAdj = ins_in_single = ins_in_empty = 0; }
    void ShowStat()
    {
        std::cout
            <<   "ins_begin_R_Adj:   " << ins_begin_R_Adj
            << "\nins_begin_NoAdj:   " << ins_begin_NoAdj
            << "\nins_end_L_Adj:     " << ins_end_L_Adj
            << "\nins_end_NoAdj:     " << ins_end_NoAdj
            << "\nins_middle_RL_Adj: " << ins_middle_RL_Adj
            << "\nins_middle_R_Adj:  " << ins_middle_R_Adj
            << "\nins_middle_L_Adj:  " << ins_middle_L_Adj
            << "\nins_middle_NoAdj:  " << ins_middle_NoAdj
            << "\nins_in_single:     " << ins_in_single
            << "\nins_in_empty:      " << ins_in_empty << std::endl;
    }
};


template<class T>
RegionsList<T>::RegionsList( size_t m_capacity, RegionP<T> managed_reg ) : RegionsList( m_capacity )
{
    Error_BasePtr err = ReleaseRegion( managed_reg );                                               TRACE_CUSTOM_THR_ERR( err, "RegionsList error during initial Release()" );
}


template<class T>
RegionsList<T>::RegionsList( size_t m_capacity )
    : m_p_list_size( 0 )
    , m_p_list_capacity( m_capacity )
    , m_p_list_spaceLeft( 0 )
    , m_p_list_spaceRight( 0 )
    , m_p_list( nullptr )
    , m_p_list_begin( nullptr )
    , m_p_list_end( nullptr )
    , m_s_list_size( 0 )
    , m_s_list_capacity( m_capacity )
    , m_s_list_spaceLeft( 0 )
    , m_s_list_spaceRight( 0 )
    , m_s_list( nullptr )
    , m_s_list_begin( nullptr )
    , m_s_list_end( nullptr )
{
    // Выделяем память для списков фрагментов. Минимум - для четырёх. Инициализируем
    if (m_capacity < g_minimum_capacity) {
        m_p_list_capacity = m_s_list_capacity = g_minimum_capacity;
    }
    Error_BasePtr err = nullptr;
    err = utils::Attempt_calloc<RegionP<T>>( 20, 100, m_p_list_capacity, m_p_list );                TRACE_REGIONSLIST_THR_ERR( err, ERL_Type::P_LIST_ALLOCATION );
    err = utils::Attempt_calloc<RegionS<T>>( 20, 100, m_s_list_capacity, m_s_list );                TRACE_REGIONSLIST_THR_ERR( err, ERL_Type::S_LIST_ALLOCATION );

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
    if (m_p_list) { free( m_p_list ); }
    if (m_s_list) { free( m_s_list ); }
}


template<class T>
Error_BasePtr RegionsList<T>::ReleaseRegion( const RegionP<T>& region )
{
    // Если списки пусты
    if (!m_p_list_size)
    {
        ins_in_empty++;
        return ReleaseInEmptyList( region );
    }
    // Если в списках только один фрагмент
    if (m_p_list_size == 1)
    {
        ins_in_single++;
        return ReleaseInSingleSizedList( region );
    }
    // Если фрагментов много
    else {
        RegionS<T> to_ins = { 0 };
        RegionS<T> to_del[2];
        uint8_t del_cnt = 0;

        // Определяем индекс вставки в P-List
        bool founded;
        size_t index = utils::lower_bound( m_p_list_begin, m_p_list_size, region.start, founded );

        // Если такой регион уже есть в P-List - возвращаем ошибку
        if (founded) {
            return ERR_REGIONSLIST( ERL_Type::EXISTING_REG_INSERTION, region.start, region.size );
        }

        // Если вставка в начало P-List
        if (index == 0)
        {
            auto err = ReleaseIntoBeginning( region, to_ins, to_del[0], del_cnt );                  TRACE_CUSTOM_RET_ERR( err, "Can't insert into beginning of P-List)" );
        }
        // Если вставка в конец P-List
        else if (index == m_p_list_size)
        {
            auto err = ReleaseIntoEnd( region, to_ins, to_del[0], del_cnt );                        TRACE_CUSTOM_RET_ERR( err, "Can't insert into the m_end of P-List)" );
        }
        // Если вставка в середину P-List
        else {
            auto err = ReleaseIntoMiddle( region, index, to_ins, to_del[0], to_del[1], del_cnt );   TRACE_CUSTOM_RET_ERR( err, "Can't insert into the middle of P-List)" );
        }
        // Удаление элементов из S-List
        for (uint8_t i = 0; i < del_cnt; ++i)
        {
            auto err = FindIndexAndPrepareSListForDeletion( to_del[i], index );                     TRACE_CUSTOM_RET_ERR( err, "Can't delete region from S-List" );
            DelFrom_List<RegionS<T>>( index );
        }
        // Добавление элемента в S-List
        index = FindIndexAndPrepareSListForInsertion( to_ins );
        auto err = InserTo_S_List( to_ins, index );     TRACE_CUSTOM_RET_ERR( err, "Can't insert region into S-List: " + utils::to_string( to_ins ) + "\nInsertion index: " + std::to_string( index ) );
    }
    return nullptr;
}


template<class T>
Error_BasePtr RegionsList<T>::GrabFromSList( size_t size, T **out, T **to_del_or_mod, bool &mode, T **new_start, size_t &new_size )
{
    // Ищем регион подходящей ширины
    size_t index = utils::lower_bound( m_s_list_begin, m_s_list_size, size );

    // Если подходящий регион не найден - ошибка
    if( index == m_s_list_size )
    {
        return ERR_REGIONSLIST( ERL_Type::CONSISTENT_REG_NOTFOUND, size );
    }

    // По умолчанию найденный регион будет удалён из P-List (по значению .start найденного в S-List региона)
    RegionS<T> founded_reg = *( m_s_list_begin + index );
    *out = founded_reg.start;
    *to_del_or_mod = founded_reg.start;

    // Если найденный регион по ширине больше запрашиваемого - регион в P-List будет модифицирован. В S-List он заменяется новым (см. if( mode )... )
    if( founded_reg.size > size )
    {
        mode = true;
        *new_start = *to_del_or_mod + size;
        new_size = founded_reg.size - size;
    }
    // Удаляем найденный регион из S-List
    if( founded_reg.count != 1 )
    {
        ( m_s_list_begin + index + 1 )->count = founded_reg.count - 1;
    }
    DelFrom_List<RegionS<T>>( index );

    // Инсертим в S-List новый регион, с изменёнными .start и .size
    if( mode )
    {
        RegionS<T> to_ins = { *new_start, new_size, 0 };
        index = FindIndexAndPrepareSListForInsertion( to_ins );
        auto err = InserTo_S_List( to_ins, index );     TRACE_CUSTOM_RET_ERR( err, "Can't insert region into S-List: " + utils::to_string( to_ins ) + "\nInsertion index: " + std::to_string( index ) );
    }
    return nullptr;
}


template<class T>
Error_BasePtr RegionsList<T>::GrabFromPList( T *to_del_or_mod, bool &mode, T *new_start, size_t new_size )
{
    bool founded;
    size_t index = utils::lower_bound( m_p_list_begin, m_p_list_size, to_del_or_mod, founded );

    // Если элемент с таким .start в P-List не найден - ошибка
    if( !founded ) {
        return ERR_REGIONSLIST( ERL_Type::REG_WITH_SUCH_START_NOTFOUND, to_del_or_mod );
    }
    // Найденный регион в P-List модифицируется?
    if( mode ) {
        ( m_p_list_begin + index )->start = new_start;
        ( m_p_list_begin + index )->size = new_size;
        return nullptr;
    }
    DelFrom_List<RegionP<T>>( index );
    return nullptr;
}


template<class T>
size_t RegionsList<T>::FindIndexAndPrepareSListForInsertion( RegionS<T> &to_ins )
{
    size_t index = utils::lower_bound( m_s_list_begin, m_s_list_size, to_ins.size );

    // Элемент с таким size уже есть в списке?
    if( index != m_s_list_size && ( m_s_list_begin + index )->size == to_ins.size )
    {
        // Элемент с таким size пока единственный в списке?
        if( ( m_s_list_begin + index )->count == 1 )
        {
            // Вставка правее?
            if( to_ins.start > ( m_s_list_begin + index )->start ) {
                ( m_s_list_begin + index )->count = 2;
                index += 1;
            }
            else {
                ( m_s_list_begin + index )->count = 0;
                to_ins.count = 2;
            }
        }
        else {
            size_t subindex = utils::lower_bound( m_s_list_begin + index, ( m_s_list_begin + index )->count, to_ins.start );

            // Вставка правее?
            if( subindex ) {
                ( m_s_list_begin + index )->count++;
                index += subindex;
            }
            else {
                to_ins.count = ( m_s_list_begin + index )->count + 1;
                ( m_s_list_begin + index )->count = 0;
            }
        }
    }
    else {
        to_ins.count = 1;
    }
    return index;
}


template<class T>
Error_BasePtr RegionsList<T>::FindIndexAndPrepareSListForDeletion( RegionS<T> &to_del, size_t &index )
{
    // Определяем индекс удаляемого элемента
    bool founded;
    index = utils::lower_bound( m_s_list_begin, m_s_list_size, to_del.size, founded );

    // Если такой элемент не найден - ошибка
    if( !founded )
    {
        return ERR_REGIONSLIST( ERL_Type::SUCH_REGION_NOTFOUND, to_del.start, to_del.size );
    }
    // Элемент с таким size не единственный?
    if( ( m_s_list_begin + index )->count != 1 )
    {
        size_t subIndex = utils::lower_bound( m_s_list_begin + index, ( m_s_list_begin + index )->count, to_del.start );

        // Удаление правее?
        if( subIndex ) {
            ( m_s_list_begin + index )->count--;
            index += subIndex;
        }
        else {
            ( m_s_list_begin + index + 1 )->count = ( m_s_list_begin + index )->count - 1;
        }
    }
    return nullptr;
}


template<class T>
Error_BasePtr RegionsList<T>::GrabRegion( size_t size, T **out )
{
    if( !size )
    {
        return ERR_PARAM( 1, "size_t size", "Non zero", "0" );
    }
    // Если S-List пустой - ошибка
    if (!m_s_list_size)
    {
        return ERR_REGIONSLIST( ERL_Type::GRAB_FROM_EMPTY_LIST );
    }
    // Если в списках 1 регион
    else if (m_s_list_size == 1)
    {
        return GrabFromSingleSizedList( size, out );
    }
    // Если в списках много регионов
    else {
        T* toDel_or_Modify = nullptr;   // RegionP.start для удаления или модификации в P-List
        T* newStart = nullptr;          // Новый .start
        size_t newSize;                 // Новая .size
        bool mode = false;              // true = Modify, false = Delete (по умолчанию)

        auto err = GrabFromSList( size, out, &toDel_or_Modify, mode, &newStart, newSize );
        if( err ) {
            return err;
        }
        return GrabFromPList( toDel_or_Modify, mode, newStart, newSize );
    }
}


template<class T>
Error_BasePtr RegionsList<T>::ReleaseInEmptyList( const RegionP<T> &region )
{
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


template<class T>
Error_BasePtr RegionsList<T>::ReleaseInSingleSizedList( const RegionP<T> &region )
{
    // Если такой регион уже есть в списке - возвращаем ошибку
    if( region.start == m_p_list_begin->start ) {
        return ERR_REGIONSLIST( ERL_Type::EXISTING_REG_INSERTION, region.start, region.size );
    }
    // Если попадаем правее (в конец)
    if( region.start > m_p_list_begin->start )
    {
        // Если регион перекрывается с левым - ошибка
        if( m_p_list_begin->start + m_p_list_begin->size > region.start ) {
            return ERR_REGIONSLIST( ERL_Type::OVERLAPPED_REG_INSERTION, region.start, region.size );
        }
        // Смежен ли левый сосед? Если да - фрагмент не добавляется, а модифицируется имеющийся (в обоих списках).
        if( ( m_p_list_begin->start + m_p_list_begin->size ) == region.start ) {
            m_p_list_begin->size += region.size;
            m_s_list_begin->size = m_p_list_begin->size;
            return nullptr;
        }
        // Добавляем новый регион правее. Место справа уменьшается, конец списка сдвигается вправо.
        auto err = ReorganizeIfNecessary<RegionP<T>>( m_p_list_spaceRight );                        TRACE_CUSTOM_RET_ERR( err, "Can't reorganize P-List (contains 1 element, insertion to the m_end, R is empty)." );
        *m_p_list_end++ = region;
        ++m_p_list_size;
        --m_p_list_spaceRight;
    }
    // Если попадаем левее (в начало)
    else
    {
        // Если регион перекрывается с правым - ошибка
        if( region.start + region.size > m_p_list_begin->start ) {
            return ERR_REGIONSLIST( ERL_Type::OVERLAPPED_REG_INSERTION, region.start, region.size );
        }
        // Смежен ли правый сосед? Если да - фрагмент не добавляется, а модифицируется имеющийся (в обоих списках).
        if( m_p_list_begin->start == region.start + region.size ) {
            m_p_list_begin->start = region.start;
            m_p_list_begin->size += region.size;
            *m_s_list_begin = { m_p_list_begin->start, m_p_list_begin->size, 1 };
            return nullptr;
        }
        // Добавляем новый регион левее. Место слева уменьшается, начало списка сдвигается влево.
        auto err = ReorganizeIfNecessary<RegionP<T>>( m_p_list_spaceLeft );                         TRACE_CUSTOM_RET_ERR( err, "Can't reorganize P-List (contains 1 element, insertion to m_begin, L is empty)." );
        *( --m_p_list_begin ) = region;
        ++m_p_list_size;
        --m_p_list_spaceLeft;
    }
    // Если попадаем правее
    if( region.size > m_s_list_begin->size )
    {
        auto err = ReorganizeIfNecessary<RegionS<T>>( m_s_list_spaceRight );                        TRACE_CUSTOM_RET_ERR( err, "Can't reorganize S-List (contains 1 element, insertion by size to the m_end, R is empty)." );
        *m_s_list_end++ = { region.start, region.size, 1 };
        --m_s_list_spaceRight;
    }
    // Если попадаем левее
    else if( region.size < m_s_list_begin->size )
    {
        auto err = ReorganizeIfNecessary<RegionS<T>>( m_s_list_spaceLeft );                         TRACE_CUSTOM_RET_ERR( err, "Can't reorganize S-List (contains 1 element, insertion by size to m_begin, L is empty)." );
        *( --m_s_list_begin ) = { region.start, region.size, 1 };
        --m_s_list_spaceLeft;
    }
    // Если ширина добавляемого фрагмента совпадает с шириной уже имеющегося - сортируем их по указателям.
    else {
        if( region.start > m_s_list_begin->start )
        {
            auto err = ReorganizeIfNecessary<RegionS<T>>( m_s_list_spaceRight );                    TRACE_CUSTOM_RET_ERR( err, "Can't reorganize S-List (contains 1 element, insertion by start to the m_end, R is empty)." );
            m_s_list_begin->count = 2;
            *m_s_list_end++ = { region.start, region.size, 0 };
            --m_s_list_spaceRight;
        }
        else {
            auto err = ReorganizeIfNecessary<RegionS<T>>( m_s_list_spaceLeft );                     TRACE_CUSTOM_RET_ERR( err, "Can't reorganize S-List (contains 1 element, insertion by start to m_begin, L is empty)." );
            m_s_list_begin->count = 0;
            *( --m_s_list_begin ) = { region.start, region.size, 2 };
            --m_s_list_spaceLeft;
        }
    }
    ++m_s_list_size;
    return nullptr;
}


template<class T>
Error_BasePtr RegionsList<T>::ReleaseIntoBeginning( const RegionP<T> &region, RegionS<T> &ins, RegionS<T> &del, uint8_t &del_cnt )
{
    // Если регион перекрывается с правым - ошибка
    if( region.start + region.size > m_p_list_begin->start ) {
        return ERR_REGIONSLIST( ERL_Type::OVERLAPPED_REG_INSERTION, region.start, region.size );
    }
    // Смежен ли правый сосед?
    if( region.start + region.size == m_p_list_begin->start ) {
        ins_begin_R_Adj++;
        del_cnt = 1;
        del = { m_p_list_begin->start, m_p_list_begin->size, 0 };
        m_p_list_begin->start = region.start;
        m_p_list_begin->size += region.size;
        ins = { m_p_list_begin->start, m_p_list_begin->size, 0 };
    }
    else {
        ins_begin_NoAdj++;
        auto err = ReorganizeIfNecessary<RegionP<T>>( m_p_list_spaceLeft );                         TRACE_CUSTOM_RET_ERR( err, "Can't reorganize P-List (insertion to beginning, L is empty)." );
        *( --m_p_list_begin ) = region;
        --m_p_list_spaceLeft;
        ++m_p_list_size;
        ins = { region.start, region.size, 0 };
        del_cnt = 0;
    }
    return nullptr;
}


template<class T>
Error_BasePtr RegionsList<T>::ReleaseIntoEnd( const RegionP<T> &region, RegionS<T> &ins, RegionS<T> &del, uint8_t &del_cnt )
{
    RegionP<T> *lastReg = ( m_p_list_end - 1 );

    // Если регион перекрывается с левым - ошибка
    if( lastReg->start + lastReg->size > region.start ) {
        return ERR_REGIONSLIST( ERL_Type::OVERLAPPED_REG_INSERTION, region.start, region.size );
    }
    // Смежен ли левый сосед?
    if( ( lastReg->start + lastReg->size ) == region.start )
    {
        ins_end_L_Adj++;
        del_cnt = 1;
        del = { lastReg->start, lastReg->size, 0 };
        lastReg->size += region.size;
        ins = { lastReg->start, lastReg->size, 0 };
    }
    else
    {
        ins_end_NoAdj++;
        auto err = ReorganizeIfNecessary<RegionP<T>>( m_p_list_spaceRight );                        TRACE_CUSTOM_RET_ERR( err, "Can't reorganize P-List (insertion to the m_end, R is empty)." );
        *m_p_list_end++ = region;
        --m_p_list_spaceRight;
        ++m_p_list_size;
        ins = { region.start, region.size, 0 };
        del_cnt = 0;
    }
    return nullptr;
}


template<class T>
Error_BasePtr RegionsList<T>::ReleaseIntoMiddle( const RegionP<T> &region, size_t index, RegionS<T> &ins, RegionS<T> &del1, RegionS<T> &del2, uint8_t &del_cnt )
{
    RegionP<T> *right = m_p_list_begin + index;
    RegionP<T> *left = right - 1;

    // Если регион перекрывается с левым или правым - ошибка
    if( ( ( left->start + left->size ) > region.start ) || ( ( region.start + region.size ) > right->start ) ) {
        return ERR_REGIONSLIST( ERL_Type::OVERLAPPED_REG_INSERTION, region.start, region.size );
    }

    // Если левый и правый соседи - смежные
    if( ( ( left->start + left->size ) == region.start ) && ( ( region.start + region.size ) == right->start ) )
    {
        ins_middle_RL_Adj++;
        del_cnt = 2;
        del1 = { left->start, left->size, 0 };
        del2 = { right->start, right->size, 0 };

        // Индекс вставки ближе к концу списка?
        if( index >= m_p_list_size / 2 ) {
            left->size += region.size + right->size;        // Модифицируем левого соседа по size
            ins = { left->start, left->size, 0 };
            memmove( right, right + 1, ( m_p_list_size - index - 1 ) * sizeof( RegionP<T> ) );    // Сдвигаем эл-ты, начиная с Правый+1, на 1 позицию влево (перекрываем правого соседа)
            ++m_p_list_spaceRight;
            *( --m_p_list_end ) = { 0 };
        }
        else {
            right->start = left->start;                     // Модифицируем правого соседа по start и size
            right->size += left->size + region.size;
            ins = { right->start, right->size, 0 };
            memmove( m_p_list_begin + 1, m_p_list_begin, ( index - 1 ) * sizeof( RegionP<T> ) );  // Сдвигаем эл-ты, начиная с Begin, на 1 поз. вправо (перекрываем левого соседа)
            ++m_p_list_spaceLeft;
            *m_p_list_begin++ = { 0 };
        }
        --m_p_list_size;
    }
    // Если только правый сосед - смежный
    else if( ( region.start + region.size ) == right->start )
    {
        ins_middle_R_Adj++;
        del_cnt = 1;
        del1 = { right->start, right->size, 0 };
        right->start = region.start;                // Модифицируем правого соседа по start и size
        right->size += region.size;
        ins = { right->start, right->size, 0 };
    }
    // Если только левый сосед - смежный
    else if( ( left->start + left->size ) == region.start )
    {
        ins_middle_L_Adj++;
        del_cnt = 1;
        del1 = { left->start, left->size, 0 };
        left->size += region.size;                  // Модифицируем левого соседа по size
        ins = { left->start, left->size, 0 };
    }
    // Если нет смежных соседей
    else
    {
        ins_middle_NoAdj++;
        // Индекс вставки ближе к концу списка?
        if( index >= m_p_list_size / 2 )
        {
            // Места справа не хватает для вставки 1 элемента?
            if( m_p_list_spaceRight == 0 ) {
                auto err = ReorganizeList<RegionP<T>>();                                            TRACE_CUSTOM_RET_ERR( err, "Can't reorganize P-List (closer to the m_end insertion, R is empty)." );
                right = m_p_list_begin + index;     // right после реаллокации списка невалиден. Переопределяем его.
            }
            memmove( right + 1, right, ( m_p_list_size - index ) * sizeof( RegionP<T> ) );        // Сдвигаем эл-ты, начиная с Правый+1 на 1 поз. вправо (освобождаем место для вставки)
            *right = region;
            --m_p_list_spaceRight;
            ++m_p_list_end;
            ins = { region.start, region.size, 0 };
        }
        else {
            // Места слева не хватает для вставки 1 элемента?
            if( m_p_list_spaceLeft == 0 ) {
                auto err = ReorganizeList<RegionP<T>>();                                            TRACE_CUSTOM_RET_ERR( err, "Can't reorganize P-List (closer to m_begin insertion, L is empty)." );
                right = m_p_list_begin + index;     // right и left после реаллокации списка невалидны. Переопределяем их.
                left = right - 1;
            }
            memmove( m_p_list_begin - 1, m_p_list_begin, ( index + 1 ) * sizeof( RegionP<T> ) );  // Сдвигаем эл-ты, начиная с Begin, на 1 поз. влево (освобождаем место для вставки)
            *left = region;
            --m_p_list_spaceLeft;
            --m_p_list_begin;
            ins = { region.start, region.size, 0 };
        }
        ++m_p_list_size;
        del_cnt = 0;
    }
    return nullptr;
}


template<class T>
Error_BasePtr RegionsList<T>::GrabFromSingleSizedList( size_t size, T **out )
{
    // Если ширина запрашиваемого региона больше той, что есть - ошибка
    if( m_s_list_begin->size < size ) {
        return ERR_REGIONSLIST( ERL_Type::CONSISTENT_REG_NOTFOUND, size );
    }
    // Если ширина запрашиваемого региона меньше той, что есть - модифицируем регионы в P- и S-List
    if( m_s_list_begin->size > size ) {
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


template<class T>
template<class ListType>
Error_BasePtr RegionsList<T>::ReorganizeIfNecessary( size_t field_size )
{
    if( !field_size ) {
        return ReorganizeList<ListType>();
    }
    return nullptr;
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

        auto err = utils::Attempt_realloc<ListType>( 20, 100, m_p_list_capacity * sizeof( ListType ), m_p_list );               TRACE_REGIONSLIST_RET_ERR( err, ERL_Type::P_LIST_ALLOCATION );

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

        auto err = utils::Attempt_realloc<ListType>( 20, 100, m_s_list_capacity * sizeof( ListType ), m_s_list );               TRACE_REGIONSLIST_RET_ERR( err, ERL_Type::S_LIST_ALLOCATION );

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
Error_BasePtr RegionsList<T>::InserTo_S_List( const RegionS<T>& ins, size_t index )
{
    // Вставка в начало?
    if (index == 0) {
        auto err = ReorganizeIfNecessary<RegionS<T>>( m_s_list_spaceLeft );                         TRACE_CUSTOM_RET_ERR( err, "Can't reorganize S-List (insertion at the beginning, L is empty)." );
        *(--m_s_list_begin) = ins;
        --m_s_list_spaceLeft;
    }
    // Вставка в конец?
    else if (index == m_s_list_size) {
        auto err = ReorganizeIfNecessary<RegionS<T>>( m_s_list_spaceRight );                        TRACE_CUSTOM_RET_ERR( err, "Can't reorganize S-List (insertion at the m_end, R is empty)." );
        *m_s_list_end++ = ins;
        --m_s_list_spaceRight;
    }
    // Вставка ближе к концу?
    else if (index >= m_s_list_size / 2) {
        auto err = ReorganizeIfNecessary<RegionS<T>>( m_s_list_spaceRight );                        TRACE_CUSTOM_RET_ERR( err, "Can't reorganize S-List (closer to m_end insertion, R is empty)." );
        memmove( m_s_list_begin + index + 1, m_s_list_begin + index, (m_s_list_size - index) * sizeof( RegionS<T> ) );
        *(m_s_list_begin + index) = ins;
        ++m_s_list_end;
        --m_s_list_spaceRight;
    }
    // Вставка ближе к началу?
    else {
        auto err = ReorganizeIfNecessary<RegionS<T>>( m_s_list_spaceLeft );                         TRACE_CUSTOM_RET_ERR( err, "Can't reorganize S-List (closer to m_begin insertion, L is empty)." );
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
        return FieldsState::R_AND_L_EMPTY;                  // Если оба поля пусты
    }
    else if( !space_R ) {
        if( space_L > size / 2 )
            return FieldsState::R_EMPTY_L_QUITE_WIDE;       // Если правое поле пусто и в левом достаточно места
        return FieldsState::R_EMPTY_L_TOO_SMALL;            // Если правое поле пусто и в левом слишком мало места
    }
    else if( !space_L ) {
        if( space_R > size / 2 )
            return FieldsState::L_EMPTY_R_QUITE_WIDE;       // Если левое поле пусто и в правом достаточно места
        return FieldsState::L_EMPTY_R_TOO_SMALL;            // Если левое поле пусто и в правом слишком мало места
    }
    else if( space_L && space_R ) {
        return FieldsState::R_AND_L_NOT_EMPTY;              // Если оба поля P-List не пусты
    }
    else {
        return FieldsState::NONE;
    }
}