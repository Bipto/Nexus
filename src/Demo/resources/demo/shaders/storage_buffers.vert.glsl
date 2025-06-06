#version 450 core

layout(location = 0) in vec3 Position;
layout(location = 1) in vec2 TexCoord;

layout(location = 0) out vec2 OutTexCoord;

layout(std430, binding = 0, set = 0) readonly buffer TransformBuffer
{
	mat4 u_Transforms[];
};

void main()
{
	gl_Position = u_Transforms[0] * vec4(Position, 1.0);
	OutTexCoord = TexCoord;
}