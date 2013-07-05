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

  void set(Buffer * buffer);
  void set(VertexAttribute * attrib);

  GLVertexArrayObject * createVAO();

  GLsizei vertexCount;
  GLsizei elementCount;
  GLsizei instanceCount;
  Buffer * elementBuffer;
private:
  Buffer * arraybuffer;
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

class TransformFeedbackElement {
public:
  TransformFeedbackElement();
  ~TransformFeedbackElement();

  void set(GLQueryObject * queryobject);
  void set(GLTransformFeedback * transformfeedback);
  void begin();
  void end();

private:
  GLQueryObject * queryobject;
  GLTransformFeedback * transformfeedback;
};