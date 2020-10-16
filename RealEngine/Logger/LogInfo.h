#pragma once
#include "Log.h"


class LogInfo : public Log
{
public:
    void SetStyle( const Styles& style ) override;
    
    std::string Header() const override
    {
        return std::string( "[ INFO  ]" );
        //std::ostringstream sstr;
        //sstr << std::this_thread::get_id();
        //return std::string( "[ INFO  ][id: " + sstr.str() + "] " );
    }
};