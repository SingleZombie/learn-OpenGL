#version 330 core
struct Material {
     sampler2D diffuse;
     sampler2D specular;
     float shininess;
}; 

uniform Material material;

out vec4 FragColor;

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;
uniform vec3 viewPos;

struct DirLight
{
     vec3 direction;

     vec3 ambient;
     vec3 diffuse;
     vec3 specular;
};
vec3 calDirLight(DirLight light, vec3 norm, vec3 viewDir)
{
     vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));

     vec3 lightDir = normalize(-light.direction);
     float diff = max(dot(norm, lightDir), 0.0);
     vec3 diffuse = diff * light.diffuse * vec3(texture(material.diffuse, TexCoords));

     //vec3 viewDir = normalize(viewPos - FragPos);
     vec3 reflectDir = reflect(-lightDir, norm);
     float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
     vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));

     return (ambient + diffuse + specular);
}

struct PointLight {
     vec3 position;

     float constant;
     float linear;
     float quadratic;

     vec3 ambient;
     vec3 diffuse;
     vec3 specular;
};  
vec3 calPointLight(PointLight light, vec3 norm, vec3 fragPos, vec3 viewDir)
{
     vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));

     vec3 lightDir = normalize(light.position - FragPos);
     float diff = max(dot(norm, lightDir), 0.0);
     vec3 diffuse = diff * light.diffuse * vec3(texture(material.diffuse, TexCoords));

     vec3 reflectDir = reflect(-lightDir, norm);
     float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
     vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));

     float distance = length(light.position - fragPos);
     float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * distance * distance);

     return (ambient + diffuse + specular) * attenuation;
}
#define NR_POINT_LIGHTS 4
uniform PointLight pointLights[NR_POINT_LIGHTS];

struct SpotLight {
     vec3 position;
     vec3 direction;
     float cutOff;
     float outerCutOff;

     float constant;
     float linear;
     float quadratic;

     vec3 ambient;
     vec3 diffuse;
     vec3 specular;
};
vec3 calSpotLight(SpotLight light, vec3 norm, vec3 fragPos, vec3 viewDir)
{
     vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));

     vec3 lightDir = normalize(light.position - fragPos);
     float diff = max(dot(norm, lightDir), 0.0);
     vec3 diffuse = diff * light.diffuse * vec3(texture(material.diffuse, TexCoords));

     vec3 reflectDir = reflect(-lightDir, norm);
     float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
     vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));

     float distance = length(light.position - fragPos);
     float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * distance * distance);

     vec3 result;
     float theta = dot(lightDir, normalize(-light.direction));
     float epsilon = light.cutOff - light.outerCutOff;
     float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
     return ambient + (diffuse + specular) * attenuation * intensity;
}

uniform DirLight dirLight;
uniform SpotLight spotLight;

void main()
{

     vec3 result;
     vec3 viewDir = normalize(viewPos - FragPos);
     vec3 norm = normalize(Normal);

     result = calDirLight(dirLight, norm, viewDir) + calSpotLight(spotLight, norm, FragPos, viewDir);
     FragColor = vec4(result, texture(material.diffuse, TexCoords).a);
}