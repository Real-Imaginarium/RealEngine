#include "Error_Custom.h"
#include "LogError.h"

#include <iostream>


Error_Custom::Error_Custom( const std::string& desc, const char* file_name, const char* func_name, int line, const char* logFile_name )
    : Error_Base( file_name, func_name, line, logFile_name )
    , m_description( desc )
{
}

void Error_Custom::PrintSelf() const
{
    Error_Base::PrintSelf();

    for (const auto& str : Error_Base::SplitString( m_description, '\n' ))
    {
        Log::error( m_logFile_name.generic_string() ) << std::string( m_tab_size, ' ' ) + str << Log::endlog();
    }
    return;
}

std::string Error_Custom::Name() const
{
    return "Error";
}