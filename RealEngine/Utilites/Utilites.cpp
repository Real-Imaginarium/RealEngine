#include "Utilites.hpp"

namespace utils
{
size_t lower_bound( size_t * arr, size_t size, size_t value, bool& founded )
{
    size_t l = 0;
    size_t h = size;
    size_t mid;
    while (l < h) {
        mid = (l + h) / 2;
        if (value <= arr[mid]) {
            h = mid;
        }
        else {
            l = mid + 1;
        }
    }
    founded = (arr[l] == value);
    return l;
}


size_t lower_bound( size_t * arr, size_t size, size_t value )
{
    size_t l = 0;
    size_t h = size;
    size_t mid;
    while (l < h) {
        mid = (l + h) / 2;
        if (value <= arr[mid]) {
            h = mid;
        }
        else {
            l = mid + 1;
        }
    }
    return l;
}


size_t upper_bound( int* arr, size_t size, int value, bool& founded ) {
    size_t l = 0;
    size_t h = size;
    size_t mid;
    while (l < h) {
        mid = (l + h) / 2;
        if (value >= arr[mid]) {
            l = mid + 1;
        }
        else {
            h = mid;
        }
    }
    founded = (arr[l] == value);
    return l;
}


size_t upper_bound( int* arr, size_t size, int value ) {
    size_t l = 0;
    size_t h = size;
    size_t mid;
    while (l < h) {
        mid = (l + h) / 2;
        if (value >= arr[mid]) {
            l = mid + 1;
        }
        else {
            h = mid;
        }
    }
    return l;
}


std::tuple<Error_BasePtr, std::string> FindStrRegular( const std::string& str, const Regex& expr, bool& founded, const std::string& default_res )
{
    // Проверяем параметры
    if (str.empty()) {
        founded = false;
        return std::tuple( nullptr, default_res );
    }
    // Находим и возвращаем подстроку, если нет - выдаём дефолтную
    std::sregex_iterator it( str.begin(), str.end(), expr.expr );

    if (it == std::sregex_iterator()) {
        founded = false;
        return std::tuple( nullptr, default_res );
    }
    founded = true;
    return std::tuple( nullptr, it->str() );
}


std::tuple<Error_BasePtr, std::string> RemoveStrRegular( std::string& str, const Regex& expr, bool& founded )
{
    // Проверяем параметры
    if (str.empty()) {
        return std::tuple<Error_BasePtr, std::string>( ERR_PARAM( 1, "const std::string &", "Not empty", "\"\"" ), "" );
    }
    // Ищем подстроку для удаления, если ошибка - трейсим наверх, если просто не найдена - выдаём ""
    std::string err_message = "Can't proceed to remove from string: \"" + str + "\"\nby regular expression:               \"" + expr.str + "\"";
    std::string substr;
    Error_BasePtr err;
    std::tie(err, substr) = FindStrRegular( str, expr, founded, "" );                                                   TRACE_CUSTOM_RET_VAL( err, std::tuple(err, substr), err_message );

    if (!founded) {
        return std::tuple(nullptr, "");
    }

    // Удаляем и возвращаем найденную подстроку из исходной строки
    size_t pos = str.find( substr );
    if (pos == std::string::npos)
    {
        return std::tuple<Error_BasePtr, std::string>( ERR_CUSTOM( "Can't find the substring:\t\"" + substr + "\"\nto be removed from string:\t\"" + str + "\"" ), "" );
    }
    str.erase( pos, substr.size() );
    return std::tuple( nullptr, substr );
}


std::string to_string( const ListFootprints& val )
{
    return
        "ListFootprints.addr_start: " + std::to_string( val.addr_start ) +
        ", ListFootprints.addr_stop: " + std::to_string( val.addr_stop ) +
        ", ListFootprints.addr_begin: " + std::to_string( val.addr_begin ) +
        ", ListFootprints.addr_end: " + std::to_string( val.addr_end );
}


std::string to_string( const ListState& val )
{
    return
        "ListState.capacity: " + std::to_string( val.capacity ) +
        ", ListState.count: " + std::to_string( val.size ) +
        ", ListState.spaceLeft: " + std::to_string( val.spaceLeft ) +
        ", ListState.spaceRight: " + std::to_string( val.spaceRight ) +
        ", ListState.beginPos: " + std::to_string( val.begin_pos ) +
        ", ListState.endPos: " + std::to_string( val.end_pos );
}


int random_int( int min, int max )
{
    std::random_device dev;
    std::mt19937 rng( dev() );
    std::uniform_int_distribution<std::mt19937::result_type> distr( min, max );
    return distr( rng );
}

}// utils