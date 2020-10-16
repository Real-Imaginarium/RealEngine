#include "Log.h"
#include "LogInfo.h"
#include "LogDebug.h"
#include "LogWarning.h"
#include "LogError.h"
#include "LogTest.h"
#include "LogTestStart.h"
#include "LogTestRun.h"
#include "LogTestPass.h"
#include "LogTestFail.h"

#include <sstream>


// Static definitions
LogDebug* Log::m_logD = nullptr;

LogInfo* Log::m_logI = nullptr;

LogWarning* Log::m_logW = nullptr;

LogError* Log::m_logE = nullptr;

LogTest* Log::m_logTest = nullptr;

bool Log::m_needNewLine = false;

std::recursive_mutex Log::m_stream_mutex;


Log& Log::operator<<( const Styles& st )
{
    m_current_style = st;
    SetStyle( st );
    return *this;
}


void Log::operator<<( const endlog& )
{
    if (m_outputFile.is_open()) {
        if (!m_header_sended)
            m_outputFile << Header() << " ";
        m_outputFile << std::endl;
        m_outputFile.close();
    }

    m_outputToFile = false;
    if (!m_header_sended) {
        SetStyle( Styles::HEADER );
        std::cout << Header() << " ";
        SetStyle( m_current_style );
    }
    m_header_sended = false;

    std::cout << std::endl;
    m_stream_mutex.unlock();
}


LogDebug& Log::debug( const std::string& toFile )
{
    m_stream_mutex.lock();
    if (m_logD == nullptr) {
        m_logD = new LogDebug;
    }
    m_logD->SetupFileOutput( toFile );
    m_logD->m_current_style = Styles::PLAIN_TEXT;
    return *m_logD;
}


LogInfo& Log::info( const std::string& toFile )
{
    m_stream_mutex.lock();
    if (m_logI == nullptr) {
        m_logI = new LogInfo;
    }
    m_logI->SetupFileOutput( toFile );
    m_logI->m_current_style = Styles::PLAIN_TEXT;
    return *m_logI;
}


LogWarning& Log::warning( const std::string& toFile )
{
    m_stream_mutex.lock();
    if (m_logW == nullptr) {
        m_logW = new LogWarning;
    }
    m_logW->SetupFileOutput( toFile );
    m_logW->m_current_style = Styles::PLAIN_TEXT;
    return *m_logW;
}


LogError& Log::error( const std::string& toFile )
{
    m_stream_mutex.lock();
    if (m_logE == nullptr) {
        m_logE = new LogError;
    }
    m_logE->SetupFileOutput( toFile );
    m_logE->m_current_style = Styles::PLAIN_TEXT;
    return *m_logE;
}

LogTest& Log::test( const std::string& toFile )
{
    m_stream_mutex.lock();
    if (m_logTest == nullptr) {
        m_logTest = new LogTest;
    }
    m_logTest->SetupFileOutput( toFile );
    m_logTest->m_current_style = Styles::HEADER;
    return *m_logTest;
}


void Log::SetStyle( const Styles& style )
{
    std::cout << "### SetStyle(const Styles&) is not implemented for this type ###" << std::endl;
}


std::string Log::Header() const
{
    std::cout << "### Header() is not implemented for this type ###" << std::endl; return std::string();
}


void Log::SetupFileOutput( const std::string fileName )
{
    m_outputToFile = false;
    if (fileName.empty())
        return;

    m_outputToFile = true;

    if (m_outputFile.is_open()) {
        m_outputFile.close();
    }
    m_outputFile.open( fileName, std::ios_base::out | std::ios_base::app );
    if (!m_outputFile.is_open())
    {
        m_outputToFile = false;
        error() << Styles::HEADER << "\nLogger Error: " << Styles::IMPORTANT_TEXT << "Can't open the file to output the log.\n";
    }
}


std::vector<std::string> Log::SplitString( const std::string& s, char delimiter )
{
    if (s.empty())
        return std::vector<std::string>();

    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream( s );

    while (std::getline( tokenStream, token, delimiter )) {
        tokens.push_back( token );
    }

    if (*(s.end()-1) == delimiter && s.size() != 1)
        tokens.push_back( "\n" );
    if (*s.begin() == '\n')
        tokens[0] = "\n";

    return tokens;
}