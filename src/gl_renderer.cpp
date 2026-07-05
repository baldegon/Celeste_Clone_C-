#include "gl_renderer.h"
#include "clonceleste_lib.h"

//#######
// OpenGL Structs
//#######
struct GLContext
{
    GLuint programID;
};

//#######
// OpenGL Globals
//#######
static GLContext glContext;

//#######
// OpenGL Functions
//#######
static void APIENTRY gl_debug_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* user)
{
    if(severity == GL_DEBUG_SEVERITY_LOW ||
       severity == GL_DEBUG_SEVERITY_MEDIUM ||
       severity == GL_DEBUG_SEVERITY_HIGH)
    {
        SM_ASSERT(false, "OpenGL Error: %s", message);
    }
    else
    {
        SM_TRACE((char*)message)
    }
}

bool gl_init(BumpAllocator* transientStorage)
{
    load_gl_functions();

    glDebugMessageCallback(&gl_debug_callback, nullptr);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glEnable(GL_DEBUG_OUTPUT);

    GLuint vertShaderID = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragShaderID = glCreateShader(GL_FRAGMENT_SHADER);

    int fileSize = 0;
    char* vertShader = read_file("assets/shaders/quad.vert", &fileSize, transientStorage);
    char* fragShader = read_file("assets/shaders/quad.frag", &fileSize, transientStorage);

    if(!vertShader || !fragShader)
    {
        SM_ASSERT(false, "Failed to load shaders");
        return false;
    }


    return true;
}
