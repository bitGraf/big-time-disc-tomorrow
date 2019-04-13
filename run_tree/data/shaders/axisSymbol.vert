#version 330 core
layout (location=0) in vec3 vertPos;

uniform mat4 view;
uniform mat4 projection;

out vec3 pass_color;

void main() {
    vec4 pos = projection * view * vec4(vertPos, 1);

    //pos.x -= 2;
    //pos.y -= 1;

    gl_Position = pos;
    //gl_Position = projection * view * vec4(vertPos, 1);

    pass_color = vertPos;
}