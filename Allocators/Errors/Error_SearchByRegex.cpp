#include "Error_SearchByRegex.h"
#include "LogError.h"

#include <iostream>


Error_SearchByRegex::Error_SearchByRegex( const std::string& src, const Regex& reg, const char* file_name, const char* func_name, int line, const char* logFile_name )
    : Error_Base( file_name, func_name, line, logFile_name )
    , m_source( src )
    , m_regex( reg )
{
}

void Error_SearchByRegex::PrintSelf() const
{
    Error_Base::PrintSelf();
    Log::error( m_logFile_name.generic_string() ) <<
        std::string( m_tab_size, ' ' ) + "Can't find the substring from string:\t" + m_source <<
        std::string( m_tab_size, ' ' ) + "By expression:\t" + m_regex.str << Log::endlog();
    return;
}

std::string Error_SearchByRegex::Name() const
{
    return "Search by regex Error";
}
