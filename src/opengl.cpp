#include <opengl.h>
#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <assert.h>

using namespace glm;
using namespace std;

BufferObject::BufferObject(GLenum target)
  : target(target)
{
  glGenBuffers(1, &this->buffer);
}

BufferObject::BufferObject(GLenum target, GLenum usage, GLsizeiptr size, GLvoid * data)
  : target(target)
{
  this->construct(target, usage, size, data);
}

BufferObject::BufferObject(const std::vector<glm::vec3> & data, GLenum target, GLenum usage)
  : target(target)
{
  this->construct(target, usage, sizeof(vec3) * data.size(), data.data());
}

BufferObject::BufferObject(const std::vector<ivec3> & data, GLenum target, GLenum usage)
  : target(target)
{
  this->construct(target, usage, sizeof(ivec3) * data.size(), data.data());
}

void
BufferObject::construct(GLenum target, GLenum usage, GLsizeiptr size, const GLvoid * data)
{
  glGenBuffers(1, &this->buffer);
  this->setValues(usage, size, data);
}

BufferObject::~BufferObject()
{
  glBindBuffer(this->target, this->buffer);
  glDeleteBuffers(1, &this->buffer);
}

void
BufferObject::setValues(GLenum usage, GLsizeiptr size, const GLvoid * data)
{
  this->bind();
  glBufferData(this->target, size, data, usage);
  this->unbind();
}

void
BufferObject::set1Value(int index, GLuint value)
{
  this->bind();
  glBufferSubData(this->target, sizeof(GLuint) * index, sizeof(GLuint), &value);
  this->unbind();
}

void
BufferObject::bind()
{
  glBindBuffer(this->target, this->buffer);
}

void
BufferObject::unbind()
{
  glBindBuffer(this->target, 0);
}

VertexBuffer::VertexBuffer(const std::vector<vec3> & data, GLuint index, GLuint divisor)
  : index(index),
    divisor(divisor),
    BufferObject(data, GL_ARRAY_BUFFER, GL_STATIC_DRAW)
{
}

VertexBuffer::~VertexBuffer()
{
}

void
VertexBuffer::bind()
{
  BufferObject::bind();
  glEnableVertexAttribArray(this->index);
  glVertexAttribPointer(this->index, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glVertexAttribDivisor(this->index, this->divisor);
}

void
VertexBuffer::unbind()
{
  BufferObject::unbind();
  glDisableVertexAttribArray(this->index);
}

TransformFeedback::TransformFeedback(GLuint buffer, GLenum mode)
  : mode(mode)
{
  glGenTransformFeedbacks(1, &this->id);
  glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, this->id);
  glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, buffer);
  glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);

}

TransformFeedback::~TransformFeedback()
{
  glDeleteTransformFeedbacks(1, &this->id);
}

void
TransformFeedback::bind()
{
  glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, this->id);
  glEnable(GL_RASTERIZER_DISCARD);
  glBeginTransformFeedback(this->mode);
}

void
TransformFeedback::unbind()
{
  glEndTransformFeedback();
  glDisable(GL_RASTERIZER_DISCARD);
  glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);
}

// *************************************************************************************************

ShaderProgram::ShaderProgram() 
  : id(glCreateProgram())
{
}

ShaderProgram::~ShaderProgram()
{
  glDeleteProgram(this->id);
}

void 
ShaderProgram::attach(const char * source, GLenum type)
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
    cout << "Shader::attach" << "failed to compile shader: " << log << endl;
    delete [] log;
  } else {
    glAttachShader(this->id, shader);
    glDeleteShader(shader);
  }
}

void
ShaderProgram::link()
{
  glLinkProgram(this->id);
  
  GLint status;
  glGetProgramiv(this->id, GL_LINK_STATUS, &status);
  if (status != GL_TRUE) {
    GLint length;
    glGetProgramiv(this->id, GL_INFO_LOG_LENGTH, &length);
    GLchar * log = new GLchar[length + 1];
    glGetProgramInfoLog(this->id, length, NULL, log);
    cout << "Shader::link()" << "failed to link program: " <<  log << endl;
    delete [] log;
  }
}
