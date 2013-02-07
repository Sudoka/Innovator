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

class ShaderProgram : public Bindable {
public:
  ShaderProgram();
  ~ShaderProgram();
  void attach(const char * shader, GLenum type);
  void link();
  virtual void bind();
  virtual void unbind();
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

class GLBufferObject : public Bindable {
public:
  GLBufferObject(GLenum target);
  GLBufferObject(GLenum target, GLenum usage, GLsizeiptr size, GLvoid * data = nullptr);
  ~GLBufferObject();

  void setValues(GLenum usage, GLsizeiptr size, const GLvoid * data);
  void set1Value(int index, GLuint value);

  virtual void bind();
  virtual void unbind();

  GLenum target;
  GLuint buffer;
};

class GLVertexAttribute : public GLBufferObject {
public:
  GLVertexAttribute(const std::vector<glm::vec3> & data, GLuint index, GLuint divisor = 0);
  ~GLVertexAttribute();

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
