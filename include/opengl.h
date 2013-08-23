#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <map>
#include <string>
#include <vector>

class State;

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

class ShaderObject;

class GLProgram : public Bindable {
public:
  GLProgram(const std::vector<std::shared_ptr<ShaderObject>> & shaderobjects);
  ~GLProgram();
  void attach(const char * shader, GLenum type);
  void link();
  virtual void bind();
  virtual void unbind();
  GLint getUniformLocation(const std::string & name);
  GLuint id;
private:
  std::map<std::string, GLint> uniformLocations;
};

class GLBufferObject : public Bindable {
public:
  GLBufferObject(GLenum target, GLenum usage, GLsizeiptr size, GLvoid * data = nullptr);
  ~GLBufferObject();

  void * map(GLenum access);
  void unmap();

  void bindBufferBase(GLuint bindingPoint);
  void bufferSubData(GLintptr offset, GLsizeiptr size, const GLvoid * data);

  virtual void bind();
  virtual void unbind();

  static GLBufferObject * create(GLenum target, GLenum usage, GLenum type, GLuint count, std::vector<double> & data);

  GLenum target;
  GLuint buffer;
};

class GLUniformBuffer {
public:
  GLUniformBuffer(GLuint blockbinding, GLsizeiptr count = 1);
  ~GLUniformBuffer();

  void updateGL(GLvoid * data, GLsizeiptr size, GLuint index = 0);
  void bindBuffer();
private:
  GLuint blockbinding;
  std::unique_ptr<GLBufferObject> buffer;
};

class GLVertexArrayObject : public Bindable {
public:
  GLVertexArrayObject();
  ~GLVertexArrayObject();

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

class GLDrawCall {
public:
  GLDrawCall();
  ~GLDrawCall();

  virtual void execute() = 0;
};

class GLDrawElements : public GLDrawCall {
public:
  GLDrawElements(GLenum mode, GLsizei count, GLenum type);
  ~GLDrawElements();

  virtual void execute();

  GLenum mode;
  GLenum type;
  GLsizei count;
};

class GLUniformBufferFeedback : public Bindable {
public:
  GLUniformBufferFeedback(GLenum mode, GLuint index, GLuint buffer);
  ~GLUniformBufferFeedback();

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

class GLTextureUnit : public Bindable {
public:
  GLTextureUnit(GLuint unit);
  ~GLTextureUnit();

  virtual void bind();
  virtual void unbind();

  GLuint unit;
};

class GLTextureObject : public Bindable {
public:
  GLTextureObject(GLenum target, 
                  GLint level, 
                  GLint internalFormat, 
                  GLsizei width, 
                  GLsizei height, 
                  GLint border, 
                  GLenum format, 
                  GLenum type, 
                  const GLvoid * data);
  ~GLTextureObject();

  virtual void bind();
  virtual void unbind();

  GLuint id;
  GLenum target;
};

class GLTextureSampler : public Bindable {
public:
  GLTextureSampler(GLint wraps, 
                   GLint wrapt, 
                   GLint wrapr, 
                   GLint min_filter, 
                   GLint mag_filter);
  ~GLTextureSampler();

  void setUnit(GLuint unit);

  virtual void bind();
  virtual void unbind();

  GLuint id;
  GLuint unit;
};

class GLFramebufferObject : public Bindable {
public:
  GLFramebufferObject();
  ~GLFramebufferObject();

  void attach(GLenum attachment, GLuint texture);
  void checkStatus();

  virtual void bind();
  virtual void unbind();

  GLuint id;
};