#pragma once
#include "LogError.h"


class LogTestFail : public LogError
{
public:
    std::string Header() const override { return std::string( "[ TEST FAIL  ]" ); }
};