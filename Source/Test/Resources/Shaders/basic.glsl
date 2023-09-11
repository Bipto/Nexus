#shader vertex
#version 450 core

layout (location = 0) in vec3 Position;
layout (location = 1) in vec2 TexCoord;
layout (location = 2) in vec3 Normal;
layout (location = 3) in vec3 Tangent;
layout (location = 4) in vec3 Bitangent;

layout (location = 0) out vec2 outTexCoord;

layout (set = 0, binding = 0) uniform Transform
{
    mat4 u_Transform;
};

void main()
{
    gl_Position = u_Transform * vec4(Position, 1.0);
    outTexCoord = TexCoord;
}

#shader fragment
#version 450 core

layout (location = 0) out vec4 FragColor;
layout (location = 0) in vec2 outTexCoord;

layout (set = 1, binding = 0) uniform sampler2D texSampler;

void main()
{
    //FragColor = vec4(0.15, 0.7, 0.3, 1.0);
    vec3 color = texture(texSampler, outTexCoord).xyz;
    FragColor = vec4(color, 1.0);
}