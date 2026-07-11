#include "gl_renderer.h"
#include "clonceleste_lib.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "render_interface.h"

//#######
// OpenGL Structs
//#######
const char* TEXTURE_PATH = "assets/textures/TEXTURE_ATLAS.PNG";


//#######
// OpenGL Structs
//#######
struct GLContext
{
    GLuint programID;
    GLuint textureID;
    GLuint transformSBOID;
};

//#######
// OpenGL Globals
//#######
static GLContext glContext;

//#######
// OpenGL Functions
//#######
static void APIENTRY gl_debug_callback(GLenum source, GLenum type, GLuint id, GLenum severity,      GLsizei length, const GLchar* message, const void* user)
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
    };
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

    glShaderSource(vertShaderID, 1, &vertShader, 0);
    glShaderSource(fragShaderID, 1, &fragShader, 0);

    glCompileShader(vertShaderID);
    glCompileShader(fragShaderID);

    // Testea si el vertex shader compilaron correctamente
    {
        int success;
        char shaderLog[2048] = {};

        glGetShaderiv(vertShaderID, GL_COMPILE_STATUS, &success);
        if(!success){
            glGetShaderInfoLog(vertShaderID, 2048, 0, shaderLog);
            SM_ASSERT(false, "failed to compile vertex shader %s", shaderLog);
        }
    }


    {
        int success;
        char shaderLog[2048] = {};

        glGetShaderiv(fragShaderID, GL_COMPILE_STATUS, &success);
        if(!success){
            glGetShaderInfoLog(fragShaderID, 2048, 0, shaderLog);
            SM_ASSERT(false, "failed to compile fragment shader %s", shaderLog);
        }
    }

    glContext.programID = glCreateProgram();
    glAttachShader(glContext.programID, vertShaderID);
    glAttachShader(glContext.programID, fragShaderID);
    glLinkProgram(glContext.programID);

    glDetachShader(glContext.programID, vertShaderID);
    glDetachShader(glContext.programID, fragShaderID);
    glDeleteShader(vertShaderID);
    glDeleteShader(fragShaderID);

    // Esto tiene que ser asi, porque sino OpenGL no va a dibujar nada
    GLuint VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // Carga de texturas usando STBI
    {
        int width, height, channels;
        char* data = (char*)stbi_load(TEXTURE_PATH , &width, &height, &channels, 4);
        if(!data)
        {
            SM_ASSERT(false, "Failed to load texture");
            return false;
        }

        glGenTextures(1, &glContext.textureID);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, glContext.textureID);

        // Setea la textura para wrapping/filtros y opciones(en la ligadura actual de la textura del objeto)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        // este seteo solo funciona cuando se esta usando la funcion texture() en GLSL
        // cuando se usa texelFetch() esta funcion no tiene efecto
        // porque texelFetch fue diseñado para este proposito
        // documentacion: 
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8_ALPHA8, width, height,
                        0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        stbi_image_free(data);
    }


    //Transform storage buffer
    {
        glGenBuffers(1, &glContext.transformSBOID);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, glContext.transformSBOID);
        glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Transform) * MAX_TRANSFORMS,
                     renderData.transforms, GL_DYNAMIC_DRAW);
    }

    // sRGB output (even if input texture is non-sRGB -> don't rely on texture used)
    // Your font is not using sRGB, for example (not that it matters there, because no actual color is sampled from it)
    // But this could prevent some future bug when you start mixing different types of textures
    // Of course, you still need to correctly set the image file source format when using glTexImage2D()
    glEnable(GL_FRAMEBUFFER_SRGB);
    glDisable(0x809D); // disable multisampling


    //Testeo de Profundidad
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_GREATER);

    glUseProgram(glContext.programID);

    return true;
}



void gl_render()
{
    glClearColor(119.0f / 255.0f, 33.0f / 255.0f, 111.0f / 255.0f, 1.0f);
    glClearDepth(0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, input.screenSizeX, input.screenSizeY);

    // opaca objetos
    {
        //copia Transforms a la GPU
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(Transform) * renderData.transformCount,
                         renderData.transforms);

        glDrawArraysInstanced(GL_TRIANGLES, 0, 6, renderData.transformCount);

        //resetea para el siguiente frame
        renderData.transformCount = 0;
    }
}