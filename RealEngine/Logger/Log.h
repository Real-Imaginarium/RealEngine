#pragma once

#include <type_traits>
#include <iostream>
#include <ostream>
#include <fstream>
#include <sstream>
#include <mutex>
#include <cstdint>
#include <filesystem>


class LogDebug;
class LogInfo;
class LogWarning;
class LogError;
class LogTest;
class LogTestStart;
class LogTestRun;
class LogTestPass;
class LogTestFail;

enum class Styles : uint8_t
{
    HEADER,
    PLAIN_TEXT,
    IMPORTANT_TEXT,
    TEST_PASSED,
    TEST_FAILED
};

enum ConsoleColor {
    Black = 0,
    Blue = 1,
    Green = 2,
    Cyan = 3,
    Red = 4,
    Magenta = 5,
    Brown = 6,
    LightGray = 7,
    DarkGray = 8,
    LightBlue = 9,
    LightGreen = 10,
    LightCyan = 11,
    LightRed = 12,
    LightMagenta = 13,
    Yellow = 14,
    White = 15
};


class Log
{
public:
    struct endlog {};

    template<typename T>
    Log& operator<<( const T &l )
    {
        std::stringstream sstream;
        sstream << l;

        std::vector<std::string> strvec = SplitString( sstream.str(), '\n' );

        for (size_t i = 0; i < strvec.size(); ++i)
        {
            if (m_needNewLine) {
                std::cout << "\n";
                m_needNewLine = false;
            }
            if (!m_header_sended)
            {
                SetStyle( Styles::HEADER );
                std::cout << Header() << " ";
                SetStyle( m_current_style );
                if (m_outputToFile && m_outputFile.is_open()) {
                    m_outputFile << Header() << " ";
                }
            }
            if (i == strvec.size() - 1 && strvec[i] == "\n")
            {
                if (strvec.size() == 1)
                {
                    std::cout << strvec[i];
                    if (m_outputToFile && m_outputFile.is_open()) {
                        m_outputFile << strvec[i];
                    }
                    m_header_sended = false;
                }
                else {
                    m_header_sended = true;
                }
            }
            else
            {
                if (strvec.size() > 1) {
                    m_header_sended = false;
                }
                else {
                    m_header_sended = true;
                }

                std::cout << strvec[i];
                if (m_outputToFile && m_outputFile.is_open()) {
                    m_outputFile << strvec[i];
                }
                if (strvec[i] != "\n" && strvec.size() > 1)
                {
                    if (i != strvec.size() - 1)
                    {
                        std::cout << "\n";
                        if (m_outputToFile && m_outputFile.is_open()) {
                            m_outputFile << '\n';
                        }
                    }
                    else {
                        m_header_sended = true;
                    }
                }
            }
        }
        return *this;
    }

    Log& operator<<( const Styles& st );

    virtual void operator<<( const endlog&);

    static LogDebug& debug( const std::string& toFile = "" );

    static LogInfo& info( const std::string& toFile = "" );

    static LogWarning& warning( const std::string& toFile = "" );

    static LogError& error( const std::string& toFile = "" );

    static LogTest& test( const std::string& toFile = "" );
    
    Styles m_current_style;
protected:
    virtual void SetStyle( const Styles& style );

    virtual std::string Header() const;

    void SetupFileOutput( const std::string fileName );

    static LogDebug *m_logD;
    static LogInfo *m_logI;
    static LogWarning *m_logW;
    static LogError *m_logE;
    static LogTest *m_logTest;

    static bool m_needNewLine;
    
    std::stringstream m_sstream;
    std::ofstream m_outputFile;
    bool m_outputToFile = false;
    bool m_header_sended = false;
    std::string m_trace;
    static std::recursive_mutex m_stream_mutex;

    std::vector<std::string> SplitString( const std::string& s, char delimiter );
};