#pragma once

#include "Error_Custom.h"


class Error_Consiquent : public Error_Custom
{
public:
    ERRORS_API Error_Consiquent( const std::string& desc, const char* file_name, const char* func_name, int line, const char* logFile_name = "ErrorLog.txt" );

protected:
    virtual std::string Name() const override;

private:
    std::string m_description;
};
