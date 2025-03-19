#include "clonceleste_lib.h"

//#############################################################
//                      Platform Globals
//#############################################################
static bool running = true;

//#############################################################
//                      Platform Functions
//#############################################################
bool platform_create_window(int width, int height, char* title);
void platform_update_window();

//#############################################################
//                      Windows Platforms
//#############################################################
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>


//#############################################################
//                      Windows Globals
//#############################################################
static HWND window;

//#############################################################
//                      Platform Implementations 
//#############################################################
LRESULT CALLBACK windows_window_callback(HWND window, UINT msg,
                                         WPARAM wParam, LPARAM lParam)
{
    LRESULT result = 0;

    switch (msg)
    {
        case WM_CLOSE:
        {
            running = false;
            break;

        }
        break;
    
        default:
        {
            result = DefWindowProcA(window, msg, wParam, lParam);
        }
        break;
    }
    return result;
}


bool platform_create_window(int width, int height, char* title)
{
    HINSTANCE instance = GetModuleHandleA(0);

    WNDCLASS wc = {};
    wc.hInstance = instance;
    wc.hIcon = LoadIcon(instance, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.lpszClassName = title;
    wc.lpfnWndProc = windows_window_callback; 

    if(!RegisterClass(&wc))
    {
        return false;
    }

    // WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX
    int dwStyle = WS_OVERLAPPEDWINDOW;

    window = CreateWindowExA(0, title,
                                    title,
                                    dwStyle,
                                    100,
                                    100,
                                    width,
                                    height,
                                    NULL,   // parent
                                    NULL,   // menu
                                    instance,
                                    NULL);

    if(window == NULL){
        return false;
    }

    ShowWindow(window, SW_SHOW);
    
    return true;
}

void platform_update_window()
{
 MSG msg;

 while (PeekMessage(&msg, window, 0, 0, PM_REMOVE))
 {
    TranslateMessage(&msg);
    DispatchMessage(&msg); // Calls the callback specified when creating the window
 }
 
}



#endif


int main()
{
    platform_create_window(1200, 720, "Baldegon Motor");
    
    while(running)
    {
        // Update
        platform_update_window();
        
        SM_TRACE("test");
        SM_WARN("test");
        SM_ERROR("test");
        //SM_ASSERT(false, "Assertion not Hit!");B
    }

    return 0;
}