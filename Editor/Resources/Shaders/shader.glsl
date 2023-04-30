#shader vertex
#version 450 core

layout (location = 0) in vec3 Position;
layout (location = 1) in vec2 TexCoord;

layout (std140, binding = 0) uniform RenderInfo
{
    mat4 u_Transform;
    vec3 u_Color;
};

layout (std140, binding = 1) uniform Camera
{
    mat4 u_View;
    mat4 u_Projection;
};

layout (location = 0) out vec2 OutTexCoord;
layout (location = 1) out vec3 OutColor;

void main()
{
    gl_Position = vec4(Position, 1.0) * u_Transform * u_View * u_Projection;
    OutTexCoord = TexCoord;
    OutColor = u_Color;
}

#shader fragment
#version 450 core

layout (location = 0) in vec2 OutTexCoord;
layout (location = 1) in vec3 OutColor;

layout (location = 0) out vec4 FragColor;
layout (binding = 0) uniform sampler2D texSampler;

void main()
{
    FragColor = texture(texSampler, OutTexCoord) * vec4(OutColor, 1.0);
}