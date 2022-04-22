#version 330 core
out vec4 color;

in vec3 FragPos;  
in vec3 Normal;  
in vec2 TexCoord;
  
uniform vec3 lightPos; 
uniform vec3 viewPos;
uniform vec3 lightColor;
uniform sampler2D ourTexture;

void main()
{
    vec3 texColor = texture(ourTexture, TexCoord).rgb;
    // ambient
    vec3 ambient = 0.7 * texColor * lightColor;
    // diffuse
    vec3 lightDir = normalize(lightPos - FragPos);
    vec3 normal = normalize(Normal);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * texColor * lightColor;
    // specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = 0.0;
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
    vec3 specular = vec3(0.3) * spec * lightColor;
    float attenuation = 0.5;
    vec3 result = (ambient + attenuation * (diffuse) + specular);
    color = vec4(result, 0.5f);
} 