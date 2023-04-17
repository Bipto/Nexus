#shader vertex
#version 450 core

layout (location = 0) in vec3 Position;
layout (location = 1) in vec2 TexCoord;

layout (location = 0) out vec2 OutTexCoord;

void main()
{
    gl_Position = vec4(Position, 1.0);
    OutTexCoord = TexCoord;
}

#shader fragment
#version 450 core

layout (location = 0) in vec2 OutTexCoord;
layout (location = 0) out vec4 FragColor;

layout (binding = 1) uniform sampler2D texSampler;

void main()
{
    //FragColor = vec4(0.2, 0.15, 0.9, 1);
    FragColor = texture(texSampler, OutTexCoord);
}