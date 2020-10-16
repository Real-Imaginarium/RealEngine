#pragma once
#include "LogInfo.h"


class LogTestPass : public LogInfo
{
public:
    std::string Header() const override { return std::string( "[ TEST PASS  ]" ); }
};