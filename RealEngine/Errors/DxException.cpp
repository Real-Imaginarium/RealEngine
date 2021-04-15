#include "DxException.h"

#include <comdef.h>


DxException::DxException( HRESULT hr, const std::wstring &func_name, const std::wstring &file_name, int line_number )
    : m_error_code( hr )
    , m_function_name( func_name )
    , m_file_name( file_name )
    , m_line_number( line_number )
{}


std::wstring DxException::ToString()const
{
    // Get the string description of the error code.
    _com_error err( m_error_code );
    std::wstring msg = err.ErrorMessage();

    return m_function_name + L" failed in " + m_file_name + L"\nline: " + std::to_wstring( m_line_number ) + L"\nerror: " + msg;
}