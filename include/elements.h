#pragma once

#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <opengl.h>
#include <fields.h>
#include <memory.h>

class State;
class Buffer;
class Uniform;
class Texture;
class TextureUnit;
class TextureSampler;
class VertexAttribute;
class GLVertexArrayObject;

class UniformElement {
public:
  void flush(State * state);
  void add(Uniform * uniform);
private:
  std::vector<Uniform*> uniforms;
};

class TransformElement {
public:
  TransformElement();
  ~TransformElement();
  void mult(const glm::mat4 & mat);
  void flush(State * state);
  glm::mat4 matrix;
};

class VertexElement {
public:
  VertexElement();
  ~VertexElement();

  void set(Buffer * buffer);
  void set(VertexAttribute * attrib);

  GLVertexArrayObject * createVAO();

  Buffer * arraybuffer;
  Buffer * vertexbuffer;
  Buffer * elementbuffer;
  Buffer * instancebuffer;
private:
  std::vector<Bindable*> statevec;
};

class TextureElement {
public:
  TextureElement();
  ~TextureElement();

  void set(Texture * texture);
  void set(TextureUnit * unit);
  void set(TextureSampler * sampler);

  void flush(State * state);
private:
  GLuint unit;
  std::vector<Bindable*> statevec;
};