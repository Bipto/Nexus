#version 450 core

layout (location = 0) in vec2 OutTexCoord;

layout (location = 0) out vec4 FragColor;

layout (binding = 0, set = 1) uniform sampler2D texSampler;

void main()
{
    FragColor = texture(texSampler, OutTexCoord);
}