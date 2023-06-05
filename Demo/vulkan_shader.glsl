#shader vertex
#version 450

layout (location = 0) in vec2 inPosition;

layout (binding = 0) uniform CameraBuffer
{
    mat4 MVP;
} cameraBuffer;

void main()
{
    gl_Position = vec4(inPosition, 0.0f, 1.0f);
}

#shader fragment
#version 450

layout (location = 0) out vec4 outFragColor;

void main()
{
    outFragColor = vec4(0.3f, 0.2f, 0.8f, 1.0f);
}