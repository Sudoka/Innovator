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
class Texture;
class TextureUnit;
class TextureSampler;
class FeedbackBuffer;

class VertexAttribute;
class GLVertexArrayObject;
class GLTransformFeedback;

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

  void set(Bindable * bindable);
  GLVertexArrayObject * createVAO();

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
