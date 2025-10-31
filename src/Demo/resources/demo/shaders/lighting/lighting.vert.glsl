#version 450 core

layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec2 a_TexCoord;
layout (location = 2) in vec3 a_Normal;
layout (location = 3) in vec3 a_Tangent;
layout (location = 4) in vec3 a_Bitangent;

layout (location = 0) out vec2 o_TexCoord;
layout (location = 1) out vec3 o_Normal;
layout (location = 2) out vec3 o_FragPos;
layout (location = 3) out vec3 o_ViewPos;
layout (location = 4) out mat3 o_TBN;

layout (std140,binding = 0, set = 0) uniform Camera
{
    mat4 u_View;
    mat4 u_Projection;
    vec3 u_ViewPos;
};

layout (std140,binding = 1, set = 0) uniform Transform
{
    mat4 u_Transform;
};

void main()
{
    gl_Position = u_Projection * u_View * u_Transform * vec4(a_Position, 1.0);
    o_TexCoord = a_TexCoord;
    o_Normal = mat3(transpose(inverse(u_Transform))) * a_Normal;
    o_FragPos = vec3(u_Transform * vec4(a_Position, 1.0));
    o_ViewPos = u_ViewPos;

    vec3 T = normalize(vec3(u_Transform * vec4(a_Tangent, 0.0)));
    vec3 B = normalize(vec3(u_Transform * vec4(a_Bitangent, 0.0)));
    vec3 N = normalize(vec3(u_Transform * vec4(a_Normal, 0.0)));
    o_TBN = mat3(T, B, N);
}