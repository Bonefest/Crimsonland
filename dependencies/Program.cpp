#include "Program.h"

Program::Program(const std::string& vpath, const std::string& fpath):_hasError(false) {
    generateProgram(vpath, fpath);
}
#include <iostream>
bool Program::generateProgram(const std::string& vpath, const std::string& fpath) {
    if(vpath == "" || fpath == "") return false;

    GLuint vertexShader = createShader(GL_VERTEX_SHADER, getSourceFromFile(vpath));
    if(!checkShaderCompilationStatus(vertexShader)) return false;

    GLuint fragmentShader = createShader(GL_FRAGMENT_SHADER, getSourceFromFile(fpath));
    if(!checkShaderCompilationStatus(fragmentShader)) return false;

    _program = glCreateProgram();
    glAttachShader(_program, vertexShader);
    glAttachShader(_program, fragmentShader);
    glLinkProgram(_program);
    if(!checkProgramCompilationStatus()) return false;

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return true;
}

GLuint Program::createShader(GLenum type, const std::string& source) {
    GLuint shader = glCreateShader(type);

    const char* csource = source.c_str();
    glShaderSource(shader, 1, &csource, NULL);
    glCompileShader(shader);

    return shader;
}

std::string Program::getSourceFromFile(const std::string& fileName) {
    std::ifstream sourceFile(fileName);
    std::stringstream sstream;
    sstream << sourceFile.rdbuf();
    sourceFile.close();
    return sstream.str();
}

bool Program::checkShaderCompilationStatus(GLuint shader) {
    int status;
    int messageLength = 0;
    char statusMessage[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if(status != GL_TRUE) {
        glGetShaderInfoLog(shader, 512, &messageLength, statusMessage);
        _hasError = true;
        _errorMessage = statusMessage;
    }

    return (status == GL_TRUE);
}

bool Program::checkProgramCompilationStatus() {
    int status;
    int messageLength = 0;
    char statusMessage[512];

    glGetProgramiv(_program, GL_LINK_STATUS, &status);
    if(status != GL_TRUE) {
        glGetProgramInfoLog(_program, 512, &messageLength, statusMessage);
        _hasError = true;
        _errorMessage = statusMessage;
    }

    return status == GL_TRUE;
}
