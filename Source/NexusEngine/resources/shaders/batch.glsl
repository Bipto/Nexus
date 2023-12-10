#shader vertex
#version 450 core

layout (location = 0) in vec3 Position;
layout (location = 1) in vec2 TexCoord;
layout (location = 2) in vec4 Color;
layout (location = 3) in float TexIndex;

layout (location = 0) out vec2 texCoord;
layout (location = 1) out vec4 outColor;
layout (location = 2) out flat float texIndex;

layout (binding = 0, set = 0) uniform MVP
{
    mat4 u_MVP;
};

void main()
{
    gl_Position = u_MVP * vec4(Position, 1.0);
    texCoord = TexCoord;
    outColor = Color;
    texIndex = TexIndex;
}

#shader fragment
#version 450 core

layout (location = 0) out vec4 FragColor;

layout (location = 0) in vec2 texCoord;
layout (location = 1) in vec4 outColor;
layout (location = 2) in flat float texIndex;

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
    switch (int(texIndex))
    {
        case 0: FragColor = texture(texture0, texCoord) * outColor; break;
        case 1: FragColor = texture(texture1, texCoord) * outColor; break;
        case 2: FragColor = texture(texture2, texCoord) * outColor; break;
        case 3: FragColor = texture(texture3, texCoord) * outColor; break;
        case 4: FragColor = texture(texture4, texCoord) * outColor; break;
        case 5: FragColor = texture(texture5, texCoord) * outColor; break;
        case 6: FragColor = texture(texture6, texCoord) * outColor; break;
        case 7: FragColor = texture(texture7, texCoord) * outColor; break;
        case 8: FragColor = texture(texture8, texCoord) * outColor; break;
        case 9: FragColor = texture(texture9, texCoord) * outColor; break;
        case 10: FragColor = texture(texture10, texCoord) * outColor; break;
        case 11: FragColor = texture(texture11, texCoord) * outColor; break;
        case 12: FragColor = texture(texture12, texCoord) * outColor; break;
        case 13: FragColor = texture(texture13, texCoord) * outColor; break;
        case 14: FragColor = texture(texture14, texCoord) * outColor; break;
        case 15: FragColor = texture(texture15, texCoord) * outColor; break;
    }
}