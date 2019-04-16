#include "ResourceManager.h"

ResourceManager Resources::manager;

void ResourceManager::loadTextureResource(std::string filename, std::string fileType) {
    if (TextureResources.find(filename) == TextureResources.end()) {
        printf("Loading new texture resource [%s].\n", filename.c_str());

        std::string resourcePath = "../data/textures/";
        std::string fullPath = resourcePath + filename + fileType;

        TextureResource* newResource = new TextureResource;
        newResource->data.loadImage(filename + fileType);
        TextureResources[filename] = newResource;
    } else {
        //printf("Texture resource [%s] already exists.\n", filename.c_str());
    }
}

void ResourceManager::loadTriMeshResource(std::string filename, std::string fileType) {
    if (TriMeshResources.find(filename) == TriMeshResources.end()) {
        printf("Loading new model resource [%s].\n", filename.c_str());

        std::string resourcePath = "../data/models/";
        std::string fullPath = resourcePath + filename + fileType;

        TriMeshResource* newResource = new TriMeshResource;
        ModelLoader::loadFile(&newResource->data, (char*)fullPath.c_str());
        TriMeshResources[filename] = newResource;
    } else {
        //printf("Model resource [%s] already exists.\n", filename.c_str());
    }
}

TextureResource* ResourceManager::getTextureResource(std::string lookup) {
    if (TextureResources.find(lookup) == TextureResources.end()) {
        printf("Texture resource [%s] not loaded yet, loading it now...\n", lookup.c_str());
        loadTextureResource(lookup, ".jpg");
        return TextureResources.at(lookup);
    } else {
        //printf("Texture resource [%s] succesfully accessed.\n", lookup.c_str());
        return TextureResources.at(lookup);
    }
}

TriMeshResource* ResourceManager::getTriMeshResource(std::string lookup) {
    if (TriMeshResources.find(lookup) == TriMeshResources.end()) {
        printf("Model resource [%s] not loaded yet, loading it now...\n", lookup.c_str());
        loadTriMeshResource(lookup, ".modl");
        return TriMeshResources.at(lookup);
    } else {
        //printf("Model resource [%s] succesfully accessed.\n", lookup.c_str());
        return TriMeshResources.at(lookup);
    }
}

void ResourceManager::printAllResources() {
    for (auto tex : TextureResources) {
        printf("Texture: [%10s] (%dx%dx%d) %d\n", tex.first.c_str(),
            tex.second->data.width, tex.second->data.height, tex.second->data.nrChannels,
            tex.second->data.textureID);
    }
    for (auto mesh : TriMeshResources) {
        printf("TriMesh: [%10s] (%dx%d) %d\n", mesh.first.c_str(),
            mesh.second->data.numVerts, mesh.second->data.numFaces,
            mesh.second->data.VAO);
    }
}