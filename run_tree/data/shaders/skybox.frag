#version 330 core
out vec4 FragColor;

in vec3 pass_vertPos;

uniform samplerCube skybox;

void main() {
   FragColor = texture(skybox, pass_vertPos);
}