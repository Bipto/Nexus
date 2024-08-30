#version 450 core

layout(location = 0) in vec3 Position;

layout(location = 0) out vec3 OutTexCoord;

layout(binding = 0, set = 0) uniform Camera
{
    mat4 u_View;
    mat4 u_Projection;
};

void main()
{
    OutTexCoord = Position;
    gl_Position = u_Projection * u_View * vec4(Position, 1.0);
}