#include "ModelLoader.h"

void ModelLoader::bufferModel(TriangleMesh* mesh, bool lineRender) {
    //printf("buffering model\n");
    GLuint VAO, VBOpos, VBOnorm, VBOtex, EBO;

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    //buffer Vertex position data;
    glGenBuffers(1, &VBOpos);
    glBindBuffer(GL_ARRAY_BUFFER, VBOpos);
    glBufferData(GL_ARRAY_BUFFER, mesh->numVerts * sizeof(vec3), mesh->vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void*)0);
    glEnableVertexAttribArray(0);
    
    //buffer Vertex normal data;
    glGenBuffers(1, &VBOnorm);
    glBindBuffer(GL_ARRAY_BUFFER, VBOnorm);
    glBufferData(GL_ARRAY_BUFFER, mesh->numVerts * sizeof(vec3), mesh->normals, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void*)0);
    glEnableVertexAttribArray(1);

	//buffer Vertex texture data
	glGenBuffers(1, &VBOtex);
	glBindBuffer(GL_ARRAY_BUFFER, VBOtex);
	glBufferData(GL_ARRAY_BUFFER, mesh->numVerts * sizeof(vec2), mesh->texcoords, GL_STATIC_DRAW);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(vec2), (void*)0);
	glEnableVertexAttribArray(2);

    //buffer indexing
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    if (lineRender)
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->numFaces*sizeof(GLuint)*2, mesh->indices, GL_STATIC_DRAW);
    else
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->numFaces*sizeof(GLuint)*3, mesh->indices, GL_STATIC_DRAW);

    glBindVertexArray(0);

    //Store all GL objects for later
    //Entity::manager.VAOs.push_back(VAO);
    //Entity::manager.VBOs.push_back(VBOpos);
    //Entity::manager.VBOs.push_back(VBOnorm);
    //Entity::manager.EBOs.push_back(EBO);

    mesh->VAO = VAO;
    mesh->VBOpos = VBOpos;
    mesh->VBOnorm = VBOnorm;
	mesh->VBOtex = VBOtex;
    mesh->EBO = EBO;

    /*for (int i = 0; i < mesh->numVerts*3; i++) {
        printf("%.2f ", *(&(mesh->vertices[0].x) + i));
    }
    printf("\n");
    for (int i = 0; i < mesh->numFaces*(mesh->lineRender ? 2 : 3); i++) {
        printf("%d ", *(mesh->indices+i));
    }
    printf("\n");*/
}

void ModelLoader::loadFile(TriangleMesh* triMesh, char* filename) {
    //printf(" Loading model from \"%s\"\n", filename);
    //
    // Open file, read contents to fileContents
    //
    FILE* modelFile = fopen(filename, "rb");
    if (modelFile == NULL) {
        printf("Failed to open file [%s]\n", filename);
        return;
    }
    printf("Reading file: \"%s\"\n", filename);
    
    fseek(modelFile, 0, SEEK_END);
	long fileLength = ftell(modelFile);
	fseek(modelFile, 0, SEEK_SET);

	char *fileContents = (char *)malloc(fileLength + 1);
	fread(fileContents, fileLength, 1, modelFile);
	fclose(modelFile);
	fileContents[fileLength] = 0;

    //printf("File contents: \n%s\n\n", fileContents);

    //
    // Split fileContents by newline characters
    //

    //TriangleMesh triMesh;   //Need to change this to not lose scope...
    int currentVertex = 0;
    int currentNormal = 0;
    int currentIndex = 0;
	int currentTexture = 0;

    char* lineContents;
    int i = 0;
    lineContents = strtok(fileContents, "\r\n");
    while (lineContents != NULL) {
        // lineContents is the current line being read
        if (lineContents[0] != '#') {
            // This line isn't a comment
            //printf("[%d]\t%s\n", i++, lineContents);

            switch (lineContents[0]) {
                case ':': {
                    //parse command
                    parseCommand(lineContents, triMesh);
                } break;
                case 'v': {
                    //parse vector data
                    vec3 result = parseVec3(lineContents);
                    assert(triMesh->vertices != NULL);
                    assert(currentVertex < triMesh->numVerts);
                    triMesh->vertices[currentVertex] = result;

                    //if lineRender, generate normals for now
                    if (triMesh->calcNormalsFromVerts) {
                        assert(triMesh->normals != NULL);
                        assert(currentNormal < triMesh->numVerts);
                        triMesh->normals[currentNormal] = Vector::normalized(result);
                        currentNormal++;
                    }

                    currentVertex++;
                } break;
                case 'n': {
                    //parse vector data
                    vec3 result = parseVec3(lineContents, !triMesh->normalisedOnLoad);
                    assert(triMesh->normals != NULL);
                    assert(currentNormal < triMesh->numVerts);
                    triMesh->normals[currentNormal] = result;
                    currentNormal++;
                } break;
                case 'f': {
                    Triangle tri = parseFace(lineContents, triMesh->index1);
                    assert(triMesh->indices != NULL);
                    if (triMesh->lineRender) {
                        if(currentIndex < triMesh->numFaces*2) {
                        triMesh->indices[currentIndex++] = tri.indices[0];
                        triMesh->indices[currentIndex++] = tri.indices[1];}
                        //currentIndex+=3;
                    } else {
                        if(currentIndex < triMesh->numFaces*3) {
                        triMesh->indices[currentIndex++] = tri.indices[0];
                        triMesh->indices[currentIndex++] = tri.indices[1];
                        triMesh->indices[currentIndex++] = tri.indices[2];}
                        //currentIndex+=3;
                    }
                } break;
				case 'u': {
					vec2 result = parseVec2(lineContents);
					assert(triMesh->texcoords != NULL);
					assert(currentTexture < triMesh->numVerts);
					triMesh->texcoords[currentTexture] = result;
					currentTexture++;
				} break;
                default: {
                }break;
            }
        }

        //prepare the next line to be read
        lineContents = strtok(NULL, "\r\n");
    }
    free(fileContents);

    int indPerPrim = (triMesh->lineRender ? 2 : 3);

    if (
        (currentVertex != triMesh->numVerts) ||
        (currentNormal != triMesh->numVerts) ||
		(currentTexture != triMesh->numVerts)||
        (currentIndex  != triMesh->numFaces*indPerPrim)

    ) {
        printf("Data load mismatch: amount of datapoints loaded incorrect.\n");
        printf("Type     Found     Expected\n");
        printf("Vertices %d        %d\n", currentVertex, triMesh->numVerts);
        printf("Normals  %d        %d\n", currentNormal, triMesh->numVerts);
		printf("Textures %d        %d\n", currentTexture, triMesh->numVerts);
        printf("Indices  %d        %d\n\n", currentIndex,  triMesh->numFaces*indPerPrim);
    }

    /*for (int i = 0; i < triMesh->numVerts; i++) {
        triMesh->vertices[i].print("v");
    }
    for (int i = 0; i < triMesh->numVerts; i++) {
        triMesh->normals[i].print("n");
    }
    for (int i = 0; i < triMesh->numFaces; i++) {
        if (triMesh->lineRender) printf("f[%d, %d]\n", triMesh->indices[2*i], triMesh->indices[2*i+1]);
        else printf("f[%d, %d, %d]\n", triMesh->indices[3*i], triMesh->indices[3*i+1], triMesh->indices[3*i+2]);
    }*/
    bufferModel(triMesh, triMesh->lineRender);
}

Triangle ModelLoader::parseFace(char* line, bool index1) {
    int start = 0;
    while(line[start] != ' ') {
        start++;
    }
    while(line[start] == ' ') {
        start++;
    }
    int offset = index1 ? 1 : 0;
    char* pstr = line+start;
    int x = (GLuint)strtoul(pstr, &pstr, 10) - offset;  //split the string into three ints
    int y = (GLuint)strtoul(pstr, &pstr, 10) - offset;
    int z = (GLuint)strtoul(pstr, NULL, 10) - offset;

    Triangle ret = {x, y, z};
    //printf("\tTriangle: [%d][%d][%d]\n", x, y, z);
    return ret;
}

vec3 ModelLoader::parseVec3(char* line, bool normalise) {
    int start = 0;
    while(line[start] != ' ') {
        start++;
    }
    while(line[start] == ' ') {
        start++;
    }
    char* pstr = line+start;
    float x = strtof(pstr, &pstr);  //split the string into three floats
    float y = strtof(pstr, &pstr);
    float z = strtof(pstr, NULL);

    vec3 ret = {x, y, z};
    if (normalise) {
        Vector::normalize(ret);
    }
    //ret.print("\tVector3: ");
    return ret;
}

vec2 ModelLoader::parseVec2(char* line, bool normalise) {
	int start = 0;
	while (line[start] != ' ') {
		start++;
	}
	while (line[start] == ' ') {
		start++;
	}
	char* pstr = line + start;
	float x = strtof(pstr, &pstr);  //split the string into two floats
	float y = strtof(pstr, NULL);


	vec2 ret = { x, y };
	//ret.print("\tVector2: ");
	return ret;
}

void ModelLoader::parseCommand(char* line, TriangleMesh* mesh) {
    int start = 0;
    while (line[start] != ' ') {
        start++;
    }
    start--;
    char* varName = (char*)malloc((start+1) * sizeof(char));
    strncpy(varName, line+1, start);
    varName[start] = 0;
    //printf("\t[%s] ", varName);
    start += 2;
    char* pstr = line+start;

    // set variable [varName] to [value]
    if (!strcmp(varName, "numVerts")) {
        int value = (int)strtol(pstr+2, NULL, 10);
        //printf("%d\n", value);
        mesh->numVerts = value;
        //printf("\tSetting mesh->numVerts to %d\n", value);
        //allocate memory for vertices
        mesh->vertices  = (vec3*)malloc(mesh->numVerts * sizeof(vec3));
        mesh->normals   = (vec3*)malloc(mesh->numVerts * sizeof(vec3));
		mesh->texcoords = (vec2*)malloc(mesh->numVerts * sizeof(vec2));

    } else if (!strcmp(varName, "numFaces")) {
        int value = (int)strtol(pstr+2, NULL, 10);
        //printf("%d\n", value);
        mesh->numFaces = value;
        //printf("\tSetting mesh->numFaces to %d\n", value);
        //allocate memory for indices
        if (mesh->lineRender) {
            mesh->indices = (GLuint*)malloc(2 * mesh->numFaces * sizeof(int));//2 indices per line
        } else {
            mesh->indices = (GLuint*)malloc(3 * mesh->numFaces * sizeof(int));//3 indices per triangle
        }
    } else if (!strcmp(varName, "normalizedVectors")) {
        bool value = (strcmp(pstr+2, "false"));
        //printf("%s\n", value ? "true" : "false");
        mesh->normalisedOnLoad = value;
    } else if (!strcmp(varName, "lineRender")) {
        bool value = (strcmp(pstr+2, "false"));
        //printf("%s\n", value ? "true" : "false");
        mesh->lineRender = value;
        mesh->calcNormalsFromVerts = value;
    } else if (!strcmp(varName, "calcNormals")) {
        bool value = (strcmp(pstr+2, "false"));
        //printf("%s\n", value ? "true" : "false");
        mesh->calcNormalsFromVerts = value;
    } else if (!strcmp(varName, "index1")) {
        //printf("Indexing starts at 1\n");
        mesh->index1 = true;
    }else {
        printf("Unknown variable [%s] found.\n", varName);
    }

    free(varName);
}