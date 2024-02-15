#shader vertex
#version 450 core

layout (location = 0) in vec3 Position;
layout (location = 1) in vec2 TexCoord;
layout (location = 2) in vec3 Normal;
layout (location = 3) in vec3 Tangent;
layout (location = 4) in vec3 Bitangent;

layout (location = 0) out vec2 OutTexCoord;
layout (location = 1) out vec3 OutNormal;
layout (location = 2) out vec3 FragPos;
layout (location = 3) out vec3 ViewPos;
layout (location = 4) out mat3 TBN;

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
    gl_Position = u_Projection * u_View * u_Transform * vec4(Position, 1.0);
    OutTexCoord = TexCoord;
    OutNormal = mat3(transpose(inverse(u_Transform))) * Normal;
    FragPos = vec3(u_Transform * vec4(Position, 1.0));
    ViewPos = u_ViewPos;

    vec3 T = normalize(vec3(u_Transform * vec4(Tangent, 0.0)));
    vec3 B = normalize(vec3(u_Transform * vec4(Bitangent, 0.0)));
    vec3 N = normalize(vec3(u_Transform * vec4(Normal, 0.0)));
    TBN = mat3(T, B, N);
}

#shader fragment
#version 450 core

layout (location = 0) in vec2 OutTexCoord;
layout (location = 1) in vec3 OutNormal;
layout (location = 2) in vec3 FragPos;
layout (location = 3) in vec3 ViewPos;
layout (location = 4) in mat3 TBN;


layout (location = 0) out vec4 FragColor;

layout (binding = 0, set = 1) uniform sampler2D diffuseMapSampler;
layout (binding = 1, set = 1) uniform sampler2D normalMapSampler;
layout (binding = 2, set = 1) uniform sampler2D specularMapSampler;

void main()
{
    vec3 lightPos = vec3(0.0, -2.0, -2.0);
    vec3 lightColor = vec3(1.0, 1.0, 1.0);
    vec3 objectColor = texture(diffuseMapSampler, OutTexCoord).rgb;
    vec3 specularColor = texture(specularMapSampler, OutTexCoord).rgb;

    float ambientStrength = 0.6;
    vec3 ambient = ambientStrength * lightColor;

    vec3 normal = texture(normalMapSampler, OutTexCoord).rgb;
    normal = normal * 2.0 - 1.0;
    normal = normalize(TBN * normal);    

    //vec3 normal = normalize(OutNormal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    float specularStrength = 0.5;
    vec3 viewDir = normalize(ViewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;

    vec3 result = (ambient + diffuse + specular) * objectColor * specularColor;

    FragColor = vec4(result, 1.0);
}