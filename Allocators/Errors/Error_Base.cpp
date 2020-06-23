#include "Error_Base.h"
#include "Error_Consiquent.h"
#include "LogError.h"

#include <vector>
#include <iostream>
#include <sstream>


Error_Base::Error_Base( const char* file_name, const char* func_name, int line, const char* logFile_name )
    : m_file( file_name )
    , m_function( func_name )
    , m_line( line )
    , m_logFile_name( logFile_name )
    , m_tab_size( 0 )
{
}


void Error_Base::PrintSelf() const
{
    Log::error( m_logFile_name.generic_string() ) <<
        std::string( m_tab_size, ' ' ) << Styles::IMPORTANT_TEXT << Name() + "\n" <<
        std::string( m_tab_size, ' ' ) << "File:     " << Styles::HEADER << m_file.generic_string() + "\n" << Styles::IMPORTANT_TEXT << 
        std::string( m_tab_size, ' ' ) << "Function: " << Styles::HEADER << m_function + "\n"              << Styles::IMPORTANT_TEXT << 
        std::string( m_tab_size, ' ' ) << "Line:     " << Styles::HEADER << std::to_string( m_line )       << Log::endlog();
    return;
}


void Error_Base::Print()
{
    PrintSelf();

    int tab_size = 4;
    for (const auto err : m_consequent_errors)
    {
        err->SetTabSize( tab_size );
        err->Print();
        tab_size += 4;
    }
    return;
}


std::vector<std::string> Error_Base::SplitString( const std::string& s, char delimiter )
{
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream( s );
    while (std::getline( tokenStream, token, delimiter ))
    {
        tokens.push_back( token );
    }
    return tokens;
}


Error_Base::~Error_Base()
{
}


bool Error_Base::Trace( Error_BasePtr err, const std::string& desc, const char* file_name, const char* func_name, int line )
{
    if (err) {
        if (!desc.empty() && file_name && func_name && line != -1) {
            Error_BasePtr error = std::make_shared<Error_Consiquent>( desc, file_name, func_name, line, "Log.txt" );
            err->m_consequent_errors.push_back( error );
        }
        return true;
    }
    return false;
}