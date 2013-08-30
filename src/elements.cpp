#include <elements.h>
#include <state.h>
#include <nodes.h>
#include <opengl.h>
#include <iostream>
#include <glm/gtc/type_ptr.hpp>

using namespace glm;
using namespace std;

VertexElement::VertexElement()
{
}

VertexElement::~VertexElement() 
{
}

void
VertexElement::set(Bindable * bindable)
{
  this->statevec.push_back(bindable);
}

GLVertexArrayObject * 
VertexElement::createVAO()
{
  GLVertexArrayObject * vao = new GLVertexArrayObject;
  vao->bind();

  for each (Bindable * bindable in this->statevec) {
    bindable->bind();
  }
  vao->unbind();

  for each (Bindable * bindable in this->statevec) {
    bindable->unbind();
  }
  return vao;
}

TextureElement::TextureElement()
  : unit(0)
{
}

TextureElement::~TextureElement()
{
}

void
TextureElement::set(Texture * texture)
{
  this->statevec.push_back(texture->gltexture.get());
}

void
TextureElement::set(TextureUnit * texunit)
{
  this->unit = texunit->unit.value;
  this->statevec.push_back(texunit->gltexunit.get());
}

void
TextureElement::set(TextureSampler * sampler)
{
  sampler->glsampler->setUnit(this->unit);
  this->statevec.push_back(sampler->glsampler.get());
}