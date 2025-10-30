#version 450 core

layout(location = 0) in vec3 a_Position;

layout(location = 0) out vec3 o_TexCoord;

layout(binding = 0, set = 0) uniform Camera
{
    mat4 u_View;
    mat4 u_Projection;
};

void main()
{
    o_TexCoord = a_Position;
    gl_Position = u_Projection * u_View * vec4(a_Position, 1.0);
}