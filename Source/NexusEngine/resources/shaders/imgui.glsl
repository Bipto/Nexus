#shader vertex
#version 450 core

layout (location = 0) in vec2 Position;
layout (location = 1) in vec2 TexCoord;
layout (location = 2) in vec4 Color;

layout (location = 0) out vec2 Frag_UV;
layout (location = 1) out vec4 Frag_Color;

layout (binding = 0, set = 0) uniform MVP
{
    mat4 u_MVP;
};

void main()
{
    gl_Position = u_MVP * vec4(Position, 0.0, 1.0);
    Frag_UV = vec2(TexCoord.x, TexCoord.y);
    Frag_Color = Color;
}

#shader fragment
#version 450 core

layout (location = 0) in vec2 Frag_UV;
layout (location = 1) in vec4 Frag_Color;

layout (set = 1, binding = 0) uniform sampler2D Texture;

layout (location = 0) out vec4 OutColor;

void main()
{
    OutColor = Frag_Color * texture(Texture, Frag_UV.st);
}