#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>
#include <glstate.h>

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

class ShaderProgram {
public:
  ShaderProgram();
  ~ShaderProgram();
  void attach(const char * shader, GLenum type);
  void link();
  GLuint id;
};

class BindBufferBase {
public:
  BindBufferBase(GLenum target, GLuint index, GLuint buffer)
    : target(target) {
    glBindBufferBase(this->target, index, buffer);
  }
  ~BindBufferBase() {
    glBindBuffer(this->target, 0);
  }
private:
  GLenum target;
};

class BindBufferRange {
public:
  BindBufferRange(GLenum target, GLuint index, GLuint buffer, GLintptr offset, GLsizeiptr size)
    : target(target) {
    glBindBufferRange(this->target, index, buffer, offset, size);
  }
  ~BindBufferRange() {
    glBindBuffer(this->target, 0);
  }
private:
  GLenum target;
};

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

class BufferObject : public Bindable {
public:
  BufferObject(GLenum target);
  BufferObject(GLenum target, GLenum usage, GLsizeiptr size, GLvoid * data = nullptr);
  BufferObject(const std::vector<glm::vec3> & data, GLenum target = GL_ARRAY_BUFFER, GLenum usage = GL_STATIC_DRAW);
  BufferObject(const std::vector<glm::ivec3> & data, GLenum target = GL_ELEMENT_ARRAY_BUFFER, GLenum usage = GL_STATIC_DRAW);
  ~BufferObject();

  void setValues(GLenum usage, GLsizeiptr size, const GLvoid * data);
  void set1Value(int index, GLuint value);

  virtual void bind();
  virtual void unbind();

private:
  void construct(GLenum target, GLenum usage, GLsizeiptr size, const GLvoid * data);

public:
  BufferState state;
};

class VertexBuffer : public BufferObject {
public:
  VertexBuffer(const std::vector<glm::vec3> & data, GLuint index, GLuint divisor = 0);
  ~VertexBuffer();

  virtual void bind();
  virtual void unbind();

private:
  GLuint index;
  GLuint divisor;
};

class TransformFeedback : public Bindable {
public:
  TransformFeedback(GLuint buffer, GLenum mode = GL_POINTS);
  ~TransformFeedback();

  virtual void bind();
  virtual void unbind();

public:
  GLuint id;
  GLenum mode;
};
