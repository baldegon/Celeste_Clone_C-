#version 430 core

// Input

// Output
location (location = 0) out vec2 textureCoordsOut;


void main()
{
    // generando los vertices en la GPU
    // mas que nada porque es un motor en 2D

    // Coordenadas de OpenGL
    // -1/1             1/1
    // -1/-1            1/-1 

    vec2 vertices[6] =
    {
        //top left
        vec2(-0.5, 0.5),
        //bottom left
        vec2(-0.5, -0.5),
        //top right
        vec2(0.5, 0.5),
        //top right
        vec2(0.5, 0.5),
        //bottom left
        vec2(-0.5, -0.5),
        //bottom right
        vec2(0.5, -0.5)
    };

    float left = 0.0;
    float top = 0.0;
    float right = 16;
    float bottom = 16;


    vec2 textureCoords[6] =
    {
        vec2(left, top),
        vec2(left, bottom),
        vec2(rigth, top),
        vec2(rigth, top),
        vec2(left, bottom),
        vec2(rigth, bottom);
    };

    gl_Position = vec4(vertices[gl_VertexID], 1.0, 1.0); 
    textureCoordsOut = textureCoords[gl_VertexID];
}