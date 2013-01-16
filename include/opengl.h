#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>

class ShaderProgram {
public:
  ShaderProgram();

  void createProgram(const std::string & filename);
  void deleteProgram();

  GLuint id;
};

class ArrayBuffer {
public:
  class Scope {
    ArrayBuffer & buffer;
  public:
    Scope(ArrayBuffer & buffer);
    ~Scope();
    void draw();
  };

  ArrayBuffer();
  ~ArrayBuffer();

  void createBuffer(const std::vector<glm::vec3> & data, GLuint index = 0, GLuint divisor = 0);
  void deleteBuffer();

private:
  friend class Scope;
  void bind();
  void draw();
  void unbind();

  GLuint index;
  GLuint buffer;
  GLuint divisor;
};
