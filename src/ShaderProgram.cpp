#include "ShaderProgram.h"

void ShaderProgram::registerAllUniforms(const char* code, long size) {
    //printf("Finding all lines that start with \"uniform\"\n");
    //printf("  Searching for all uniforms\n");
    int position = 0;
    char* wordMatch = (char*)malloc(9*sizeof(char));
    wordMatch[8] = 0;
    while(position < size) {
        strncpy(wordMatch, code+position, 8);
        //printf("first 8 characters: %s\n", wordMatch);
        if (!strcmp(wordMatch, "void mai")) {
            //found the main function, no uniforms should be after This
            //printf("   Main function found at [%d], breaking...\n", position);
            break;
        }
        if (!strcmp(wordMatch, "uniform ")) {
            char uniformName[32];
            int nameLoc = 0;
            //printf("   Line starts with uniform. Rest of line: \"");
            position+=7;
            while ((code[position] != '\r')&&(code[position] != '\n')) {
                position++;
                if (position >= size)
                    break;
                //if (code[position] != '\r') {
                if (code[position] != ';') {
                    //printf("%c", code[position]);    //put code[position] into an array instead of print
                    uniformName[nameLoc] = code[position];
                    nameLoc++;
                } else {
                    uniformName[nameLoc] = '\0';
                    //printf("\"%d characters long\n", nameLoc);    //append /0 to end of array
                    position++;
                }
            }
            //printf("\t%s\n", uniformName);
            smartRegisterUniform(uniformName, nameLoc);
            position++;
        } else {
            //printf("no match\n");
            while (code[position] != '\n') {
                position++;
                if (position >= size)
                    break;
            }
            position++;
        }
        //printf("[%2d]: %c\n", position, code[position]);
    }
    free(wordMatch);
    //printf("  Done searching.\n\n");
}

void ShaderProgram::smartRegisterUniform(const char* line, int size) {
    //line is something like "mat4 model\0~~~~~~"
    //first read until the first space, then read the rest
    int loc = 0;
    while (line[loc] != ' ') {
        loc++;
    }
    loc++;
    int length = size - loc;

    char* finalName = (char*)malloc((length+1)*sizeof(char));
    strncpy(finalName, line+loc, length);
    finalName[length] = 0;
    //finalName is a zero-terminated string with [length+1] 
    //elements (inlcuding \0)
    //This is the final string that gets registered in the unordered_map
    std::string uniName(finalName);


    /*printf("Uniform Name: (%d)|", length);
    for (int i = 0; i < length+1; i++) {
        printf("%d ", finalName[i]);
    }
    printf("|");
    printf("%s\n", uniName.c_str());*/

    registerUniform(uniName);

    //free the dynamic data
    free(finalName);
}

void ShaderProgram::registerUniform(const std::string &name) {
    GLuint lookupID = glGetUniformLocation(shaderID, name.c_str());
    uniformMap[name] = lookupID;
    //printf("    %s: Uniform Register [%2d]: \"%s\"\n", Name.c_str(), lookupID, name.c_str());
}

void ShaderProgram::smartLoad(std::string vShaderPath,
    std::string fShaderPath, const std::string &name) {
    Name = name;
    printf("Building shader(%s): [%s]|[%s]\n", name.c_str(), vShaderPath.c_str(), fShaderPath.c_str());
    //Load Shaders
    FILE *f = fopen((ShaderResourcePath + vShaderPath).c_str(), "rb");
    if (f == NULL) {
        printf("  Failed to open file: %s\n", vShaderPath.c_str());
        return;
    }
    fseek(f, 0, SEEK_END);
    long vSize = ftell(f);
    fseek(f, 0, SEEK_SET);

    char *vShaderCode = (char *)malloc(vSize + 1);
    fread(vShaderCode, vSize, 1, f);
    fclose(f);
    vShaderCode[vSize] = 0;

    f = fopen((ShaderResourcePath + fShaderPath).c_str(), "rb");
    if (f == NULL) {
        printf("  Failed to open file: %s\n", fShaderPath.c_str());
        return;
    }
    fseek(f, 0, SEEK_END);
    long fSize = ftell(f);
    fseek(f, 0, SEEK_SET);

    char *fShaderCode = (char *)malloc(fSize + 1);
    fread(fShaderCode, fSize, 1, f);
    fclose(f);
    fShaderCode[fSize] = 0;

    //Build OpenGL Shader Program
    //Vertex shader
    GLuint vShader;
    vShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vShader, 1, &vShaderCode, NULL);
    glCompileShader(vShader);
    //Check compile errors
    int  success;
    char infoLog[512];
    glGetShaderiv(vShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vShader, 512, NULL, infoLog);
        printf("  ERROR::SHADER::VERTEX::COMPILATION_FAILED(%s)\n %s\n", vShaderPath.c_str(), infoLog);
    }
    //Fragment shader
    GLuint fShader;
    fShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fShader, 1, &fShaderCode, NULL);
    glCompileShader(fShader);
    //Check compile errors
    glGetShaderiv(fShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fShader, 512, NULL, infoLog);
        printf("  ERROR::SHADER::FRAGMENT::COMPILATION_FAILED(%s)\n %s\n", fShaderPath.c_str(), infoLog);
    }
    //Shader program
    shaderID = glCreateProgram();
    glAttachShader(shaderID, vShader);
    glAttachShader(shaderID, fShader);
    glLinkProgram(shaderID);
    //Check errors
    glGetProgramiv(shaderID, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderID, 512, NULL, infoLog);
        printf("  ERROR::SHADER::LINK_FAILED\n %s\n", infoLog);
    }

    free(vShaderCode);
    free(fShaderCode);
    glDeleteShader(vShader);
    glDeleteShader(fShader);

    if (success && useUniformMap) {
        registerAllUniforms(vShaderCode, vSize);
        registerAllUniforms(fShaderCode, fSize);
    }
}

void ShaderProgram::use() {
    glUseProgram(shaderID);
}

GLuint ShaderProgram::getLoc(const std::string &name, bool useMap) const {
    if (useUniformMap && useMap) {
        // lookup uniform location in map
        return uniformMap.at(name);
    } else {
        // just query the gpu for the location
        return glGetUniformLocation(shaderID, name.c_str());
    }
}

void ShaderProgram::setBool(const std::string &name, bool value) const {
    //glUniform1i(glGetUniformLocation(shaderID, name.c_str()), (int)value);
    GLuint loc = getLoc(name);
    glUniform1i(loc, (int)value);
}

void ShaderProgram::setInt(const std::string & name, int value) const {
    //glUniform1i(glGetUniformLocation(shaderID, name.c_str()), value);
    int loc = getLoc(name);
    glUniform1i(loc, value);
}

void ShaderProgram::setFloat(const std::string & name, float value) const {
    //glUniform1f(glGetUniformLocation(shaderID, name.c_str()), value);
    int loc = getLoc(name);
    glUniform1f(loc, value);
}

void ShaderProgram::setMat4(const std::string & name, mat4* value) const {
    //glUniformMatrix4fv(glGetUniformLocation(shaderID, name.c_str()), 1, GL_FALSE, &(value->a11));
    int loc = getLoc(name);
    glUniformMatrix4fv(loc, 1, GL_FALSE, &(value->a11));
}

void ShaderProgram::setvec3(const std::string & name, vec3* value) const {
    //glUniform3fv(glGetUniformLocation(shaderID, (name).c_str()), 1, &(value->x));
    int loc = getLoc(name);
    glUniform3fv(loc, 1, &(value->x));
}

void ShaderProgram::setVec4(const std::string & name, float a, float b, float c, float d) const {
    //glUniform3fv(glGetUniformLocation(shaderID, (name).c_str()), 1, &(value->x));
    int loc = getLoc(name);
    glUniform4f(loc, a, b, c, d);
}

void ShaderProgram::setPointLight(const std::string& name, PointLight light) const {
    glUniform3fv(getLoc(name + ".position"), 1, &(light.position.x));
    glUniform4fv(getLoc(name + ".color"), 1, &(light.color.x));
    glUniform1fv(getLoc(name + ".strength"), 1, &(light.strength));
}

void ShaderProgram::setDirectionalLight(const std::string& name, DirectionalLight light) const {
    glUniform3fv(getLoc(name + ".direction"), 1, &(light.direction.x));
    glUniform4fv(getLoc(name + ".color"), 1, &(light.color.x));
    glUniform1fv(getLoc(name + ".strength"), 1, &(light.strength));
}