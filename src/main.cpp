#include "clonceleste_lib.h"

#include "platform.h"

#define WIN32_LEAN_AND_MEAN
#define APIENTRY
#define GL_GLEXT_PROTOTYPES
#include "glcorearb.h"

#ifdef _WIN32
#include "win32_platform.cpp"
#endif

#include "gl_renderer.cpp"

int main()
{
    BumpAllocator transientStorage = make_bump_allocator(MB(50)); // 10 MB

    platform_create_window(1200, 720, "Baldegon Motor");
    
    gl_init(&transientStorage);

    while(running)
    {
        // Update
        platform_update_window();
    }

    return 0;
}