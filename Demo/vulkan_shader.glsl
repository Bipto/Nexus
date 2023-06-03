#shader vertex
#version 450

layout (location = 0) in vec2 inPosition;

void main()
{
    gl_Position = vec4(inPosition, 0.0f, 1.0f);
}

#shader fragment
#version 450

layout (location = 0) out vec4 outFragColor;

void main()
{
    outFragColor = vec4(0.0f, 0.0f, 1.0f, 1.0f);
}