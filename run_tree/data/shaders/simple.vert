#version 330 core
layout (location=0) in vec3 vertPos;
layout (location=1) in vec3 vertNorm;
layout (location=2) in vec2 vertTex;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 pass_normal;
out vec3 pass_fragPos;
out vec2 pass_tex;

void main() {
    pass_fragPos = vec3(model * vec4(vertPos, 1.0));
    gl_Position = projection * view * vec4(pass_fragPos, 1);

    //pass_normal = vec3(mat4(mat3(model)) * vec4(vertNorm, 0));
    pass_normal = mat3(transpose(inverse(model))) * vertNorm;

    pass_tex = vertTex;
}