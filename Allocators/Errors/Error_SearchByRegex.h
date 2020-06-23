#pragma once

#include "Error_Base.h"
#include "Types.h"


class Error_SearchByRegex : public Error_Base
{
public:
    ERRORS_API Error_SearchByRegex( const std::string& src, const Regex& reg, const char* file_name, const char* func_name, int line, const char* logFile_name = "ErrorLog.txt" );

protected:
    virtual std::string Name() const override;

    virtual void PrintSelf() const override;

private:
    std::string m_source;
    Regex m_regex;
};