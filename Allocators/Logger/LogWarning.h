#pragma once
#include "Log.h"


class LogWarning : public Log
{
public:
    void SetStyle( const Styles& style ) override;

    std::string Header() const override
    {
        return std::string( "[ WARN  ]" );
        //std::ostringstream sstr;
        //sstr << std::this_thread::get_id();
        //return std::string( "[ WARN  ][id: " + sstr.str() + "] " );
    }
};

