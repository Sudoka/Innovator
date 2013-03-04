#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>
#include <node.h>

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
  GLuint id;
private:
  virtual void bind();
  virtual void unbind();
};

class GLBufferObject : public Bindable {
public:
  GLBufferObject(GLenum target);
  GLBufferObject(GLenum target, GLenum usage, std::vector<glm::vec3> & data);
  GLBufferObject(GLenum target, GLenum usage, std::vector<glm::ivec3> & data);
  ~GLBufferObject();

  void construct(GLenum target, GLenum usage, GLsizeiptr size, GLvoid * data);

  virtual void bind();
  virtual void unbind();

  GLenum target;
  GLuint buffer;
  GLuint count;
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
