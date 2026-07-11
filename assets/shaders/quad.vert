#version 430 core

// Structs
struct Transform
{
    ivec2 atlasOffset;
    ivec2 spriteSize;
};

// Input
layout(std430, binding = 0) buffer TransformSBO
{
    Transform transforms[];
};

// Output
layout (location = 0) out vec2 textureCoordsOut;


void main()
{
    Transform transform = transforms[gl_instanceID];

    // generando los vertices en la GPU
    // mas que nada porque es un motor en 2D

    // Coordenadas de OpenGL
    // -1/1             1/1
    // -1/-1            1/-1 

    vec2 vertices[6] =
    {
        transform.pos,
        vec2(transform.pos + vec2(0.0, transform.size.y)),
        vec2(transform.pos + vec2(transform.size.x, 0.0)),
        vec2(transform.pos + vec2(transform.size.x, 0.0)),
        vec2(transform.pos + vec2(0.0, transform.size.y)),
        transform.pos + transform.size
    };

    float left = transform.atlasOffset.x;
    float top = transformn.atlasOffset.y;
    float right = transform.atlasOffset.x + transform.spriteSize.x;
    float bottom = transform.atlasOffset.y + transform.spriteSize.y;


    vec2 textureCoords[6] =
    {
        vec2(left, top),
        vec2(left, bottom),
        vec2(right, top),
        vec2(right, top),
        vec2(left, bottom),
        vec2(right, bottom),
    };

    gl_Position = vec4(vertices[gl_VertexID], 1.0, 1.0); 
    textureCoordsOut = textureCoords[gl_VertexID];
}