#pragma once

#define PLACE() __FILE__, __FUNCTION__, __LINE__

#include <filesystem>
#include <memory>
#include <list>
#include <string>


class Error_Base;
using Error_BasePtr = std::shared_ptr<Error_Base>;
using ErrorList = std::list<Error_BasePtr>;


class Error_Base
{
public:
    static bool Trace( Error_BasePtr err, Error_BasePtr consiquent_err );

    Error_Base( const char *file_name, const char *func_name, int line, const char *logFile_name = g_error_log_name );

    virtual void Print();

    static const char *g_error_log_name;

protected:
    virtual ~Error_Base();

    virtual std::string Name() const = 0;

    virtual void PrintSelf() const;

    static std::vector<std::string> SplitString( const std::string& s, char delimiter );

    std::filesystem::path m_file;
    std::filesystem::path m_logFile_name;
    std::string m_function;
    int m_line;
    int m_tab_size;

private:
    void SetTabSize( int val ) { m_tab_size = val; }

    ErrorList m_consequent_errors;
};


template<class CAST>
std::shared_ptr<CAST> ErrorCast( Error_BasePtr errToCast )
{
    return std::dynamic_pointer_cast<CAST, Error_Base>( errToCast );
}
