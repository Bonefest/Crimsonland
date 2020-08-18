#ifndef PROGRAM_H_INCLUDED
#define PROGRAM_H_INCLUDED

#include "glad/glad.h"
#include <GL/gl.h>

#include <fstream>
#include <sstream>
#include <string>

class Program {
public:
    Program(const std::string& vertexShaderFile = "",
            const std::string& fragmentShaderFile = "");

    bool generateProgram(const std::string& vertexShaderFile,
                         const std::string& fragmentShaderFile);

    GLuint getProgramID() { return _program; }
    bool hasError() const { return _hasError; }
    const std::string& getErrorMessage() { return _errorMessage; }

private:
    GLuint createShader(GLenum type, const std::string& source);
    bool checkShaderCompilationStatus(GLuint shader);
    bool checkProgramCompilationStatus();

    std::string getSourceFromFile(const std::string& fileName);

    GLuint _program;
    bool _hasError;
    std::string _errorMessage;
};


#endif // PROGRAM_H_INCLUDED
