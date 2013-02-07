#include <opengl.h>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <string>
#include <assert.h>

using namespace glm;
using namespace std;

GLBufferObject::GLBufferObject(GLenum target)
  : target(target)
{
  glGenBuffers(1, &buffer);
}

GLBufferObject::GLBufferObject(GLenum target, GLenum usage, GLsizeiptr size, GLvoid * data)
  : target(target)
{
  glGenBuffers(1, &this->buffer);
  this->setValues(usage, size, data);
}

GLBufferObject::~GLBufferObject()
{
  glBindBuffer(this->target, this->buffer);
  glDeleteBuffers(1, &this->buffer);
}

void
GLBufferObject::setValues(GLenum usage, GLsizeiptr size, const GLvoid * data)
{
  this->bind();
  glBufferData(this->target, size, data, usage);
  this->unbind();
}

void
GLBufferObject::set1Value(int index, GLuint value)
{
  this->bind();
  glBufferSubData(this->target, sizeof(GLuint) * index, sizeof(GLuint), &value);
  this->unbind();
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

GLVertexAttribute::GLVertexAttribute(const std::vector<vec3> & data, GLuint index, GLuint divisor)
  : index(index),
    divisor(divisor),
    GLBufferObject(GL_ARRAY_BUFFER, GL_STATIC_DRAW, sizeof(vec3) * data.size(), (GLvoid*)data.data())
{
}

GLVertexAttribute::~GLVertexAttribute()
{
}

void
GLVertexAttribute::bind()
{
  GLBufferObject::bind();
  glEnableVertexAttribArray(this->index);
  glVertexAttribPointer(this->index, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glVertexAttribDivisor(this->index, this->divisor);
}

void
GLVertexAttribute::unbind()
{
  GLBufferObject::unbind();
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
  if (source == nullptr) return;

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

void
ShaderProgram::bind()
{
  glUseProgram(this->id);
}

void
ShaderProgram::unbind()
{
  glUseProgram(0);
}
