#shader vertex
#version 450

layout (location = 0) in vec2 inPosition;
layout (location = 1) in vec3 inColor;
layout (location = 2) in vec2 inTexCoord;

layout (location = 0) out vec3 outColor;
layout (location = 1) out vec2 outTexCoord;

layout (set = 0, binding = 0) uniform CameraBuffer
{
    mat4 MVP;
} cameraBuffer;

void main()
{
    gl_Position = cameraBuffer.MVP * vec4(inPosition, 0.0f, 1.0f);
    outColor = vec3(inTexCoord, 0.0f);
    outTexCoord = inTexCoord;
}

#shader fragment
#version 450

layout (location = 0) in vec3 inColor;
layout (location = 1) in vec2 inTexCoord;

layout (location = 0) out vec4 outFragColor;

layout (set = 1, binding = 0) uniform sampler2D tex1;

void main()
{
    //outFragColor = vec4(inColor, 1.0f);

    vec3 color = texture(tex1, inTexCoord).xyz;
    outFragColor = vec4(color, 1.0f);
}