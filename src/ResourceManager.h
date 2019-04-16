#pragma once

#include <unordered_map>
#include <string>

#include "ModelLoader.h"
#include "Texture.h"

struct TextureResource;
struct TriMeshResource;

struct ResourceManager {
    std::unordered_map<std::string, TriMeshResource*> TriMeshResources;
    std::unordered_map<std::string, TextureResource*> TextureResources;
    
    void loadTextureResource(std::string filename, std::string fileType);
    void loadTriMeshResource(std::string filename, std::string fileType);
    
    TextureResource* getTextureResource(std::string lookup);
    TriMeshResource* getTriMeshResource(std::string lookup);
};

struct TextureResource {
    Texture data;
};

struct TriMeshResource {
    TriangleMesh data;
};

namespace Resources {
    extern ResourceManager manager;
}
