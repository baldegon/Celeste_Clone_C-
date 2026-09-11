#version 430 core

// Structs
struct Transform
{
    ivec2 atlasOffset;
    ivec2 spriteSize;
    vec2 pos;
    vec2 size;
};

// Input
layout (std430, binding = 0) buffer TransformSBO
{
    Transform transforms[];
};

uniform vec2 screenSize;
uniform mat4 orthoProjection;

// Output
layout (location = 0) out vec2 textureCoordsOut;


void main()
{
    Transform transform = transforms[gl_InstanceID];

    // generando los vertices en la GPU
    // mas que nada porque es un motor en 2D

    // Coordenadas de OpenGL
    // -1/1             1/1
    // -1/-1            1/-1 

    vec2 vertices[6] =
    {
        transform.pos,                                          // Top left
        vec2(transform.pos + vec2(0.0, transform.size.y)),      // bottom left
        vec2(transform.pos + vec2(transform.size.x, 0.0)),      // Top right  
        vec2(transform.pos + vec2(transform.size.x, 0.0)),      // top right
        vec2(transform.pos + vec2(0.0, transform.size.y)),      // bottom left
        transform.pos + transform.size                          // bottom right
    };

    float left = transform.atlasOffset.x;
    float top = transform.atlasOffset.y;
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

    //Normalized Position
    {
        vec2 vertexPos = vertices[gl_VertexID];
        // vertexPos.y = -vertexPos.y + screenSize.y;
        // vertexPos = 2.0 * (vertexPos / screenSize) - 1.0;
        gl_Position = orthoProjection * vec4(vertexPos, 0.0, 1.0);
    }


    textureCoordsOut = textureCoords[gl_VertexID];
}