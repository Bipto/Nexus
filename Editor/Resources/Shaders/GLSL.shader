#shader vertex
#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

uniform mat4 Transform;

void main()
{
    gl_Position = Transform * vec4(aPos, 1.0);
    TexCoord = aTexCoord;
}

#shader fragment
#version 330 core

out vec4 FragColor;
out vec4 Color2;

in vec2 TexCoord;

uniform sampler2D ourTexture;
uniform vec4 TintColor;
uniform mat4 Transform;

void main()
{
    //FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
    FragColor = texture(ourTexture, TexCoord) * TintColor;
    Color2 = vec4(0, 1, 0, 1);
}