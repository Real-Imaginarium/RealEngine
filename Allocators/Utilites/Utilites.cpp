#include "Utilites.hpp"

namespace utils
{
size_t lower_bound( int* arr, size_t size, int value, bool& founded )
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

size_t lower_bound( int* arr, size_t size, int value )
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

std::vector<std::string> SplitString( const std::string& s, char delimiter )
{
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream( s );

    while (std::getline( tokenStream, token, delimiter )) {
        tokens.push_back( token );
    }
    return tokens;
}

std::string FindStrRegular( const std::string& str, const Regex& expr, bool& founded, Error_BasePtr& err, const std::string& default_res )
{
    // Проверяем параметры
    if (str.empty())
    {
        err = std::make_shared<Error_Param>( 1, "const std::string &", "Not empty", "\"\"", PLACE(), "Log.txt" );
        return std::string();
    }
    // Находим и возвращаем подстроку, если нет - выдаём дефолтную
    std::sregex_iterator it( str.begin(), str.end(), expr.expr );

    if (it == std::sregex_iterator())
    {
        founded = false;
        return default_res;
    }
    founded = true;
    return it->str();
}

std::string RemoveStrRegular( std::string& str, const Regex& expr, bool& founded, Error_BasePtr& err )
{
    // Проверяем параметры
    if (str.empty())
    {
        err = std::make_shared<Error_Param>( 1, "const std::string &", "Not empty", "\"\"", PLACE(), "Log.txt" );
        return std::string();
    }

    // Ищем подстроку для удаления, если ошибка - трейсим наверх, если просто не найдена - выдаём ""
    std::string err_message = "Can't proceed to remove from string: \"" + str + "\"\nby regular expression:               \"" + expr.str + "\"";
    std::string substr = FindStrRegular( str, expr, founded, err, "" );     TRACE_RETURN( err, err_message, std::string() );


    if (!founded) {
        return std::string();
    }

    // Удаляем и возвращаем найденную подстроку из исходной строки
    size_t pos = str.find( substr );
    if (pos == std::string::npos)
    {
        err = std::make_shared<Error_Custom>( "Can't find the substring:\t\"" + substr + "\"\nto be removed from string:\t\"" + str + "\"", PLACE(), "Log.txt" );
        return std::string();
    }
    str.erase( pos, substr.size() );
    return substr;
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