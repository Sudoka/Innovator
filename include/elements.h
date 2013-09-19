#pragma once

#include <memory>
#include <vector>
#include <GL/glew.h>

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
