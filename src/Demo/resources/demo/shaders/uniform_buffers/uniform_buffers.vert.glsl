#version 450 core

layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec2 a_TexCoord;

layout (location = 0) out vec2 o_TexCoord;

layout (std140,binding = 0, set = 0) uniform Transform
{
    mat4 u_Transform;
};

void main()
{
    gl_Position = u_Transform * vec4(a_Position, 1.0);
    o_TexCoord = a_TexCoord;
}