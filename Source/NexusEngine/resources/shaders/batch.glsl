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

layout (set = 1, binding = 0) uniform sampler2D texture0;
layout (set = 1, binding = 1) uniform sampler2D texture1;
layout (set = 1, binding = 2) uniform sampler2D texture2;
layout (set = 1, binding = 3) uniform sampler2D texture3;
layout (set = 1, binding = 4) uniform sampler2D texture4;
layout (set = 1, binding = 5) uniform sampler2D texture5;
layout (set = 1, binding = 6) uniform sampler2D texture6;
layout (set = 1, binding = 7) uniform sampler2D texture7;
layout (set = 1, binding = 8) uniform sampler2D texture8;
layout (set = 1, binding = 9) uniform sampler2D texture9;
layout (set = 1, binding = 10) uniform sampler2D texture10;
layout (set = 1, binding = 11) uniform sampler2D texture11;
layout (set = 1, binding = 12) uniform sampler2D texture12;
layout (set = 1, binding = 13) uniform sampler2D texture13;
layout (set = 1, binding = 14) uniform sampler2D texture14;
layout (set = 1, binding = 15) uniform sampler2D texture15;

void main()
{
    float alpha = 0;
    
    switch (int(texIndex))
    {
        case 0: alpha = texture(texture0, texCoord).r; break;
        case 1: alpha = texture(texture1, texCoord).r; break;
        case 2: alpha = texture(texture2, texCoord).r; break;
        case 3: alpha = texture(texture3, texCoord).r; break;
        case 4: alpha = texture(texture4, texCoord).r; break;
        case 5: alpha = texture(texture5, texCoord).r; break;
        case 6: alpha = texture(texture6, texCoord).r; break;
        case 7: alpha = texture(texture7, texCoord).r; break;
        case 8: alpha = texture(texture8, texCoord).r; break;
        case 9: alpha = texture(texture9, texCoord).r; break;
        case 10: alpha = texture(texture10, texCoord).r; break;
        case 11: alpha = texture(texture11, texCoord).r; break;
        case 12: alpha = texture(texture12, texCoord).r; break;
        case 13: alpha = texture(texture13, texCoord).r; break;
        case 14: alpha = texture(texture14, texCoord).r; break;
        case 15: alpha = texture(texture15, texCoord).r; break;
    }

    if (alpha < 0.5)
    {
        discard;
    }

    //alpha = 1.0 - smoothstep(0.5, 0.6, alpha);

    float w = fwidth(alpha);
    float opacity = smoothstep(0.5 - w, 0.5 + w, alpha);

    FragColor = vec4(outColor.rgb, opacity);

    /* float dist = 0;

    switch (int(texIndex))
    {
        case 0: dist = texture(texture0, texCoord).r; break;
        case 1: dist = texture(texture1, texCoord).r; break;
        case 2: dist = texture(texture2, texCoord).r; break;
        case 3: dist = texture(texture3, texCoord).r; break;
        case 4: dist = texture(texture4, texCoord).r; break;
        case 5: dist = texture(texture5, texCoord).r; break;
        case 6: dist = texture(texture6, texCoord).r; break;
        case 7: dist = texture(texture7, texCoord).r; break;
        case 8: dist = texture(texture8, texCoord).r; break;
        case 9: dist = texture(texture9, texCoord).r; break;
        case 10: dist = texture(texture10, texCoord).r; break;
        case 11: dist = texture(texture11, texCoord).r; break;
        case 12: dist = texture(texture12, texCoord).r; break;
        case 13: dist = texture(texture13, texCoord).r; break;
        case 14: dist = texture(texture14, texCoord).r; break;
        case 15: dist = texture(texture15, texCoord).r; break;
    }

    if (glyphShape < 0.5)
    {
        discard;
    }

    FragColor = outColor;

    if (dist < 0.5)
    {
        discard;
    }

    FragColor = vec4(outColor.rgb, 1.0f); */
}