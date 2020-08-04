#pragma once
#include "Log.h"


enum class TestStates : uint8_t
{
    TestState_UNDEFINED,
    TestState_STARTED,
    TestState_PROGRESS,
    TestState_PASS,
    TestState_FAIL
};


class LogTest : public Log
{
public:
    struct Start
    {
        std::string test_name;
        size_t testCases_number = 1;
    };

    struct Progress {};

    struct Finished
    {
        bool ok;
    };

    LogTest();

    void SetStyle( const Styles& style ) override;

    std::string Header() const override
    {
        return std::string( "[ TEST " + m_stateStr + " ]" );
        //std::ostringstream sstr;
        //sstr << std::this_thread::get_id();
        //return std::string( "[ TEST " + m_stateStr + " ][id: " + sstr.str() + "] " );
    }

    LOGGER_API void operator<<( const Start& val );

    LOGGER_API void operator<<( const Progress& );

    LOGGER_API void operator<<( const Finished& val );

private:
    LOGGER_API void operator<<( const endlog& end ) override;

    bool SwitchState( TestStates newState );

    std::string m_stateStr;
    std::string m_testName;
    TestStates m_state;
    size_t m_casesPassed;
    size_t m_casesNumber;
};