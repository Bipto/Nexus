#version 450 core

layout (location = 0) in vec2 OutTexCoord;
layout (location = 1) in vec3 OutNormal;
layout (location = 2) in vec3 FragPos;
layout (location = 3) in vec3 ViewPos;
layout (location = 4) in mat3 TBN;


layout (location = 0) out vec4 FragColor;

layout(binding = 0, set = 1) uniform sampler2D diffuseMapSampler;

void main()
{
	FragColor = texture(diffuseMapSampler, OutTexCoord);
}