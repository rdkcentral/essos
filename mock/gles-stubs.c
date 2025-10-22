/*
 * OpenGL ES Stub implementations
 * Provides minimal GLES function implementations for linking when OpenGL ES is not available
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// GLES types
typedef unsigned int GLenum;
typedef unsigned char GLboolean;
typedef unsigned int GLbitfield;
typedef signed char GLbyte;
typedef short GLshort;
typedef int GLint;
typedef int GLsizei;
typedef unsigned char GLubyte;
typedef unsigned short GLushort;
typedef unsigned int GLuint;
typedef float GLfloat;
typedef float GLclampf;
typedef double GLdouble;
typedef double GLclampd;
typedef void GLvoid;
typedef char GLchar;

#define GL_NO_ERROR 0
#define GL_INVALID_ENUM 0x0500
#define GL_INVALID_VALUE 0x0501
#define GL_INVALID_OPERATION 0x0502
#define GL_STACK_OVERFLOW 0x0503
#define GL_STACK_UNDERFLOW 0x0504
#define GL_OUT_OF_MEMORY 0x0505

#define GL_FALSE 0
#define GL_TRUE  1

#define GL_VERTEX_SHADER 0x8B31
#define GL_FRAGMENT_SHADER 0x8B30
#define GL_COMPILE_STATUS 0x8B81
#define GL_LINK_STATUS 0x8B82
#define GL_INFO_LOG_LENGTH 0x8B84

#define GL_COLOR_BUFFER_BIT 0x00004000
#define GL_DEPTH_BUFFER_BIT 0x00000100

#define GL_TRIANGLES 0x0004

// Global error state
static GLenum gl_error = GL_NO_ERROR;

// Shader/Program stubs
GLuint glCreateShader(GLenum shaderType)
{
    (void)shaderType;
    return 1;
}

void glShaderSource(GLuint shader, GLsizei count, const GLchar **string, const GLint *length)
{
    (void)shader;
    (void)count;
    (void)string;
    (void)length;
}

void glCompileShader(GLuint shader)
{
    (void)shader;
}

void glGetShaderiv(GLuint shader, GLenum pname, GLint *params)
{
    (void)shader;
    (void)pname;
    if (params) {
        *params = (pname == GL_COMPILE_STATUS) ? GL_TRUE : 0;
    }
}

void glGetShaderInfoLog(GLuint shader, GLsizei maxLength, GLsizei *length, GLchar *infoLog)
{
    (void)shader;
    if (length) {
        *length = 0;
    }
    if (infoLog && maxLength > 0) {
        infoLog[0] = '\0';
    }
}

GLuint glCreateProgram(void)
{
    return 1;
}

void glAttachShader(GLuint program, GLuint shader)
{
    (void)program;
    (void)shader;
}

void glLinkProgram(GLuint program)
{
    (void)program;
}

void glGetProgramiv(GLuint program, GLenum pname, GLint *params)
{
    (void)program;
    (void)pname;
    if (params) {
        *params = (pname == GL_LINK_STATUS) ? GL_TRUE : 0;
    }
}

void glGetProgramInfoLog(GLuint program, GLsizei maxLength, GLsizei *length, GLchar *infoLog)
{
    (void)program;
    if (length) {
        *length = 0;
    }
    if (infoLog && maxLength > 0) {
        infoLog[0] = '\0';
    }
}

void glUseProgram(GLuint program)
{
    (void)program;
}

void glBindAttribLocation(GLuint program, GLuint index, const GLchar *name)
{
    (void)program;
    (void)index;
    (void)name;
}

GLint glGetUniformLocation(GLuint program, const GLchar *name)
{
    (void)program;
    (void)name;
    return 0;
}

// Rendering functions
void glViewport(GLint x, GLint y, GLsizei width, GLsizei height)
{
    (void)x;
    (void)y;
    (void)width;
    (void)height;
}

void glClearColor(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha)
{
    (void)red;
    (void)green;
    (void)blue;
    (void)alpha;
}

void glClear(GLbitfield mask)
{
    (void)mask;
}

void glUniform4f(GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3)
{
    (void)location;
    (void)v0;
    (void)v1;
    (void)v2;
    (void)v3;
}

void glUniformMatrix4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)
{
    (void)location;
    (void)count;
    (void)transpose;
    (void)value;
}

void glVertexAttribPointer(GLuint index, GLint size, GLenum type,
                          GLboolean normalized, GLsizei stride, const GLvoid *pointer)
{
    (void)index;
    (void)size;
    (void)type;
    (void)normalized;
    (void)stride;
    (void)pointer;
}

void glDrawArrays(GLenum mode, GLint first, GLsizei count)
{
    (void)mode;
    (void)first;
    (void)count;
}

void glDisableVertexAttribArray(GLuint index)
{
    (void)index;
}

// Error handling
GLenum glGetError(void)
{
    GLenum err = gl_error;
    gl_error = GL_NO_ERROR;
    return err;
}
