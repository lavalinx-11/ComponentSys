#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) out vec4 fragColor;

layout(location = 0) in vec3 vertNormal;
layout(location = 1) in vec3 eyeDir; 
layout(location = 2) in vec2 textureCoords; 
layout(location = 3) in vec3 fragPos;   

uniform vec3 lightPos[5];              
uniform vec4 Specular[5]; 
uniform vec4 Diffuse[5]; 
uniform vec4 Ambient[5]; 

uniform sampler2D myTexture; 

void main() {
    vec4 kt = texture(myTexture, textureCoords); 
    
    vec4 totalLight = vec4(0.0);
    
    vec3 n = normalize(vertNormal);
    vec3 e = normalize(eyeDir);

    for(int i = 0; i < 5; i++) {

        // Light direction from fragment → light
        vec3 l = normalize(lightPos[i] - fragPos);

        // Diffuse
        float diff = max(dot(n, l), 0.0);

        // Specular
        vec3 reflection = reflect(-l, n);
        float spec = pow(max(dot(e, reflection), 0.0), 14.0);

        // Accumulate ALL components
        totalLight += Ambient[i] 
                    + (diff * Diffuse[i]) 
                    + (spec * Specular[i]);
    }

    fragColor = clamp(totalLight, 0.0, 1.0) * kt;
//fragColor = vec4(lightPos[1], 1.0);
}