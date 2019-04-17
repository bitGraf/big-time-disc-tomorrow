#pragma once

#include <unordered_map>
#include <string>

#include "ModelLoader.h"
#include "Texture.h"

struct TextureResource;
struct TriMeshResource;
struct TerrainResource;

struct ResourceManager {
    std::unordered_map<std::string, TriMeshResource*> TriMeshResources;
    std::unordered_map<std::string, TextureResource*> TextureResources;
    std::unordered_map<std::string, TerrainResource*> TerrainResources;
    
    void loadTextureResource(std::string filename, std::string fileType);
    void loadTriMeshResource(std::string filename, std::string fileType);
    void loadTerrainResource(std::string filename, std::string fileType);
    
    TextureResource* getTextureResource(std::string lookup);
    TriMeshResource* getTriMeshResource(std::string lookup);
    TerrainResource* getTerrainResource(std::string lookup);

    void printAllResources();
};

struct TextureResource {
    Texture data;
};

struct TriMeshResource {
    TriangleMesh data;
};

struct TerrainResource {
    TriangleMesh mesh;

    float length, width, height;
    int   N, M;
};

namespace Resources {
    extern ResourceManager manager;
}