#pragma once

#include <stdint.h>
#include <regex>


/* Элемент P-List */
template<class T> 
struct RegionP {
    T *start = nullptr;     // Начало региона
    size_t size = 0;        // Размер региона, байт

    bool operator==( T *rhs_start ) { return start == rhs_start; }

    bool operator<=( RegionP *rhs ) { return start <= rhs->start; }
};


/* Элемент S-List */
template<class T>
struct RegionS {
    T *start = nullptr;     // Начало региона
    size_t size = 0;        // Размер региона, байт
    size_t count = 0;       // Количество регионов с таким "size" в Size-списке
};


/* Определение части (правая/левая). Используется для описания позиции вставки или удаления */
enum Side
{
    Side_NONE,
    Side_LEFT,
    Side_RIGHT
};


/* Типы смежности */
enum class Adjacency {
    Adj_NONE,
    Adj_Left,
    Adj_Right,
    Adj_Both
};


/* Описание адресного пространства списка */
struct ListFootprints
{
    size_t addr_start;      // Начало списка
    size_t addr_stop;       // Конец списка
    size_t addr_begin;      // Начало последовательности элементов в списке
    size_t addr_end;        // Конец последовательности элементов в списке
};


/* Состояние списка */
struct ListState
{
    size_t capacity;        // Ёмкость списка
    size_t size;            // Число элементов в списке
    size_t spaceLeft;       // Ширина правого поля (элементов)
    size_t spaceRight;      // Ширина левого поля (элементов)
    size_t begin_pos;       // Позиция Begin() списка (элементов)
    size_t end_pos;         // Позиция End() списка (элементов)

    bool operator==( const ListState& r ) const {
        return (this->capacity == r.capacity && this->size == r.size && this->spaceLeft == r.spaceLeft && this->spaceRight == r.spaceRight && this->begin_pos == r.begin_pos && this->end_pos == r.end_pos);
    }
    bool operator!=( const ListState& r ) const {
        return !(*this == r);
    }
};


/* Строковая обёртка над std::regex */
struct Regex
{
    Regex(std::string expr) 
        : expr(expr)
        , str(expr)
    {}

    std::regex expr;
    std::string str;
};