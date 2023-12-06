#shader vertex
#version 450 core

layout (location = 0) in vec3 Position;
layout (location = 1) in vec2 TexCoord;
layout (location = 2) in vec3 Normal;
layout (location = 3) in vec3 Tangent;
layout (location = 4) in vec3 Bitangent;

layout (location = 0) out vec2 texCoord;

layout (set = 0, binding = 0) uniform Transform
{
    mat4 u_Transform;
};

void main()
{
    gl_Position = u_Transform * vec4(Position, 1.0);
    texCoord = TexCoord;
}

#shader fragment
#version 450 core

layout (location = 0) out vec4 FragColor;

layout (location = 0) in vec2 texCoord;

layout (set = 1, binding = 0) uniform sampler2D texSampler;

void main()
{
    float color = texture(texSampler, texCoord).x;

    if (color == 0.0)
    {
        discard;
    }

    FragColor = vec4(0.0, 0.0, 0.0, 1.0);

}