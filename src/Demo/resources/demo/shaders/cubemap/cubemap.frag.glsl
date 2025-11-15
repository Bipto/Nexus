#version 450 core

layout(location = 0) in vec3 a_TexCoord;

layout(location = 0) out vec4 o_Colour;

layout(binding = 0, set = 1) uniform samplerCube u_Skybox;

void main()
{
    o_Colour = texture(u_Skybox, a_TexCoord);
}