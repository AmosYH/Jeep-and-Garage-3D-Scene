#ifndef shader_H
#define shader_H

#define GLEW_STATIC
#include <GL/glew.h>

GLuint initShader(const GLchar* vertexPath, const GLchar* fragmentPath);

#endif