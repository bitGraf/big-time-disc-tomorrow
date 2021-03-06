#ifndef _TEXTURE_H_
#define _TEXTURE_H_

#include "glad/glad.h"
#include "stb_image.h"
//#include "stb_image_write.h"

#include <string>
#include <iostream>

const std::string TextureResourcePath = "../data/textures/";

class Texture {
public:
    Texture();

    void loadImage(std::string filename);
    void loadCubeMap(std::string filename, std::string filetype);
    void loadImage2(std::string filename);
    void bind(GLuint offset);
    void bindCube(GLuint offset);
    void cleanup();

    GLuint textureID;
    int width, height, nrChannels;
};

#endif