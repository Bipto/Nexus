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
	vec3 lightColor	   = vec3(3.5, 3.5, 3.5);
	vec3 objectColor = texture(diffuseMapSampler, OutTexCoord).rgb;
    vec3 specularColor = texture(specularMapSampler, OutTexCoord).rgb;

    float ambientStrength = 0.8;
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