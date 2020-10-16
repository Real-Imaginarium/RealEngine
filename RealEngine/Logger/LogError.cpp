#include "LogError.h"
#include <Windows.h>


void LogError::SetStyle( const Styles& style )
{
    HANDLE hConsole = GetStdHandle( STD_OUTPUT_HANDLE );
    switch (style)
    {
    case Styles::HEADER: {
        SetConsoleTextAttribute( hConsole, (WORD)((Black << 4) | Red) );
        break;
    }
    case Styles::PLAIN_TEXT: {
        SetConsoleTextAttribute( hConsole, (WORD)((Black << 4) | LightRed) );
        break;
    }
    case Styles::IMPORTANT_TEXT: {
        SetConsoleTextAttribute( hConsole, (WORD)((Black << 4) | LightMagenta) );
        break;
    }
    case Styles::TEST_FAILED:
    case Styles::TEST_PASSED: {
        std::cout << "### Styles::TEST_FAILED and Styles::TEST_PASSED doesn't make sense for LogError ###" << std::endl;
        break;
    }
    }
}
