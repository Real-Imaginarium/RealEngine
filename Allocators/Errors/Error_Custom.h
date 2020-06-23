#pragma once

#include "Error_Base.h"


class Error_Custom : public Error_Base
{
public:
    ERRORS_API Error_Custom( const std::string& desc, const char* file_name, const char* func_name, int line, const char* logFile_name = "ErrorLog.txt" );

protected:
    virtual std::string Name() const override;

    virtual void PrintSelf() const override;

private:
    std::string m_description;
};