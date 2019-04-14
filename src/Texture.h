#ifndef _TEXTURE_H_
#define _TEXTURE_H_

#include "glad\glad.h"
#include "stb_image.h"
//#include "stb_image_write.h"

#include <string>
#include <iostream>

const std::string TextureResourcePath = "../data/textures/";

class Texture {
public:
	Texture();

	void loadImage(std::string filename, std::string _type = "DEFAULT");
	void loadImage2(std::string filename);
	void bind(GLuint offset);
	void bindCube(GLuint offset);

private:
	GLuint textureID;
	std::string type;
	int width, height, nrChannels;
};

#endif