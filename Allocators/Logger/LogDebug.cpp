#include "LogDebug.h"
#include <Windows.h>


void LogDebug::SetStyle( const Styles& style )
{
    HANDLE hConsole = GetStdHandle( STD_OUTPUT_HANDLE );
    switch (style)
    {
    case Styles::HEADER: {
        SetConsoleTextAttribute( hConsole, (WORD)((Black << 4) | White) );
        break;
    }
    case Styles::PLAIN_TEXT: {
        SetConsoleTextAttribute( hConsole, (WORD)((Black << 4) | DarkGray) );
        break;
    }
    case Styles::IMPORTANT_TEXT: {
        SetConsoleTextAttribute( hConsole, (WORD)((Black << 4) | LightGray) );
        break;
    }
    case Styles::TEST_FAILED:
    case Styles::TEST_PASSED: {
        std::cout << "### Styles::TEST_FAILED and Styles::TEST_PASSED doesn't make sense for LogDebug ###" << std::endl;
        break;
    }
    }
}
