#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>
#include <memory>

class ShaderProgram {
public:
  ShaderProgram();
  void createProgram(const std::string & filename);
  void deleteProgram();
  GLuint id;
};

class VertexBufferObject {
public:
  VertexBufferObject();
  ~VertexBufferObject();

  void createBuffer(GLuint index, const int num_data);
  void createBuffer(const std::vector<glm::vec3> & data, GLuint index = 0, GLuint divisor = 0);
  void deleteBuffers();

  void bind();
  void unbind();

private:
  class VertexBufferObjectP;
  std::unique_ptr<VertexBufferObjectP> self;
};