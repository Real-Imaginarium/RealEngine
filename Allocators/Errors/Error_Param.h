#pragma once

#include "Error_Base.h"


class Error_Param : public Error_Base
{
public:
    ERRORS_API Error_Param(
        int arg_number, std::string arg_name, std::string expected, std::string gained, const char* file_name, const char* func_name, int line, const char* logFile_name = "ErrorLog.txt" );

protected:
    virtual std::string Name() const override;

    virtual void PrintSelf() const override;

private:
    int m_arg_number;
    std::string m_arg_name;
    std::string m_expected;
    std::string m_gained;
};