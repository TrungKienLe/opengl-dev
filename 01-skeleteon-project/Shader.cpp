/*
    krogl::Shader implement
*/

#include "Shader.h"
#include <stdexcept>
#include <string>
#include <cassert>

using namespace krogl;

Shader::Shader(const std::string& shaderSource, GLenum shaderType):
    _object(0),
    _refCount(NULL) {
    //Create the shader object
    _object = glCreateShader(shaderType);
    if (_object == 0)
        throw std::runtime_error("glCreateShader failed");

    //set the source code
    const char* source = shaderSource.c_str();
    glShaderSource(_object, 1, (const GLchar**)&source, NULL);

    // compile
    glCompileShader(_object);

    // throw exception if compile error occourred
    GLint status;
    glGetShaderiv(_object, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE) {
        std::string msg("Compile failure in shader:\n");

        GLint infoLogLength;
        glGetShaderiv(_object, GL_INFO_LOG_LENGTH, &infoLogLength);
        char* strInfoLog = new char[infoLogLength + 1];
        glGetShaderInfoLog(_object, infoLogLength, NULL, strInfoLog);
        msg += strInfoLog;
        delete[] strInfoLog;

        glDeleteShader(_object); _object = 0;
        throw std::runtime_error(msg);
    }

    _refCount = new unsigned;
    *_refCount = 1;
}


Shader::Shader(const Shader& other) :
    _object(other._object),
    _refCount(other._refCount) {
    _retain();
}

GLuint Shader::object() const {
    return _object;
}

Shader& Shader::operator = (const Shader& other) {
    _release();
    _object = other._object;
    _refCount = other._refCount;
    _retain();
    return *this;
}

Shader::~Shader() {
    //_refCount will be NULL if constructor failed and threw an exception
    if (_refCount) _release();
}

void Shader::_retain() {
    assert(_refCount);
    *_refCount += 1;
}

void Shader::_release() {
    assert(_refCount && *_refCount > 0);
    *_refCount -= 1;
    if (*_refCount == 0) {
        glDeleteShader(_object); _object = 0;
        delete _refCount; _refCount = NULL;
    }
}

