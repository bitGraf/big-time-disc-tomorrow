#pragma once

#include "glad\glad.h"

#include "Matrix.h"

//#include <iostream>
#include <string>
#include "stdio.h"
#include <unordered_map>

const std::string ShaderResourcePath = "data/shaders/";

struct ShaderProgram {
	GLuint shaderID = 0;
	std::unordered_map<std::string, unsigned int> uniformMap;
	std::string Name;

	//void BuildShader(std::string vShaderPath, 
	//	std::string fShaderPath, const std::string &name = "UNNAMED SHADER");
	void smartLoad(std::string vShaderPath, 
		std::string fShaderPath, const std::string &name = "UNNAMED SHADER");
	void use();

	//Uniforms
	void setBool(const std::string &name, bool value) const;
	void setInt(const std::string &name, int value) const;
	void setFloat(const std::string &name, float value) const;
	void setMat4(const std::string &name, mat4* value) const;
	void setvec3(const std::string &name, vec3* value) const;
	void setVec4(const std::string &name, float a, float b, float c, float d) const;

	//void setLight(const std::string &name, Light light) const;
	//void initMaterial(const std::string &name) const;

private:
	void registerUniform(const std::string &name);
	void smartRegisterUniform(const char* line, int size);
	void registerAllUniforms(const char* code, long size);
};