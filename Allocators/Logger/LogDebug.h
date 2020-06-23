#pragma once
#include "Log.h"


class LogDebug : public Log
{
public:
    void SetStyle( const Styles& style ) override;
    
    std::string Header() const override
    {
        return std::string( "[ DEBUG ]" );
        //std::ostringstream sstr;
        //sstr << std::this_thread::get_id();
        //return std::string( "[ DEBUG ][id: " + sstr.str() + "] " );

    }
};

