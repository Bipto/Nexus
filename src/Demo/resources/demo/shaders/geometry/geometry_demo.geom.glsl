#version 450 core

layout(points) in;
layout(line_strip, max_vertices = 2) out;

layout(location = 0) in vec4 vertexPosition[];

void main()
{
	gl_Position = vertexPosition[0] + vec4(-0.1, 0.0, 0.0, 0.0);
	EmitVertex();

	gl_Position = vertexPosition[0] + vec4(0.1, 0.0, 0.0, 0.0);
	EmitVertex();

	EndPrimitive();
}