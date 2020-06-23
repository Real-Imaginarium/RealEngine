#pragma once

#ifdef ERRORS_EXPORTS
    #define ERRORS_API __declspec(dllexport)
#else
    #define ERRORS_API __declspec(dllimport)
#endif

#define PLACE() __FILE__, __FUNCTION__, __LINE__

#include <filesystem>
#include <memory>
#include <list>
#include <string>


class Error_Base;
typedef std::shared_ptr<Error_Base> Error_BasePtr;
typedef std::list<Error_BasePtr> ErrorList;


class Error_Base
{
public:
    ERRORS_API static bool Trace( Error_BasePtr err, const std::string& desc = "", const char* file_name = nullptr, const char* func_name = nullptr, int line = -1 );

    ERRORS_API Error_Base( const char* file_name, const char* func_name, int line, const char* logFile_name = "ErrorLog.txt" );

    ERRORS_API virtual void Print();

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
