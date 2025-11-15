#version 450 core

layout (location = 0) in vec2 a_TexCoord;

layout (location = 0) out vec4 o_Colour;

layout (binding = 0, set = 0) uniform sampler2D u_Texture;

void main()
{
    o_Colour = texture(u_Texture, a_TexCoord);
}