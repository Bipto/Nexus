#version 450 core

layout (location = 0) in vec3 Position;
layout (location = 1) in vec2 TexCoord;

layout (location = 0) out vec2 OutTexCoord;

layout (std140,binding = 0, set = 0) uniform Transform
{
    mat4 u_Transform;
};

void main()
{
    gl_Position = u_Transform * vec4(Position, 1.0);
    OutTexCoord = TexCoord;
}