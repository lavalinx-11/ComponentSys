#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec4 vVertex;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec2 uvCoord;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform vec3 lightPos[5];   

layout(location = 0) out vec3 vertNormal;
layout(location = 1) out vec3 eyeDir;
layout(location = 2) out vec2 textureCoords;
layout(location = 3) out vec3 fragPos;  

void main() {
    textureCoords = uvCoord;
    textureCoords.y *= -1.0;
    vec4 viewPos = viewMatrix * modelMatrix * vVertex;
    fragPos = viewPos.xyz;

    mat3 normalMatrix = mat3(transpose(inverse(viewMatrix * modelMatrix)));
    vertNormal = normalize(normalMatrix * vNormal);

    eyeDir = normalize(-fragPos);

    gl_Position = projectionMatrix * viewPos;
}