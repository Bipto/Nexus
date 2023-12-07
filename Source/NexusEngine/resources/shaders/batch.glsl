#shader vertex
#version 450 core

layout (location = 0) in vec3 Position;
layout (location = 1) in vec2 TexCoord;

layout (location = 0) out vec2 texCoord;

void main()
{
    gl_Position = vec4(Position, 1.0);
    texCoord = TexCoord;
}

#shader fragment
#version 450 core

layout (location = 0) out vec4 FragColor;

layout (location = 0) in vec2 texCoord;

layout (set = 0, binding = 0) uniform sampler2D texture0;
layout (set = 0, binding = 1) uniform sampler2D texture1;
layout (set = 0, binding = 2) uniform sampler2D texture2;
layout (set = 0, binding = 3) uniform sampler2D texture3;
layout (set = 0, binding = 4) uniform sampler2D texture4;
layout (set = 0, binding = 5) uniform sampler2D texture5;
layout (set = 0, binding = 6) uniform sampler2D texture6;
layout (set = 0, binding = 7) uniform sampler2D texture7;
layout (set = 0, binding = 8) uniform sampler2D texture8;
layout (set = 0, binding = 9) uniform sampler2D texture9;
layout (set = 0, binding = 10) uniform sampler2D texture10;
layout (set = 0, binding = 11) uniform sampler2D texture11;
layout (set = 0, binding = 12) uniform sampler2D texture12;
layout (set = 0, binding = 13) uniform sampler2D texture13;
layout (set = 0, binding = 14) uniform sampler2D texture14;
layout (set = 0, binding = 15) uniform sampler2D texture15;

void main()
{
    FragColor = texture(texture0, texCoord);
}