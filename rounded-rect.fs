#version 450 core

layout (location = 0) out vec4 FragColor;

layout(location = 0) out vec2 texCoord;
layout(location = 1) out vec4 outColor;
layout(location = 2) out flat float texIndex;

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
layout (set = 1, binding = 16) uniform sampler2D texture16;
layout (set = 1, binding = 17) uniform sampler2D texture17;
layout (set = 1, binding = 18) uniform sampler2D texture18;
layout (set = 1, binding = 19) uniform sampler2D texture19;
layout (set = 1, binding = 20) uniform sampler2D texture20;
layout (set = 1, binding = 21) uniform sampler2D texture21;
layout (set = 1, binding = 22) uniform sampler2D texture22;
layout (set = 1, binding = 23) uniform sampler2D texture23;
layout (set = 1, binding = 24) uniform sampler2D texture24;
layout (set = 1, binding = 25) uniform sampler2D texture25;
layout (set = 1, binding = 26) uniform sampler2D texture26;
layout (set = 1, binding = 27) uniform sampler2D texture27;
layout (set = 1, binding = 28) uniform sampler2D texture28;
layout (set = 1, binding = 29) uniform sampler2D texture29;
layout (set = 1, binding = 30) uniform sampler2D texture30;
layout (set = 1, binding = 31) uniform sampler2D texture31;

float roundedBoxSDF(vec2 centerPos, vec2 size, float radius)
{
	return length(max(abs(centerPos)-size+radius, 0.0))-radius;
}

void main()
{
    switch (int(texIndex))
    {
        case 0: FragColor = texture(texture0, texCoord); break;
        case 1: FragColor = texture(texture1, texCoord); break;
        case 2: FragColor = texture(texture2, texCoord); break;
        case 3: FragColor = texture(texture3, texCoord); break;
        case 4: FragColor = texture(texture4, texCoord); break;
        case 5: FragColor = texture(texture5, texCoord); break;
        case 6: FragColor = texture(texture6, texCoord); break;
        case 7: FragColor = texture(texture7, texCoord); break;
        case 8: FragColor = texture(texture8, texCoord); break;
        case 9: FragColor = texture(texture9, texCoord); break;
        case 10: FragColor = texture(texture10, texCoord); break;
        case 11: FragColor = texture(texture11, texCoord); break;
        case 12: FragColor = texture(texture12, texCoord); break;
        case 13: FragColor = texture(texture13, texCoord); break;
        case 14: FragColor = texture(texture14, texCoord); break;
        case 15: FragColor = texture(texture15, texCoord); break;
        case 16: FragColor = texture(texture16, texCoord); break;
        case 17: FragColor = texture(texture17, texCoord); break;
        case 18: FragColor = texture(texture18, texCoord); break;
        case 19: FragColor = texture(texture19, texCoord); break;
        case 20: FragColor = texture(texture20, texCoord); break;
        case 21: FragColor = texture(texture21, texCoord); break;
        case 22: FragColor = texture(texture22, texCoord); break;
        case 23: FragColor = texture(texture23, texCoord); break;
        case 24: FragColor = texture(texture24, texCoord); break;
        case 25: FragColor = texture(texture25, texCoord); break;
        case 26: FragColor = texture(texture26, texCoord); break;
        case 27: FragColor = texture(texture27, texCoord); break;
        case 28: FragColor = texture(texture28, texCoord); break;
        case 29: FragColor = texture(texture29, texCoord); break;
        case 30: FragColor = texture(texture30, texCoord); break;
        case 31: FragColor = texture(texture31, texCoord); break;
    }
    FragColor *= outColor;
	
	vec2 size = vec2(1, 1);
	vec2 location = vec2(0, 0);
	
	float edgeSoftness = 1.0f;
	
	float radius = 50.0f;
	
	float distance = roundedBoxSDF(texCoord.xy - location - (size / 2.0f), size / 2.0f, radius);
	
	float smoothedAlpha = 1.0f - smoothstep(0.0, edgeSoftness * 2.0f, distance);
	
	float shadowSoftness = 30.0f;
	vec2 shadowOffset = vec2(0.0f, 10.0f);
	float shadowDistance = roundedBoxSDF(texCoord.xy - location + shadowOffset - (size / 2.0f), size / 2.0f, radius);
	float shadowAlpha 1.0f - smoothstep(-shadowSoftness, shadowSoftness, shadowDistance);
	vec4 shadowColor = vec4(0.4f, 0.4f, 0.4f, 1.0f);
	
	FragColor = max(FragColor, shadowColor, shadowAlpha - smoothedAlpha);
}