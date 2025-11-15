#version 450 core

layout (location = 0) in vec2 a_TexCoord;
layout (location = 1) in vec3 a_Normal;
layout (location = 2) in vec3 a_FragPos;
layout (location = 3) in vec3 a_ViewPos;
layout (location = 4) in mat3 a_TBN;


layout (location = 0) out vec4 o_Colour;

layout(binding = 0, set = 1) uniform sampler2D u_DiffuseMap;

void main()
{
	o_Colour = texture(u_DiffuseMap, a_TexCoord);
}