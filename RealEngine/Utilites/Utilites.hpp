#pragma once


#include "Types.h"
#include "Error_Custom.h"
#include "Error_Param.h"
#include "Error_MemAlloc.h"
#include "LogWarning.h"

#include <stdint.h>
#include <string>
#include <vector>
#include <array>
#include <list>
#include <map>
#include <iostream>
#include <sstream>
#include <limits>
#include <random>


namespace utils
{
/*      Находит   индекс   первого   элемента  массива,  >= (lower_bound) или < (upper_bound) искомого  значения.  Если  такой  элемент
|   не найден - возвращает длину массива. Массив должен быть отсортирован по типу искомого значения.
|
|   Пример для lower_bound: arr = [1 1 1 4 4 6 7 8 8 8 9], value = 4, return = 3
|   Пример для lower_bound: arr = [1 1 1 4 4 6 7 8 8 8 9], value = 5, return = 5
|   Пример для upper_bound: arr = [1 1 1 4 4 6 7 8 8 8 9], value = 4, return = 5
|   Пример для upper_bound: arr = [1 1 1 4 4 6 7 8 8 8 9], value = 5, return = 5
*/
template<class T>
inline size_t lower_bound( RegionP<T> *arr, size_t size, const T* value, bool& founded );  // lower_bound по RegionP.start, с подтверждением

template<class T>
inline size_t lower_bound( RegionP<T> *arr, size_t size, const T* value );                 // lower_bound по RegionP.start

template<class T>
inline size_t lower_bound( RegionS<T> *arr, size_t size, size_t value, bool& founded );    // lower_bound по RegionS.size, с подтверждением

template<class T>
inline size_t lower_bound( RegionS<T> *arr, size_t size, size_t value );                   // lower_bound по RegionS.size

template<class T>
inline size_t lower_bound( RegionS<T> *arr, size_t size, const T* value );                 // lower_bound по RegionS.start

size_t lower_bound( int* arr, size_t size, int value, bool& founded );                     // lower_bound по int, с подтверждением

size_t lower_bound( int* arr, size_t size, int value );                                    // lower_bound по int

template<class T>
inline size_t upper_bound( RegionP<T> *arr, size_t size, T* value, bool& founded );        // upper_bound по RegionP.start, с подтверждением

template<class T>
inline size_t upper_bound( RegionP<T> *arr, size_t size, T* value );                       // upper_bound по RegionP.start

template<class T>
inline size_t upper_bound( RegionP<T> *arr, size_t size, size_t value, bool& founded );    // upper_bound по RegionP.size, с подтверждением

template<class T>
inline size_t upper_bound( RegionP<T> *arr, size_t size, size_t value );                   // upper_bound по RegionP.size

size_t upper_bound( int *arr, size_t size, int value, bool& founded );                     // upper_bound по int, с подтверждением

size_t upper_bound( int *arr, size_t size, int value );                                    // upper_bound по int

template<class T>
inline RegionS<T> RegionPtoS( const RegionP<T> &regP, size_t count = 0 );                  // Строит RegionS на основе RegionP, просто добавляя отсутствующее поле "count" = 0

template<class T>
inline std::vector<RegionS<T>> SListFromPList( const std::vector<RegionP<T>> &in );                // Строит S-List на основе P-List (пересортировкой сначала по size, затем по start)

template<class T>
inline bool CheckAdj( const RegionP<T>& left, const RegionP<T>& right );                           // Проверяет, смежны ли регионы в RegionP

template<class T>
inline Adjacency CheckAdj( const RegionP<T>& left, const RegionP<T>& middle, const RegionP<T>& right );    // Определяет тип смежности среднего региона с соседними

template<template<class> class REG_T, class CELL_T, class REG = REG_T<CELL_T>>                     // Находит индекс региона в S-List или P-List. Если не найден, возвращает -1
inline size_t FindRegion( const std::vector<REG>& in, const REG& toSearch );

template<class T>
inline size_t Find_SList_InsertionIndex( const std::vector<RegionS<T>>& in, const RegionS<T>& toInsert );  // Находит индекс вставки региона в S-List (по правилам сортировки S-List)

/*  Находит, в какую часть S-List (правую/левую) будет вставлен регион (по правилам вставки в S-List)  */
template<class T>
inline Side Find_SList_InsertionSide( const std::vector<RegionS<T>>& in, const RegionS<T>& toInsert );

/*  Находит и возвращает первую найденную подстроку из исходной строки по регулярному  выражению  */
std::tuple<Error_BasePtr, std::string> FindStrRegular( const std::string& str, const Regex& expr, bool& founded, const std::string& default_res = "" );

/*  Удаляет и выдаёт первую найденную подстроку из исходной строки по регулярному выражению. */
std::string RemoveStrRegular( std::string& str, const Regex& expr, bool& founded, Error_BasePtr& err );

/*  Удаляет регион из S-List или P-List. Возвращает, из какой части (правой/левой) было удаление  */
template<template<class> class REG_T, class CELL_T, class REG = REG_T<CELL_T>>
inline Side DeleteRegion( std::vector<REG>& in, const REG& toDel );

/*  Строковое представление для различных типов  */
template<class T>
inline std::string to_string( const RegionP<T>& obj );                 // "[61488;01]",       где 61488 - start, 01 - size;

template<class T>
inline std::string to_string( const RegionS<T>& obj );                 // "[61492;01;05]",    где 61488 - start, 01 - size, 05 - count;

template<class T>
inline std::string to_string( const std::vector<RegionP<T>>& obj );    // "[61488;01][61492;01][61498;01][61504;01][61510;01][61516;01]"

template<class T>
inline std::string to_string( const std::vector<RegionS<T>>& obj );    // "[61492;01;05][61498;01;00][61504;01;00][61510;01;00][61516;01;00]"

std::string to_string( const ListFootprints& val );

std::string to_string( const ListState& val );

int random_int( int min, int max );

template<class T>
Error_BasePtr Attempt_calloc( uint8_t attemptions, uint16_t period_ms, size_t calloc_size, T* &output );

template<class T>
Error_BasePtr Attempt_realloc( uint8_t attemptions, uint16_t period_ms, size_t realloc_size, T *&output );
}

/*  Набор операторов сравнения для различных типов  */
template<class T>
inline bool operator==( const RegionP<T>& left, const RegionP<T>& right );

template<class T>
inline bool operator==( const RegionS<T>& left, const RegionS<T>& right );

template<class T>
inline bool operator!=( const RegionP<T>& left, const RegionP<T>& right );

template<class T>
inline bool operator!=( const RegionS<T>& left, const RegionS<T>& right );

template<class T>
RegionP<T> operator+( const RegionP<T>& l_op, size_t r_op );

template<class T>
RegionP<T> operator-( const RegionP<T>& l_op, size_t r_op );


namespace utils
{
/*      Находит индекс первого элемента массива, >= (lower_bound) или < (upper_bound) искомого значения. Если такой элемент не найден
 *  - возвращает длину массива. Массив должен быть отсортирован по типу искомого значения.
 */
template<class T>
size_t lower_bound( RegionP<T>* arr, size_t size, const T* value, bool& founded )
{
    size_t l = 0;
    size_t h = size;
    size_t mid;

    while (l < h) {
        mid = (l + h) / 2;
        if (value <= arr[mid].start) {
            h = mid;
        }
        else {
            l = mid + 1;
        }
    }
    founded = (l < size) && (arr[l].start == value);
    return l;
}


template<class T>
size_t lower_bound( RegionS<T>* arr, size_t size, size_t value, bool& founded )
{
    size_t l = 0;
    size_t h = size;
    size_t mid;

    while (l < h) {
        mid = (l + h) / 2;
        if (value <= arr[mid].size) {
            h = mid;
        }
        else {
            l = mid + 1;
        }
    }
    founded = ( l < size ) && (arr[l].size == value);
    return l;
}


template<class T>
size_t lower_bound( RegionP<T>* arr, size_t size, const T* value )
{
    size_t l = 0;
    size_t h = size;
    size_t mid;

    while (l < h) {
        mid = (l + h) / 2;
        if (value <= arr[mid].start) {
            h = mid;
        }
        else {
            l = mid + 1;
        }
    }
    return l;
}


template<class T>
size_t lower_bound( RegionS<T>* arr, size_t size, size_t value )
{
    size_t l = 0;
    size_t h = size;
    size_t mid;

    while (l < h) {
        mid = (l + h) / 2;
        if (value <= arr[mid].size) {
            h = mid;
        }
        else {
            l = mid + 1;
        }
    }
    return l;
}


template<class T>
size_t lower_bound( RegionS<T>* arr, size_t size, const T* value )
{
    size_t l = 0;
    size_t h = size;
    size_t mid;

    while (l < h) {
        mid = (l + h) / 2;
        if (value <= arr[mid].start) {
            h = mid;
        }
        else {
            l = mid + 1;
        }
    }
    return l;
}


template<class T>
size_t upper_bound( RegionP<T>* arr, size_t size, T* value, bool& founded ) {
    size_t l = 0;
    size_t h = size;
    size_t mid;
    while (l < h) {
        mid = (l + h) / 2;
        if (value >= arr[mid].start) {
            l = mid + 1;
        }
        else {
            h = mid;
        }
    }
    founded = ( l < size ) && (arr[l].start == value);
    return l;
}


template<class T>
size_t upper_bound( RegionP<T>* arr, size_t size, size_t value, bool& founded ) {
    size_t l = 0;
    size_t h = size;
    size_t mid;
    while (l < h) {
        mid = (l + h) / 2;
        if (value >= arr[mid].size) {
            l = mid + 1;
        }
        else {
            h = mid;
        }
    }
    founded = ( l < size ) && (arr[l].size == value);
    return l;
}


template<class T>
size_t upper_bound( RegionP<T>* arr, size_t size, T* value ) {
    size_t l = 0;
    size_t h = size;
    size_t mid;
    while (l < h) {
        mid = (l + h) / 2;
        if (value >= arr[mid].start) {
            l = mid + 1;
        }
        else {
            h = mid;
        }
    }
    return l;
}


template<class T>
size_t upper_bound( RegionP<T>* arr, size_t size, size_t value ) {
    size_t l = 0;
    size_t h = size;
    size_t mid;
    while (l < h) {
        mid = (l + h) / 2;
        if (value >= arr[mid].size) {
            l = mid + 1;
        }
        else {
            h = mid;
        }
    }
    return l;
}


/*  Строит RegionS на основе RegionP, просто добавляя отсутствующее поле "count" = 0  */
template<class T>
RegionS<T> RegionPtoS( const RegionP<T>& regP, size_t count )
{
    return { regP.start, regP.size, count };
}


/*  Строит S-List на основе P-List (пересортировкой сначала по size, затем по start)  */
template<class T>
std::vector<RegionS<T>> SListFromPList( const std::vector<RegionP<T>>& in )
{
    std::vector<RegionS<T>> ret;
    size_t max_size = 0;
    for (const auto& in_reg : in)
    {
        if (max_size < in_reg.size) {
            max_size = in_reg.size;
        }
    }
    std::vector<std::vector<RegionP<T>>> list_inter( max_size );	// Промежуточный двумерный вектор для сортировки

    // Формируем исходный S-список на основе P-списка (сортируем эл-ты сначала по Size, затем по Pointer)
    for (const auto& p_reg : in) {
        list_inter[p_reg.size - 1].push_back( p_reg );
    }
    for (const auto& p_vec : list_inter)
    {
        if (p_vec.size() == 0) {
            continue;
        }
        uint8_t count_enable = 1;
        for (const auto& p_reg : p_vec)
        {
            RegionS<T> reg = { p_reg.start, p_reg.size, p_vec.size() * count_enable };
            ret.push_back( reg );
            count_enable = 0;
        }
    }
    return ret;
}


/*  Проверяет, смежны ли регионы в RegionP */
template<class T>
bool CheckAdj( const RegionP<T>& left, const RegionP<T>& right ) { return left.start + left.size == right.start; }


/*  Определяет тип смежности среднего региона с соседними левым/правым регионами  */
template<class T>
Adjacency CheckAdj( const RegionP<T>& left, const RegionP<T>& middle, const RegionP<T>& right )
{
    if (!CheckAdj( left, middle ) && !CheckAdj( middle, right ))
        return Adjacency::Adj_NONE;
    else if (CheckAdj( left, middle ) && !CheckAdj( middle, right ))
        return Adjacency::Adj_Left;
    else if (!CheckAdj( left, middle ) && CheckAdj( middle, right ))
        return Adjacency::Adj_Right;
    else
        return Adjacency::Adj_Both;
}


/*  Находит индекс региона в S-List или P-List. Если не найден, возвращает -1  */
template<template<class> class REG_T, class CELL_T, class REG>
size_t FindRegion( const std::vector<REG>& in, const REG& toSearch )
{
    if (!in.size()) {
        Log::warning() << "FindRegion(): Empty input vector passed in FindRegion()" << Log::endlog{};
        return std::numeric_limits<size_t>::max();
    }
    if constexpr (std::is_same_v<REG, RegionP<CELL_T>> || std::is_same_v<REG, RegionS<CELL_T>>)
    {
        for (size_t i = 0; i < in.size(); ++i) {
            if (in[i].start == toSearch.start && in[i].size == toSearch.size)
                return i;
        }
        return std::numeric_limits<size_t>::max();
    }
    Log::warning() << "FindRegion(): Undefined Region Type received: " + std::string(typeid(REG_T).name()) << Log::endlog{};
    return std::numeric_limits<size_t>::max();
}


/*  Находит индекс вставки региона в S-List (по правилам сортировки S-List)  */
template<class T>
size_t Find_SList_InsertionIndex( const std::vector<RegionS<T>>& in, const RegionS<T>& toInsert )
{
    if (!in.size()) {
        return std::numeric_limits<size_t>::max();
    }
    std::vector<RegionS<T>> subArray;
    size_t subArray_begin = 0;
    bool subArray_begin_detected = false;
    for (size_t i = 0; i < in.size(); ++i)
    {
        if (toInsert.size == in[i].size)
        {
            if (!subArray_begin_detected) {
                subArray_begin = i;
                subArray_begin_detected = true;
            }
            subArray.push_back( in[i] );
        }
    }
    if (!subArray.size())
    {
        size_t i = 0;
        for (; i < in.size(); ++i)
        {
            if (toInsert.size < in[i].size) {
                return i;
            }
        }
        return i;
    }
    else
    {
        size_t i = 0;
        for (; i < subArray.size(); ++i)
        {
            if (toInsert.start < subArray[i].start) {
                break;
            }
        }
        return subArray_begin + i;
    }
}


/*  Находит, в какую часть S-List (правую/левую) будет вставлен регион (по правилам сортировки S-List)  */
template<class T>
Side Find_SList_InsertionSide( const std::vector<RegionS<T>>& in, const RegionS<T>& toInsert )
{
    return Find_SList_InsertionIndex<T>( in, toInsert ) >= in.size() / 2 ? Side_RIGHT : Side_LEFT;;
}


template<template<class> class REG_T, class CELL_T, class REG>
Side DeleteRegion( std::vector<REG>& in, const REG& toDel )
{
    if constexpr (std::is_same_v<REG, RegionP<CELL_T>> || std::is_same_v<REG, RegionS<CELL_T>>)
    {
        size_t del_index = FindRegion<REG_T, CELL_T>( in, toDel );
        if (del_index == std::numeric_limits<size_t>::max()) {
            return Side_NONE;
        }
        Side side = del_index >= in.size() / 2 ? Side_RIGHT : Side_LEFT;
        in.erase( in.begin() + del_index );
        return side;
    }
    Log::warning() << "DeleteRegion(): Undefined Region Type received: " + std::string( typeid(REG_T).name() ) << Log::endlog{};
    return Side_NONE;
}


/*  Строковое представление для различных типов  */
template<class T>
std::string to_string( const RegionP<T>& obj )
{
    std::string start = std::to_string( (size_t)obj.start );
    std::string size = std::to_string( obj.size );
    std::string fill;
    if (obj.size < 10) {
        fill = "0";
    }
    return "[" + start + ";" + fill + size + "]";
}

template<class T>
std::string to_string( const RegionS<T>& obj )
{
    std::string start = std::to_string( (size_t)obj.start );
    std::string size = std::to_string( obj.size );
    std::string count = std::to_string( obj.count );
    std::string fill_sz, fill_cnt;
    if (obj.size < 10) {
        fill_sz = "0";
    }
    if (obj.count < 10) {
        fill_cnt = "0";
    }
    return "[" + start + ";" + fill_sz + size + ";" + fill_cnt + count + "]";
}

template<class T>
std::string to_string( const std::vector<RegionP<T>>& obj )
{
    std::string out;
    for (auto it = obj.begin(); it != obj.end(); ++it)
    {
        std::string start = std::to_string( (size_t)it->start );
        std::string size = std::to_string( it->size );
        std::string fill;
        if (it->size < 10) {
            fill = "0";
        }
        out += "[" + start + ";" + fill + size + "]";
    }
    return out;
}

template<class T>
std::string to_string( const std::vector<RegionS<T>>& obj )
{
    std::string out;
    for (auto it = obj.begin(); it != obj.end(); ++it)
    {
        std::string start = std::to_string( (size_t)it->start );
        std::string size = std::to_string( it->size );
        std::string count = std::to_string( it->count );
        std::string fill_sz, fill_cnt;
        if (it->size < 10) {
            fill_sz = "0";
        }
        if (it->count < 10) {
            fill_cnt = "0";
        }
        out += "[" + start + ";" + fill_sz + size + ";" + fill_cnt + count + "]";
    }
    return out;
}


template<class T>
Error_BasePtr Attempt_calloc( uint8_t attemptions, uint16_t period_ms, size_t calloc_size, T* &output )
{
    output = reinterpret_cast<T*>(calloc( calloc_size, sizeof( T )));
    if (!output) {
        for (uint8_t i = 0; i < attemptions; ++i) {
            std::this_thread::sleep_for( std::chrono::milliseconds( period_ms ));
            if( output = reinterpret_cast<T*>( calloc( calloc_size, sizeof( T ))))
                break;
        }
        if (!output) {
            return ERR_MEMALLOC( calloc_size, sizeof( T ), "Type is \"" + std::string( typeid( T ).name() ) + "\". Calloc() returned NULL after " + std::to_string( attemptions ) + " attempts." );
        }
    }
    return nullptr;
}

template<class T>
Error_BasePtr Attempt_realloc( uint8_t attemptions, uint16_t period_ms, size_t realloc_size, T *&output )
{
    auto resized_block = realloc( output, realloc_size * sizeof( T ));
    if( !resized_block ) {
        for( uint8_t i = 0; i < attemptions; ++i ) {
            std::this_thread::sleep_for( std::chrono::milliseconds( period_ms ));
            if( resized_block = realloc( output, realloc_size * sizeof( T ))) {
                output = reinterpret_cast<T*>( resized_block );
                break;
            }
        }
        if( !resized_block ) {
            return ERR_MEMALLOC( output, realloc_size, "Type is \"" + std::string(typeid(T).name()) + "\". Realloc() returned NULL after " + std::to_string( attemptions ) + " attempts." );
        }
    }
    else {
        output = reinterpret_cast<T*>( resized_block );
    }
    return nullptr;
}

} // utils


/*  Набор операторов сравнения для различных типов  */
template<class T>
bool operator==( const RegionP<T>& left, const RegionP<T>& right ) {
    return (left.size == right.size) && (left.start == right.start);
}

template<class T>
bool operator==( const RegionS<T>& left, const RegionS<T>& right ) {
    return (left.size == right.size) && (left.start == right.start) && (left.count == right.count);
}

template<class T>
bool operator!=( const RegionP<T>& left, const RegionP<T>& right ) {
    return !(left == right);
}

template<class T>
bool operator!=( const RegionS<T>& left, const RegionS<T>& right ) {
    return !(left == right);
}

template<class T>
RegionP<T> operator+( const RegionP<T>& l_op, size_t r_op )
{
    RegionP<T> res = l_op;
    for (size_t i = 0; i < res.size; ++i)
        res.start[i] += r_op;
    return res;
}

template<class T>
RegionP<T> operator-( const RegionP<T>& l_op, size_t r_op )
{
    RegionP<T> res = l_op;
    for (size_t i = 0; i < res.size; ++i)
        res.start[i] -= r_op;
    return res;
}