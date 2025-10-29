#version 450 core

layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec2 a_TexCoord;

layout (location = 0) out vec2 o_TexCoord;

void main()
{
    gl_Position = vec4(a_Position, 1.0);
    o_TexCoord = a_TexCoord;
}