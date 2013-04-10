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
    if (this->bindable) {
      this->bindable->bind();
    }
  }
  ~BindScope() {
    if (this->bindable) {
      this->bindable->unbind();
    }
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

  static GLBufferObject * create(GLenum target, GLenum usage, GLenum type, GLuint count, std::vector<double> & data);

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
  GLTransformFeedback(GLenum mode, GLuint index, GLuint buffer);
  ~GLTransformFeedback();

  virtual void bind();
  virtual void unbind();

public:
  GLuint id;
  GLenum mode;
  GLuint index;
  GLuint buffer;
};

class GLQueryObject : public Bindable {
public:
  GLQueryObject(GLenum target);
  ~GLQueryObject();

  GLuint getResult();

  virtual void bind();
  virtual void unbind();

private:
  GLuint query;
  GLenum target;
};
