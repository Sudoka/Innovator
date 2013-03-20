#pragma once

#include <GL/glew.h>
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

class GLBufferObject : public Bindable {
public:
  GLBufferObject(GLenum target, GLenum usage, GLsizeiptr size, GLvoid * data = nullptr);
  ~GLBufferObject();

  void * map(GLenum access);
  void unmap();

  virtual void bind();
  virtual void unbind();

  static GLBufferObject * create(GLenum target, GLenum usage, GLenum type, const std::vector<double> & data);

  GLenum target;
  GLuint buffer;
};

class GLVertexArrayObject : public Bindable {
public:
  GLVertexArrayObject();
  ~GLVertexArrayObject();

private:
  virtual void bind();
  virtual void unbind();

  GLuint vao;
};

class GLVertexAttribute : public Bindable {
public:
  GLVertexAttribute(GLuint index, GLint size, GLenum type, GLuint divisor);

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
