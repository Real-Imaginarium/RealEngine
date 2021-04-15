#pragma once

#include <string>

#include <Windows.h>

class DxException
{
public:
    DxException() = default;

    DxException( HRESULT hr, const std::wstring & functionName, const std::wstring & filename, int lineNumber );

    std::wstring ToString() const;

    HRESULT         m_error_code = S_OK;
    std::wstring    m_function_name;
    std::wstring    m_file_name;
    int             m_line_number = -1;
};


inline std::wstring AnsiToWString( const std::string &str )
{
    WCHAR buffer[512];
    MultiByteToWideChar( CP_ACP, 0, str.c_str(), -1, buffer, 512 );
    return std::wstring( buffer );
}


#ifndef THROW_ON_FAIL
#define THROW_ON_FAIL(x)                                                                    \
{                                                                                           \
    HRESULT hr__ = (x);                                                                     \
    WCHAR buffer[512];                                                                      \
    MultiByteToWideChar( CP_ACP, 0, __FILE__, -1, buffer, 512 );                            \
    if(FAILED(hr__)) { throw DxException(hr__, L#x, std::wstring( buffer ), __LINE__); }    \
}
#endif


#ifndef THROW_ON_FALSE
#define THROW_ON_FALSE(x)                                                                   \
{                                                                                           \
    bool res = x;                                                                           \
    WCHAR buffer[512];                                                                      \
    MultiByteToWideChar( CP_ACP, 0, __FILE__, -1, buffer, 512 );                            \
    if(!res) { throw DxException(E_FAIL, L#x, std::wstring( buffer ), __LINE__); }          \
}
#endif
