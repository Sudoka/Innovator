#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>

class Bindable {
public:
  virtual void bind() = 0;
  virtual void unbind() = 0;
};

class BindScope {
public:
  BindScope(Bindable * b) : bindable(b) {
    this->bindable->bind();
  }
  ~BindScope() {
    this->bindable->unbind();
  }
private:
  Bindable * bindable;
};

struct DrawElementsIndirectBuffer
{
  DrawElementsIndirectBuffer(GLuint count, 
                             GLuint primCount = 0, 
                             GLuint firstIndex = 0, 
                             GLint baseVertex = 0)
    : count(count), 
      primCount(primCount),
      firstIndex(firstIndex),
      baseVertex(baseVertex),
      reservedMustBeZero(0) 
  {}
  ~DrawElementsIndirectBuffer() {}

  GLuint count;
  GLuint primCount;
  GLuint firstIndex;
  GLint baseVertex;
  GLuint reservedMustBeZero;
};

class GLProgram : public Bindable {
public:
  GLProgram();
  ~GLProgram();
  void attach(const char * shader, GLenum type);
  void link();
  GLuint id;
private:
  virtual void bind();
  virtual void unbind();
};

template <typename T>
class GLBufferObject : public Bindable {
public:
  GLBufferObject(GLenum target, GLenum usage, std::vector<T> & data) 
    : target(target) {
    glGenBuffers(1, &this->buffer);
    glBindBuffer(this->target, this->buffer);
    glBufferData(this->target, sizeof(T) * data.size(), data.data(), usage);
    glBindBuffer(this->target, 0);
  }
  ~GLBufferObject() {
    glBindBuffer(this->target, 0);
    glDeleteBuffers(1, &this->buffer);
  }
  virtual void bind() {
    glBindBuffer(this->target, this->buffer);
  }
  virtual void unbind() {
    glBindBuffer(this->target, 0);
  }
  GLenum target;
  GLuint buffer;
};

class GLVertexAttribute : public Bindable {
public:
  GLVertexAttribute(GLuint index, GLuint divisor);
  ~GLVertexAttribute();

  virtual void bind();
  virtual void unbind();

  GLuint index;
  GLuint divisor;
  GLenum type;
  GLint size;
};

class GLTransformFeedback : public Bindable {
public:
  GLTransformFeedback(GLuint buffer, GLenum mode = GL_POINTS);
  ~GLTransformFeedback();

  virtual void bind();
  virtual void unbind();

public:
  GLuint id;
  GLenum mode;
};
