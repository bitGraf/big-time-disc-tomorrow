#version 330 core
layout (location=0) in vec3 vertPos;

uniform mat4 view;
uniform mat4 projection;

out vec3 pass_vertPos;


void main() {
	pass_vertPos = vertPos;
	gl_Position = projection * mat4(mat3(view)) * vec4(vertPos, 1.0);
}