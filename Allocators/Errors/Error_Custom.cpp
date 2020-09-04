#include "Error_Custom.h"
#include "LogError.h"

#include <iostream>


Error_Custom::Error_Custom( const std::string &desc, const char *file, const char *func, int line, const char *logFile )
    : Error_Base( file, func, line, logFile )
    , m_description( desc )
{
}

Error_Custom::~Error_Custom()
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