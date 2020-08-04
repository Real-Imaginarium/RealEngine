#pragma once
#include "LogDebug.h"


class LogTestRun : public LogDebug
{
public:
    std::string Header() const override { return std::string( "[ TEST RUN   ]" ); }
};