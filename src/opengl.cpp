#include <opengl.h>
#include <innovator.h>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <string>
#include <assert.h>

using namespace glm;
using namespace std;

GLBufferObject::GLBufferObject(GLenum target, GLenum usage, GLsizeiptr size, GLvoid * data)
  : target(target)
{
  glGenBuffers(1, &this->buffer);
  glBindBuffer(this->target, this->buffer);
  glBufferData(this->target, size, data, usage);
  glBindBuffer(this->target, 0);
}

GLBufferObject::~GLBufferObject()
{
  glBindBuffer(this->target, 0);
  glDeleteBuffers(1, &this->buffer);
}

void
GLBufferObject::bind()
{
  glBindBuffer(this->target, this->buffer);
}

void
GLBufferObject::unbind()
{
  glBindBuffer(this->target, 0);
}

// *************************************************************************************************

GLVertexAttribute::GLVertexAttribute(GLuint index, GLuint divisor)
  : index(index), 
    divisor(divisor),
    size(3),
    type(GL_FLOAT)
{
}

GLVertexAttribute::~GLVertexAttribute()
{
}

void
GLVertexAttribute::bind()
{
  glEnableVertexAttribArray(this->index);
  glVertexAttribPointer(this->index, this->size, this->type, GL_FALSE, 0, 0);
  glVertexAttribDivisor(this->index, this->divisor);
}

void
GLVertexAttribute::unbind()
{
  glDisableVertexAttribArray(this->index);
}


// *************************************************************************************************

GLTransformFeedback::GLTransformFeedback(GLuint buffer, GLenum mode)
  : mode(mode)
{
  glGenTransformFeedbacks(1, &this->id);
  glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, this->id);
  glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, buffer);
  glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);

}

GLTransformFeedback::~GLTransformFeedback()
{
  glDeleteTransformFeedbacks(1, &this->id);
}

void
GLTransformFeedback::bind()
{
  glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, this->id);
  glEnable(GL_RASTERIZER_DISCARD);
  glBeginTransformFeedback(this->mode);
}

void
GLTransformFeedback::unbind()
{
  glEndTransformFeedback();
  glDisable(GL_RASTERIZER_DISCARD);
  glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);
}

// *************************************************************************************************

GLProgram::GLProgram() 
  : id(glCreateProgram())
{
}

GLProgram::~GLProgram()
{
  glDeleteProgram(this->id);
}

void 
GLProgram::attach(const char * source, GLenum type)
{
  GLuint shader = glCreateShader(type);
  glShaderSource(shader, 1, &source, NULL);
  glCompileShader(shader);
  
  GLint status;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
  if (status != GL_TRUE) {
    GLint length;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
    GLchar * log = new GLchar[length + 1];
    glGetShaderInfoLog(shader, length, NULL, log);
    Innovator::postError("Shader::attach: failed to compile shader: " + string(log));
    delete [] log;
  } else {
    glAttachShader(this->id, shader);
    glDeleteShader(shader);
  }
}

void
GLProgram::link()
{
  glLinkProgram(this->id);
  
  GLint status;
  glGetProgramiv(this->id, GL_LINK_STATUS, &status);
  if (status != GL_TRUE) {
    GLint length;
    glGetProgramiv(this->id, GL_INFO_LOG_LENGTH, &length);
    GLchar * log = new GLchar[length + 1];
    glGetProgramInfoLog(this->id, length, NULL, log);
    Innovator::postError("Shader::link(): failed to link program: " + string(log));
    delete [] log;
  }
}

void
GLProgram::bind()
{
  glUseProgram(this->id);
}

void
GLProgram::unbind()
{
  glUseProgram(0);
}
