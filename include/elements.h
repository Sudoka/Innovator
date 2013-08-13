#pragma once

#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <opengl.h>
#include <fields.h>
#include <memory.h>
#include <functional>

class Shape;
class State;
class Buffer;
class Texture;
class DrawCall;
class TextureUnit;
class TextureSampler;
class FeedbackBuffer;

class VertexAttribute;
class GLVertexArrayObject;
class GLTransformFeedback;

class DrawElement {
public:
  DrawElement();
  ~DrawElement();
  DrawCall * drawcall;
};

class ProgramElement {
public:
  ProgramElement();
  ~ProgramElement();
  GLuint program;
};

class ViewMatrixElement {
public:
  ViewMatrixElement();
  ~ViewMatrixElement();
  glm::mat4 matrix;
};

class ProjectionMatrixElement {
public:
  ProjectionMatrixElement();
  ~ProjectionMatrixElement();
  glm::mat4 matrix;
};

class TransformElement {
public:
  TransformElement();
  ~TransformElement();
  void mult(const glm::mat4 & mat);
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

class CacheElement {
public:
  CacheElement();
  ~CacheElement();

  void push();
  std::function<void()> pop();
  void append(std::function<void()> draw);

  int depth;
  bool isCreatingCache;
  std::vector<std::function<void()>> drawlist;
};