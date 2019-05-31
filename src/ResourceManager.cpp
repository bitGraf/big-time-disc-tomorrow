#include "ResourceManager.h"

ResourceManager Resources::manager;

int ResourceManager::YXtoIndex(int y, int x, int stride) {
    return x + y*stride;
}

vec3 ResourceManager::calcNorm(vec3 v1, vec3 v2, vec3 v3) {
    return Vector::cross(v2-v1, v3-v2);
}

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

void ResourceManager::loadTriMeshResource(std::string filename, std::string fileType, bool loadTangents) {
    if (TriMeshResources.find(filename) == TriMeshResources.end()) {
        printf("Loading new model resource [%s].\n", filename.c_str());

        std::string resourcePath = "../data/models/";
        std::string fullPath = resourcePath + filename + fileType;

        TriMeshResource* newResource = new TriMeshResource;
        if (fileType == ".modl")
            ModelLoader::loadFile(&newResource->data, (char*)fullPath.c_str());
        else if (fileType == ".ply") {
            printf("ply time\n");
            ModelLoader::loadFileStanford(&newResource->data, (char*)fullPath.c_str(), loadTangents);
        }
        TriMeshResources[filename] = newResource;
    } else {
        printf("Model resource [%s] already exists.\n", filename.c_str());
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

TerrainData ResourceManager::loadTerrainResource(std::string filename, std::string fileType, TerrainData inDat) {
    TerrainData retData;
    if (TriMeshResources.find(filename) == TriMeshResources.end()) {
        printf("Loading new terrain resource [%s].\n", filename.c_str());

        std::string resourcePath = "../data/textures/";
        std::string fullPath = resourcePath + filename + fileType;

        //TriMeshResource* r = new TriMeshResource;

        float length, width, height, offX, offZ, offY;
        int N, M;
        if (inDat.width) {
            length = inDat.length;
            width  = inDat.width;
            height = inDat.height;
            N = inDat.N;
            M = inDat.M;
            offX = inDat.originX;
            offY = inDat.originY;
            offZ = inDat.originZ;
        } else {
            length = 100;
            width  = 100;
            height = 10;
            N = 513;
            M = 513;
            offX = 0;
            offY = 0;
            offZ = 0;
        }

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

                vec3 position = {x + offX, yHeight*height + offY, z + offZ};
                vec3 normal = {0, 1, 0};
                vec2 tex      = {x / length, z / width};

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

        //Generate correct normals
        for (int i = 1; i < N-1; i++) {
            for (int j = 1; j < M-1; j++) {
                vec3 v1 = mesh->vertices[YXtoIndex(i-1, j-1, M)];
                vec3 v2 = mesh->vertices[YXtoIndex(i-1, j,   M)];
                vec3 v3 = mesh->vertices[YXtoIndex(i-1, j+1, M)];
                vec3 v4 = mesh->vertices[YXtoIndex(i-1, j-1, M)];
                vec3 v5 = mesh->vertices[YXtoIndex(i,   j,   M)];
                vec3 v6 = mesh->vertices[YXtoIndex(i+1, j+1, M)];
                vec3 v7 = mesh->vertices[YXtoIndex(i-1, j-1, M)];
                vec3 v8 = mesh->vertices[YXtoIndex(i,   j,   M)];
                vec3 v9 = mesh->vertices[YXtoIndex(i+1, j+1, M)];

                vec3 norm;
                norm = norm + calcNorm(v4, v5, v2);
                norm = norm + calcNorm(v2, v5, v3);
                norm = norm + calcNorm(v3, v5, v6);
                norm = norm + calcNorm(v6, v5, v8);
                norm = norm + calcNorm(v8, v5, v7);
                norm = norm + calcNorm(v7, v5, v4);
                Vector::normalize(norm);

                mesh->normals[YXtoIndex(i, j, M)] = norm;
            }
        }

        printf("Done loading indices\n");

        ModelLoader::bufferModel(mesh);

        printf("Done buffering model\n");

        //r->data = *mesh;

        TriMeshResources[filename] = ret;

        //stbi_image_free(data);
        retData.data = data;
        retData.height = height;
        retData.width = width;
        retData.length = length;
        retData.M = M;
        retData.N = N;
        retData.imgHeight = imgHeight;
        retData.imgWidth = imgWidth;
        retData.imgComps = imgComps;
        retData.originX = offX;
        retData.originY = offY;
        retData.originZ = offZ;
    } else {
        //printf("Model resource [%s] already exists.\n", filename.c_str());
    }

    return retData;
}

float ResourceManager::getHeight(unsigned char* data, float x, float y, int nComps, int stride) {
    int lookupX = (int)x;
    int lookupY = (int)y;

    //fetch just the red component, and convert from [0->255] to [0->1]
    int lookup = nComps * (lookupX + lookupY*stride);
    float value = ((float)data[lookup]) / 255.0f;

    return value;
}

float ResourceManager::getHeight(TerrainData data, float xPos, float zPos) {
    float xScale = data.imgWidth / data.length;
    float zScale = data.imgHeight / data.width;
    float x = (xPos - data.originX) * xScale;
    float z = (zPos - data.originZ) * zScale;
    float height = data.height * getHeight(data.data, x, z, data.imgComps, data.imgWidth);
    return height + data.originY;
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

void ResourceManager::cleanup() {
	// delete all textures
	for (auto tex : TextureResources) {
		tex.second->data.cleanup();
		delete tex.second;
	}

	// delete all meshes
	for (auto mesh : TriMeshResources) {
		mesh.second->data.~TriangleMesh();
		delete mesh.second;
	}
}


bool ResourceFile::load(char* filename) {
    close();

    //Read whole file into array
    fp = fopen(filename, "rb");
    if (fp == NULL) {
        printf("Failed to open file [%s]\n", filename);
        return false;
    }
    printf("Reading file: \"%s\"\n", filename);
    
    fseek(fp, 0, SEEK_END);
	fileLength = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	fileContents = (char *)malloc(fileLength + 1);
	fread(fileContents, fileLength, 1, fp);
	fclose(fp);
	fileContents[fileLength] = 0;

    // fill the filecontents with zeros
    numLines = 1;
    for (int i = 0; i < fileLength; i++) {
        char c = fileContents[i];
        if (c == '\r') {
            fileContents[i] = '\0';
        } else if (c == '\n') {
            fileContents[i] = '\0';
            numLines++;
        }
    }

    currentLine = NULL;

    // temp
    printf("File loaded. %d lines, %d characters\n", numLines, (int)fileLength);

    loaded = true;
    return true;
}
char* ResourceFile::getNextLine() {
    if (currentLine == NULL) {
        currentLine = fileContents;
        return currentLine;
    }

    int offset = 0;

    while (*(currentLine + offset) != '\0') {
        offset++;
    }
    if (*(currentLine + offset + 1) == '\0')
        offset+=2;
    //while (*(currentLine + offset) == '\0') {
    //    offset++;}
    
    currentLine = currentLine + offset;

    if (currentLine > fileContents+fileLength)
        return NULL;

    /*char c = currentLine[0];
    int i = 1;
    do {
        printf("|%c|", c);
        c = currentLine[i];
        i++;
    } while (c != 0);*/

    return currentLine;
}

char* ResourceFile::tokenSplit(char* delim) {
    return NULL;
}

void ResourceFile::close() {
    if (fp) {
        fclose(fp);
        fp = NULL;
    }

    if (fileContents) {
        free(fileContents);
        fileContents = NULL;
    }

    loaded = false;
}

ResourceFile::~ResourceFile() {
    close();
}