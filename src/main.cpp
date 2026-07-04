#include "clonceleste_lib.h"

#include "platform.h"

#define WIN32_LEAN_AND_MEAN
#define APIENTRY
#include "glcorearb.h"

#ifdef _WIN32
#include "win32_platform.cpp"
#endif

#include "gl_renderer.h"

int main()
{
    platform_create_window(1200, 720, "Baldegon Motor");
    
    while(running)
    {
        // Update
        platform_update_window();
    }

    return 0;
}