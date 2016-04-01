/*
    krogl::Shader
*/
#ifndef __SHADER_H__
#define __SHADER_H__

#include <GL/glew.h>
#include <string>

namespace krogl{

    class Shader
    {
    public:
        /* Create a shader from std::string source*/
        Shader(const std::string& shaderCode, GLenum shaderType);

        /* Get shader's object ID (returned from glCreateShader */
        GLuint object() const;

        /* Copy constructor */
        Shader(const Shader& rhs);

        /* Asignement operator */
        Shader& operator=(const Shader& rhs);
        ~Shader();
    private:
        GLuint _object;
        unsigned* _refCount;

        void _retain();
        void _release();
    };
}

#endif //__SHADER_H__
