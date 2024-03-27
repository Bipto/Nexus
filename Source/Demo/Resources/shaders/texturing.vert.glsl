#version 450 core

layout (location = 0) in vec3 Position;
layout (location = 1) in vec2 TexCoord;

layout (location = 0) out vec2 OutTexCoord;



void main()
{
    gl_Position = vec4(Position, 1.0);
    OutTexCoord = TexCoord;
}