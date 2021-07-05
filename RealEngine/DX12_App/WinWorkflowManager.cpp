#include "WinWorkflowManager.h"

#include "LogError.h"
#include "LogInfo.h"

#include <cassert>
#include <windowsx.h>

WinWorkflowManager *WinWorkflowManager::m_wwm = nullptr;



WinWorkflowManager::WinWorkflowManager( HINSTANCE inst, 
                                        std::function<void( bool )> win_activation_cb, 
                                        std::function<void( size_t, size_t )> win_start_resizing_cb,
                                        std::function<void( size_t, size_t )> win_stop_resizing_cb,
                                        std::function<void( size_t, size_t, bool )> win_resized_cb,
                                        std::function<void( size_t, size_t )> win_resized_by_api_cb,
                                        std::function<void( bool, size_t, size_t )> mouse_toggle_cb,
                                        std::function<void( size_t, size_t )> mouse_move_cb,
                                        std::function<void( size_t )> key_up_cb )
    : m_hwnd( nullptr )
    , m_hinst( inst )
    , m_window_caption( "DX12 Application" )
    , m_window_width( 1024 )
    , m_window_height( 768 )
    , m_windowed_mode( true )
    , m_window_maximized( false )
    , m_window_minimized( false )
    , m_resizing( false )
    , m_window_activation_cb( win_activation_cb )
    , m_window_start_resizing_cb( win_start_resizing_cb )
    , m_window_stop_resizing_cb( win_stop_resizing_cb )
    , m_window_resized_cb( win_resized_cb )
    , m_window_resized_by_api_cb( win_resized_by_api_cb )
    , m_mouse_toggle_cb( mouse_toggle_cb )
    , m_mouse_move_cb( mouse_move_cb )
    , m_key_up_cb( key_up_cb )
{
    assert( m_wwm == nullptr && "There can be only one WinWorkflowManager" );
    m_wwm = this;
}


bool WinWorkflowManager::Initialize()
{
    WNDCLASS wc;
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = MsgProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = m_hinst;
    wc.hIcon = LoadIcon( 0, IDI_APPLICATION );
    wc.hCursor = LoadCursor( 0, IDC_ARROW );
    wc.hbrBackground = (HBRUSH)GetStockObject( NULL_BRUSH );
    wc.lpszMenuName = 0;
    wc.lpszClassName = L"MainWnd";

    if( !RegisterClass( &wc ) )
    {
        Log::error() << "[WinWorkflowManager] RegisterClass Failed" << Log::endlog{};
        return false;
    }
    // Compute window rectangle dimensions based on requested client area dimensions.
    RECT R = { 0, 0, static_cast<LONG>(m_window_width), static_cast<LONG>(m_window_height) };
    AdjustWindowRect( &R, WS_OVERLAPPEDWINDOW, false );
    int width = R.right - R.left;
    int height = R.bottom - R.top;

    m_hwnd = CreateWindowA( "MainWnd", m_window_caption.c_str(), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, width, height, 0, 0, m_hinst, 0 );
    if( !m_hwnd )
    {
        Log::error() << "[WinWorkflowManager] CreateWindow Failed" << Log::endlog{};
        return false;
    }
    ShowWindow( m_hwnd, SW_SHOW );
    UpdateWindow( m_hwnd );
    return true;
}


bool WinWorkflowManager::Deinitialize()
{
    return true;
}


void WinWorkflowManager::SetMainWindowText( const char *text )
{
    SetWindowTextA( m_hwnd, (m_window_caption + text).c_str());
}


LRESULT WinWorkflowManager::MsgProc( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
    switch( msg )
    {
    case WM_ACTIVATE: {
        m_wwm->m_window_activation_cb( LOWORD( wParam ) != WA_INACTIVE );
        return 0;
    }
    case WM_SIZE: {
        m_wwm->m_window_width = LOWORD( lParam );
        m_wwm->m_window_height = HIWORD( lParam );
        
        if( wParam == SIZE_MINIMIZED )
        {
            m_wwm->m_window_resized_cb( m_wwm->m_window_width, m_wwm->m_window_height, false );
            m_wwm->m_window_minimized = true;
            m_wwm->m_window_maximized = false;
        }
        else if( wParam == SIZE_MAXIMIZED )
        {
            m_wwm->m_window_resized_cb( m_wwm->m_window_width, m_wwm->m_window_height, true );
            m_wwm->m_window_minimized = false;
            m_wwm->m_window_maximized = true;
        }
        else if( wParam == SIZE_RESTORED )
        {
            // Restoring from minimized state?
            if( m_wwm->m_window_minimized )
            {
                m_wwm->m_window_minimized = false;
                m_wwm->m_window_resized_cb( m_wwm->m_window_width, m_wwm->m_window_height, true );
            }
            // Restoring from maximized state?
            else if( m_wwm->m_window_maximized )
            {
                m_wwm->m_window_maximized = false;
                m_wwm->m_window_resized_cb( m_wwm->m_window_width, m_wwm->m_window_height, true );
            }
            else if( m_wwm->m_resizing )
            {
                // If user is dragging the resize bars, we do not resize the buffers here because as the user continuously drags the resize bars, a stream of WM_SIZE messages are
                // sent to the window, and it would be pointless (and slow) to resize for each WM_SIZE message received from dragging the resize bars.  So instead, we reset after
                // the user is done resizing the window and releases the resize bars, which sends a WM_EXITSIZEMOVE message.
            }
            else // API call such as SetWindowPos or mSwapChain->SetFullscreenState.
            {
                m_wwm->m_window_resized_by_api_cb( m_wwm->m_window_width, m_wwm->m_window_height );
            }
        }
        return 0;
    }
    case WM_ENTERSIZEMOVE: {
        m_wwm->m_resizing = true;
        m_wwm->m_window_start_resizing_cb( m_wwm->m_window_width, m_wwm->m_window_height );
        return 0;
    }
    case WM_EXITSIZEMOVE: {
        m_wwm->m_resizing = false;
        m_wwm->m_window_stop_resizing_cb( m_wwm->m_window_width, m_wwm->m_window_height );
        return 0;
    }
    case WM_DESTROY: {
        PostQuitMessage( 0 );
        return 0;
    }
    case WM_MENUCHAR: {
        return MAKELRESULT( 0, MNC_CLOSE );     // Don't beep when we alt-enter.
    }
    // Catch this message so to prevent the window from becoming too small.
    case WM_GETMINMAXINFO: {
        ( (MINMAXINFO *)lParam )->ptMinTrackSize.x = 200;
        ( (MINMAXINFO *)lParam )->ptMinTrackSize.y = 200;
        return 0;
    }
    case WM_LBUTTONDOWN:
    case WM_MBUTTONDOWN:
    case WM_RBUTTONDOWN: {
        m_wwm->m_mouse_toggle_cb( true, GET_X_LPARAM( lParam ), GET_Y_LPARAM( lParam ) );
        return 0;
    }
    case WM_LBUTTONUP:
    case WM_MBUTTONUP:
    case WM_RBUTTONUP: {
        m_wwm->m_mouse_toggle_cb( false, GET_X_LPARAM( lParam ), GET_Y_LPARAM( lParam ) );
        return 0;
    }
    case WM_MOUSEMOVE: {
        m_wwm->m_mouse_move_cb( GET_X_LPARAM( lParam ), GET_Y_LPARAM( lParam ) );
        return 0;
    }
    case WM_KEYUP: {
        if( wParam == VK_ESCAPE ) {
            PostQuitMessage( 0 );
        }
        else {
            m_wwm->m_key_up_cb( static_cast<size_t>( wParam ) );
        }
        return 0;
    }
    }
    return DefWindowProc( hwnd, msg, wParam, lParam );
}