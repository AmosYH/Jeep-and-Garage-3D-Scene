#version 330 core
out vec4 color;

in vec3 FragPos;  
in vec3 Normal;  
in vec2 TexCoord;
  
uniform vec3 lightPos; 
uniform vec3 viewPos;
uniform vec3 lightColor;
uniform float ambientVal;
uniform sampler2D ourTexture;

void main()
{
    float ambientStrength = 1;
    vec3 ambient = ambientStrength * lightColor;
    float attenuation = ambientVal;
    vec3 tex = vec3(texture(ourTexture, TexCoord));
    vec3 result = ( ambient + attenuation ) * tex;
    color = vec4(result, 0.5f);
} 