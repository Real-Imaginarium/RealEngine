#include "LogInfo.h"
#include <Windows.h>


void LogInfo::SetStyle( const Styles& style )
{
    HANDLE hConsole = GetStdHandle( STD_OUTPUT_HANDLE );
    switch (style)
    {
    case Styles::HEADER: {
        SetConsoleTextAttribute( hConsole, (WORD)((Black << 4) | LightGreen) );
        break;
    }
    case Styles::PLAIN_TEXT: {
        SetConsoleTextAttribute( hConsole, (WORD)((Black << 4) | Green) );
        break;
    }
    case Styles::IMPORTANT_TEXT: {
        SetConsoleTextAttribute( hConsole, (WORD)((Black << 4) | LightCyan) );
        break;
    }
    case Styles::TEST_FAILED:
    case Styles::TEST_PASSED: {
        std::cout << "### Styles::TEST_FAILED and Styles::TEST_PASSED doesn't make sense for LogInfo ###" << std::endl;
        break;
    }
    }
}
