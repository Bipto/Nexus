#shader vertex
#version 450 core

layout (location = 0) in vec3 Position;
layout (location = 1) in vec2 TexCoord;
layout (location = 2) in vec3 Normal;

layout (location = 0) out vec2 OutTexCoord;
layout (location = 1) out vec3 OutNormal;
layout (location = 2) out vec3 FragPos;
layout (location = 3) out vec3 ViewPos;

layout (std140, binding = 0) uniform Camera
{
    mat4 u_View;
    mat4 u_Projection;
    vec3 u_ViewPos;
};

layout (std140, binding = 1) uniform Transform
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
}

#shader fragment
#version 450 core

layout (location = 0) in vec2 OutTexCoord;
layout (location = 1) in vec3 OutNormal;
layout (location = 2) in vec3 FragPos;
layout (location = 3) in vec3 ViewPos;

layout (location = 0) out vec4 FragColor;

layout (binding = 0) uniform sampler2D texSampler;

void main()
{
    vec3 lightPos = vec3(0.0, -2.0, -2.0);
    vec3 lightColor = vec3(1.0, 1.0, 1.0);
    vec3 objectColor = texture(texSampler, OutTexCoord).rgb;

    float ambientStrength = 0.6;
    vec3 ambient = ambientStrength * lightColor;

    vec3 norm = normalize(OutNormal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    float specularStrength = 0.5;
    vec3 viewDir = normalize(ViewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;

    vec3 result = (ambient + diffuse + specular) * objectColor;

    FragColor = vec4(result, 1.0);
}