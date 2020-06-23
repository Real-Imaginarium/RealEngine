#include "Error_Consiquent.h"


Error_Consiquent::Error_Consiquent( const std::string& desc, const char* file_name, const char* func_name, int line, const char* logFile_name )
    : Error_Custom( desc, file_name, func_name, line, logFile_name )
    , m_description( desc )
{
}

std::string Error_Consiquent::Name() const
{
    return "Consiquent Error";
}
