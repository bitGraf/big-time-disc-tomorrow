#pragma once
#include "ShaderProgram.h"
#include "Texture.h"

class SpriteRenderer
{
public:
	SpriteRenderer(ShaderProgram &shader);
	~SpriteRenderer();

	void DrawSprite(Texture &texture, vec2 position,
		vec2 size = vec2{ 10.f, 10.f }, GLfloat rotate = 0.0f,
		vec3 color = vec3{ 1.f, 1.f, 1.f });
private:
	ShaderProgram shader;
	GLuint quadVAO;

	void initRenderData();
};