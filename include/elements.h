#pragma once

#include <memory>
#include <vector>
#include <GL/glew.h>
#include <rendercache.h>

class Texture;
class TextureUnit;
class TextureSampler;
class GLVertexArrayObject;

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

private:
  GLuint unit;
  std::vector<Bindable*> statevec;
};
