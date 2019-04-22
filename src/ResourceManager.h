#pragma once

#include <unordered_map>
#include <string>

#include "ModelLoader.h"
#include "Texture.h"

struct TextureResource;
struct TriMeshResource;
//struct TerrainResource;

struct TerrainData {
    float originX = 0;
    float originY = 0;
    float originZ = 0;

    float width = 0;
    float height =0;
    float length = 0;

    int N = 0;
    int M = 0;

    int imgWidth = 0;
    int imgHeight = 0;
    int imgComps = 0;
    unsigned char* data;
};

struct ResourceManager {
    std::unordered_map<std::string, TriMeshResource*> TriMeshResources;
    std::unordered_map<std::string, TextureResource*> TextureResources;
    //std::unordered_map<std::string, TerrainResource*> TerrainResources;
    
    void loadTextureResource(std::string filename, std::string fileType);
    void loadTriMeshResource(std::string filename, std::string fileType, bool loadTangents = false);
    TriMeshResource* loadTriMeshResource(std::string filename, int numVerts, int numFaces);
    TerrainData loadTerrainResource(std::string filename, std::string fileType, TerrainData inDat);
    
    TextureResource* getTextureResource(std::string lookup);
    TriMeshResource* getTriMeshResource(std::string lookup);
    //TriMeshResource* getTerrainResource(std::string lookup);

    void printAllResources();

    float getHeight(TerrainData, float x, float y);

private:
    float getHeight(unsigned char* data, float x, float y, int nComps, int stride);
    int YXtoIndex(int y, int x, int stride);
    vec3 calcNorm(vec3 v1, vec3 v2, vec3 v3);
};

struct TextureResource {
    Texture data;
};

struct TriMeshResource {
    TriangleMesh data;
};

/*struct TerrainResource {
    TriangleMesh mesh;

    float length, width, height;
    int   N, M;
};*/

namespace Resources {
    extern ResourceManager manager;
}