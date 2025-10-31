#version 450 core

layout (location = 0) in vec2 a_TexCoord;
layout (location = 1) in vec3 a_Normal;
layout (location = 2) in vec3 a_FragPos;
layout (location = 3) in vec3 a_ViewPos;
layout (location = 4) in mat3 a_TBN;


layout (location = 0) out vec4 FragColor;

layout (binding = 0, set = 1) uniform sampler2D u_DiffuseMap;
layout (binding = 1, set = 1) uniform sampler2D u_NormalMap;
layout (binding = 2, set = 1) uniform sampler2D u_SpecularMap;

void main()
{
    vec3 lightPos = vec3(0.0, -2.0, -2.0);
    vec3 lightColor = vec3(1.0, 1.0, 1.0);
    vec3 objectColor = texture(u_DiffuseMap, a_TexCoord).rgb;
    vec3 specularColor = texture(u_SpecularMap, a_TexCoord).rgb;

    float ambientStrength = 0.6;
    vec3 ambient = ambientStrength * lightColor;

    vec3 normal = texture(u_NormalMap, a_TexCoord).rgb;
    normal = normal * 2.0 - 1.0;
    normal = normalize(a_TBN * normal);    

    //vec3 normal = normalize(a_Normal);
    vec3 lightDir = normalize(lightPos - a_FragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    float specularStrength = 0.5;
    vec3 viewDir = normalize(a_ViewPos - a_FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;

    vec3 result = (ambient + diffuse + specular) * objectColor * specularColor;

    FragColor = vec4(result, 1.0);
}