#include "LogTest.h"
#include <Windows.h>

LogTest::LogTest()
    : m_stateStr("undefined")
    , m_state( TestStates::TestState_UNDEFINED )
{
}

void LogTest::SetStyle( const Styles& style )
{
    HANDLE hConsole = GetStdHandle( STD_OUTPUT_HANDLE );
    switch (style)
    {
    case Styles::HEADER: {
        SetConsoleTextAttribute( hConsole, (WORD)((Black << 4) | Cyan) );
        break;
    }
    case Styles::PLAIN_TEXT:
    case Styles::IMPORTANT_TEXT: {
        std::cout << "### Styles::PLAIN_TEXT and Styles::IMPORTANT_TEXT doesn't make sense for LogTest ###" << std::endl;
        break;
    }
    case Styles::TEST_FAILED: {
        SetConsoleTextAttribute( hConsole, (WORD)((Black << 4) | LightRed) );
        break;
    }
    case Styles::TEST_PASSED: {
        SetConsoleTextAttribute( hConsole, (WORD)((Black << 4) | Green) );
        break;
    }
    }
}


void LogTest::operator<<( const Start& val )
{
    if (!SwitchState( TestStates::TestState_STARTED )) {
        return;
    }
    m_stateStr = "STARTED";
    m_casesNumber = val.testCases_number;
    m_testName = val.test_name;
    m_casesPassed = 0;
    Log::operator<<( "---" + val.test_name);
    Log::operator<<( Log::endlog{} );
}


void LogTest::operator<<( const Progress& )
{
    if (!SwitchState( TestStates::TestState_PROGRESS )) {
        return;
    }
    m_stateStr = "RUNNING";

    m_casesPassed++;
    if (m_casesPassed > m_casesNumber)
    {
        m_casesPassed = m_casesNumber;
    }
    size_t progress = static_cast<size_t>((static_cast<double>(m_casesPassed) / static_cast<double>(m_casesNumber)) * 100);

    SetStyle( m_current_style );
    std::cout << "\r" << Header() << " " << std::to_string( progress ) << "%";

    if (m_outputToFile && m_outputFile.is_open()) {
        m_outputFile << Header() << " " << std::to_string( progress ) << "%\n";
        m_outputFile.close();
    }

    m_needNewLine = true;
    m_stream_mutex.unlock();
}


void LogTest::operator<<( const Finished& val )
{
    if (!(val.ok == true ? SwitchState( TestStates::TestState_PASS ) : SwitchState( TestStates::TestState_FAIL ))) {
        m_stream_mutex.unlock();
        return;
    }
    m_stateStr = val.ok == true ? " PASSED" : " FAILED";
    if (m_needNewLine)
    {
        std::cout << "\n";
        m_needNewLine = false;
    }
    SetStyle( Styles::TEST_FAILED );
    if (val.ok) {
        SetStyle( Styles::TEST_PASSED );
    }
    operator<<( Log::endlog{} );
    m_stream_mutex.unlock();
}


void LogTest::operator<<( const endlog& end )
{
    if (m_outputFile.is_open()) {
        if (!m_header_sended)
            m_outputFile << Header();// << " " << "********** " << m_testName << " **********";
        m_outputFile << std::endl;
        m_outputFile.close();
    }

    m_outputToFile = false;
    
    if (!m_header_sended) {
        std::cout << Header();// << " ********** " << m_testName << " **********";
    }
    m_header_sended = false;
    std::cout << std::endl;
}


bool LogTest::SwitchState( TestStates newState )
{
    switch (m_state)
    {
    case TestStates::TestState_UNDEFINED:
    {
        if (newState == TestStates::TestState_STARTED) {
            m_state = newState;
            return true;
        }
        return false;
    }
    case TestStates::TestState_STARTED:
    {
        if (newState == TestStates::TestState_PROGRESS || newState == TestStates::TestState_FAIL || newState == TestStates::TestState_PASS) {
            m_state = newState;
            return true;
        }
        return false;
    }
    case TestStates::TestState_PROGRESS:
    {
        if(newState == TestStates::TestState_PROGRESS || newState == TestStates::TestState_FAIL || newState == TestStates::TestState_PASS) {
            m_state = newState;
            return true;
        }
        return false;
    }
    case TestStates::TestState_FAIL:
    case TestStates::TestState_PASS:
    {
        if (newState == TestStates::TestState_UNDEFINED || newState == TestStates::TestState_STARTED) {
            m_state = newState;
            return true;
        }
        return false;
    }
    default:
        return false;
    }
}
