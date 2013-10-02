#pragma once

#include <GL/glew.h>

#include <memory>
#include <vector>

class Texture;
class TextureUnit;
class TextureSampler;

class TextureElement {
public:
  TextureElement();
  ~TextureElement();

  void set(Texture * texture);
  void set(TextureUnit * unit);
  void set(TextureSampler * sampler);

private:
  GLuint unit;
};
