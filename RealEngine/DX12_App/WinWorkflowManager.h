#pragma once

#include <functional>
#include <Windows.h>


class WinWorkflowManager
{
public:
    WinWorkflowManager( HINSTANCE inst,
                        std::function<void( bool )> win_activation_cb,
                        std::function<void( size_t, size_t )> win_start_resizing_cb,
                        std::function<void( size_t, size_t )> win_stop_resizing_cb,
                        std::function<void( size_t, size_t, bool )> win_resized_cb,
                        std::function<void( size_t, size_t )> win_resized_by_api_cb,
                        std::function<void( bool, size_t, size_t )> mouse_toggle_cb,
                        std::function<void( size_t, size_t )> mouse_move_cb,
                        std::function<void( size_t )> key_up_cb );

    bool Initialize();

    bool Deinitialize();

    size_t Width() const { return m_window_width; }

    size_t Height() const { return m_window_height; }

    bool IsWindowed() const { return m_windowed_mode; }

    HWND GetWindowHandler() const { return m_hwnd; }

    void SetMainWindowText( const char *text );

private:
    static LRESULT CALLBACK MsgProc( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam );
    
    static WinWorkflowManager *m_wwm;
    
    HWND m_hwnd;
    HINSTANCE m_hinst;
    std::string m_window_caption;
    size_t m_window_width;
    size_t m_window_height;
    bool m_windowed_mode;
    bool m_window_maximized;
    bool m_window_minimized;
    bool m_resizing;

    std::function<void( bool )> m_window_activation_cb;
    std::function<void( size_t, size_t )> m_window_start_resizing_cb;
    std::function<void( size_t, size_t )> m_window_stop_resizing_cb;
    std::function<void( size_t, size_t, bool )> m_window_resized_cb;
    std::function<void( size_t, size_t )> m_window_resized_by_api_cb;
    std::function<void( bool, size_t, size_t )> m_mouse_toggle_cb;
    std::function<void( size_t, size_t )> m_mouse_move_cb;
    std::function<void( size_t )> m_key_up_cb;
};

