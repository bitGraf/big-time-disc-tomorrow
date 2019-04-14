#include "Texture.h"

Texture::Texture() {
}

void Texture::loadImage(std::string filename, std::string _type) {
	type = _type;
	//Generate OpenGL Texture
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	// set the texture wrapping/filtering options

	//Load image data
	unsigned char *data = stbi_load((TextureResourcePath + filename).c_str(), 
		&width, &height, &nrChannels, 0);

	if (data) {
		GLenum format;
		if (nrChannels == 1)
			format = GL_RED;
		else if (nrChannels == 3)
			format = GL_RGB;
		else if (nrChannels == 4)
			format = GL_RGBA;

		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	} else {
		std::cout << "Failed to load texture [" << filename << "]" << std::endl;
	}

	stbi_image_free(data);
}

void Texture::loadImage2(std::string filename) {
	loadImage(filename);
}

void Texture::bind(GLuint offset) {
	glActiveTexture(offset);
	glBindTexture(GL_TEXTURE_2D, textureID);
}

void Texture::bindCube(GLuint offset) {
	glActiveTexture(offset);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
}
