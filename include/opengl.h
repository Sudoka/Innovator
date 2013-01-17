#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>
#include <memory>

class VertexBuffer;
class VertexBufferScope;

class ShaderProgram {
public:
  ShaderProgram();
  ShaderProgram(const std::string & filename);
  ~ShaderProgram();
  GLuint id;
};

class VertexBufferScope {
public:
  VertexBufferScope(VertexBuffer * buffer);
  ~VertexBufferScope();
private:
  VertexBuffer * buffer;
};

class VertexBuffer {
  friend class VertexBufferScope;
public:
  VertexBuffer(const std::vector<glm::vec3> & datasource, GLuint index, GLuint divisor = 0);
  VertexBuffer(const int num_data, GLuint index, GLuint divisor);
  ~VertexBuffer();

private:
  void bind();
  void unbind();
  void bindTransformFeedback();
  void unbindTransformFeedback();
  void construct(const glm::vec3 * data, const int num_data);

private:
  GLuint index;
  GLuint buffer;
  GLuint divisor;
  GLuint feedback_buffer;
};