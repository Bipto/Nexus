#shader vertex
#version 450 core

layout (location = 0) in vec3 Position;

void main()
{
    gl_Position = vec4(Position, 1.0);
}

#shader fragment
#version 450 core

layout (location = 0) out vec4 FragColor;

void main()
{
    FragColor = vec4(0.15, 0.7, 0.3, 1.0);
}