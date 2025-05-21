#version 450 core

layout(location = 0) in vec2 Position;
layout(location = 0) out vec4 vertexPosition;

void main()
{
	gl_Position	   = vec4(Position, 1.0, 1.0);
	vertexPosition = gl_Position;
}