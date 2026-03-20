#version 450
#extension GL_ARB_separate_shader_objects : enable

out vec4 FragColor;
in vec3 texture_coordinates;

uniform samplerCube skybox;


void main() {
    FragColor = texture(skybox, texture_coordinates);
}
