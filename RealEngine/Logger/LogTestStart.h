#pragma once
#include "LogInfo.h"


class LogTestStart : public LogInfo
{
public:
    std::string Header() const override { return std::string("[ TEST START ]"); }
};