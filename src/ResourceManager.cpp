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

TriMeshResource* ResourceManager::loadTriMeshResource(std::string filename, int numVerts, int numFaces) {
    if (TriMeshResources.find(filename) == TriMeshResources.end()) {
        printf("Creating new model resource [%s].\n", filename.c_str());

        TriMeshResource* newResource = new TriMeshResource;

        //newResource->data.
        TriangleMesh* mesh = &newResource->data;
        mesh->numVerts = numVerts;
        //allocate memory for vertices
        mesh->vertices = (vec3*)malloc(mesh->numVerts * sizeof(vec3));
        mesh->normals  = (vec3*)malloc(mesh->numVerts * sizeof(vec3));
		mesh->texcoords = (vec2*)malloc(mesh->numVerts * sizeof(vec2));

        mesh->numFaces = numFaces;
        //allocate memory for indices
        mesh->indices = (GLuint*)malloc(3 * mesh->numFaces * sizeof(int));//3 indices per triangle

        TriMeshResources[filename] = newResource;
    } else {
        //printf("Model resource [%s] already exists.\n", filename.c_str());
    }

    return TriMeshResources[filename];
}

void ResourceManager::loadTerrainResource(std::string filename, std::string fileType) {
    if (TriMeshResources.find(filename) == TriMeshResources.end()) {
        printf("Loading new terrain resource [%s].\n", filename.c_str());

        std::string resourcePath = "../data/textures/";
        std::string fullPath = resourcePath + filename + fileType;

        //TriMeshResource* r = new TriMeshResource;

        float length = 100;
        float width  = 100;
        float height = 10;
        int N = 513;
        int M = 513;

        //load image, parse data, and create terrain from heightmap.
        int imgWidth, imgHeight, imgComps;
        unsigned char *data = stbi_load(fullPath.c_str(), 
		    &imgWidth, &imgHeight, &imgComps, 0);

        float delx = length / (M-1);
        float delz = width / (N-1);

        int numVerts = N*M;
        int numFaces = (N-1)*(M-1)*2;

        printf("NumVerts: %d, NumFaces: %d\n", numVerts, numFaces);

        TriMeshResource* ret = loadTriMeshResource(filename, numVerts, numFaces);
        TriangleMesh* mesh = &ret->data;

        int currentVert = 0;

        for (int i = 0; i < N; i++) {
            for (int j = 0; j < M; j++) {
                float x = j * delx;
                float z = i * delz;
                float lx = ((float)j / (M)) * imgWidth;
                float ly = ((float)i / (N)) * imgHeight;
                float yHeight = getHeight(data, lx, ly, imgComps, imgWidth);

                vec3 position = {x, yHeight*height - 20, z};
                vec3 normal   = {0, 1, 0};
                vec2 tex      = {0, 0};

                mesh->vertices[currentVert]  = position;
                mesh->normals[currentVert]   = normal;
                mesh->texcoords[currentVert] = tex;
                currentVert++;

                //position.print("Adding vert: ");

                int pH = (int)(yHeight * 10);

                //printf("%2d ", pH);

                //printf("Height at %d,%d = %f\n", i, j, getHeight(data, j, i, imgComps, imgWidth));
            }
            //printf("\n");
        }

        printf("Done loading verts\n");

        stbi_image_free(data);

        int currentIndex = 0;
        int offset = 0;

        for (int ynum = 0; ynum < N-1; ynum++) {
            int A1 = 0 + offset;
            int A2 = M + offset;
            int A3 = 1 + offset;
            int B1 = 1 + offset;
            int B2 = M + offset;
            int B3 = M+1 + offset;
            for (int xnum = 0; xnum < M-1; xnum++) {
                //printf("Adding triangles (%d %d %d) and (%d %d %d)\n", A1, A2, A3, B1, B2, B3);
                mesh->indices[currentIndex++] = A1;
                mesh->indices[currentIndex++] = A2;
                mesh->indices[currentIndex++] = A3;
                mesh->indices[currentIndex++] = B1;
                mesh->indices[currentIndex++] = B2;
                mesh->indices[currentIndex++] = B3;

                A1++;
                A2++;
                A3++;

                B1++;
                B2++;
                B3++;
            }

            offset += M;
        }

        printf("Done loading indices\n");

        ModelLoader::bufferModel(mesh);

        printf("Done buffering model\n");

        //r->data = *mesh;

        TriMeshResources[filename] = ret;
    } else {
        //printf("Model resource [%s] already exists.\n", filename.c_str());
    }
}

float ResourceManager::getHeight(unsigned char* data, float x, float y, int nComps, int stride) {
    int lookupX = (int)x;
    int lookupY = (int)y;

    //fetch just the red component, and convert from [0->255] to [0->1]
    int lookup = nComps * (lookupX + lookupY*stride);
    float value = ((float)data[lookup]) / 255.0f;

    return value;
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

/*TerrainResource* ResourceManager::getTerrainResource(std::string lookup) {
    if (TerrainResources.find(lookup) == TerrainResources.end()) {
        printf("Terrain resource [%s] not loaded yet, loading it now...\n", lookup.c_str());
        loadTerrainResource(lookup, ".png");
        return TerrainResources.at(lookup);
    } else {
        //printf("Model resource [%s] succesfully accessed.\n", lookup.c_str());
        return TerrainResources.at(lookup);
    }
}*/

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