#shader vertex
#version 450 core

layout (location = 0) in vec3 Position;
layout (location = 1) in vec2 TexCoord;

layout (location = 0) out vec2 OutTexCoord;

layout (binding = 0, set = 0) uniform Camera
{
    mat4 u_View;
    mat4 u_Projection;
};

layout (binding = 1, set = 0) uniform Transform
{
    mat4 u_Transform;
};

void main()
{
    gl_Position = u_Projection * u_View * u_Transform * vec4(Position, 1.0);
    OutTexCoord = TexCoord;
}

#shader fragment
#version 450 core

layout (location = 0) in vec2 OutTexCoord;

layout (location = 0) out vec4 FragColor;

layout (binding = 0, set = 1) uniform sampler2D texSampler;

void main()
{
    FragColor = texture(texSampler, OutTexCoord);
}