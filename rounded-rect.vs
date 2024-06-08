#version 450 core

layout(location = 0) in vec3 Position;
layout(location = 1) in vec2 TexCoord;
layout(location = 2) in vec4 Color;
layout(location = 3) in float TexIndex;

layout(location = 0) out vec2 texCoord;
layout(location = 1) out vec4 outColor;
layout(location = 2) out flat float texIndex;

layout(binding = 0, set = 0) uniform MVP
{
    mat4 u_MVP;
};

void main()
{
    gl_Position = u_MVP * vec4(Position, 1.0);
    texCoord = TexCoord;
    outColor = Color;
    texIndex = TexIndex;
}";