#include "Error_Param.h"
#include "LogError.h"

#include <iostream>


Error_Param::Error_Param(
    int arg_number, std::string arg_name, std::string expected, std::string gained, const char* file_name, const char* func_name, int line, const char* logFile_name )
    : Error_Base( file_name, func_name, line, logFile_name )
    , m_arg_number( arg_number )
    , m_arg_name( arg_name )
    , m_expected( expected )
    , m_gained( gained )
{
}

void Error_Param::PrintSelf() const
{
    Error_Base::PrintSelf();
    Log::error( m_logFile_name.generic_string() ) <<
        std::string( m_tab_size, ' ' ) + "Bad parameter " + std::to_string( m_arg_number ) + " (" + m_arg_name + "):\n" <<
        std::string( m_tab_size, ' ' ) + "Expected: " + m_expected + "\n" <<
        std::string( m_tab_size, ' ' ) + "Gained: " + m_gained << Log::endlog();
    return;
}

std::string Error_Param::Name() const
{
    return "Params Error";
}
