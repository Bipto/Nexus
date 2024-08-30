#version 450 core

layout(location = 0) in vec3 Position;
layout(location = 1) in vec2 TexCoord;

layout(location = 0) out vec2 OutTexCoord;

layout(binding = 0, set = 0) uniform Camera
{
    mat4 u_View;
    mat4 u_Projection;
};

void main()
{
    gl_Position = u_Projection * u_View * vec4(Position, 1.0);
    OutTexCoord = TexCoord;
}