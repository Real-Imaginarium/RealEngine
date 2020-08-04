#pragma once
#include "Log.h"


class LogError : public Log
{
public:
    void SetStyle( const Styles& style ) override;

    std::string Header() const override
    {
        return std::string( "[ ERROR ]" );
        //std::ostringstream sstr;
        //sstr << std::this_thread::get_id();
        //return std::string( "[ ERROR ][id: " + sstr.str() + "] " );
    }
};