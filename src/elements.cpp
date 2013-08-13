#include <elements.h>
#include <state.h>
#include <nodes.h>
#include <opengl.h>
#include <iostream>
#include <glm/gtc/type_ptr.hpp>

using namespace glm;
using namespace std;

DrawElement::DrawElement()
  : drawcall(nullptr)
{
}

DrawElement::~DrawElement()
{
}

ProgramElement::ProgramElement()
  : program(0)
{
}

ProgramElement::~ProgramElement()
{
}

ViewMatrixElement::ViewMatrixElement()
  : matrix(1.0)
{
}

ViewMatrixElement::~ViewMatrixElement()
{
}

ProjectionMatrixElement::ProjectionMatrixElement()
  : matrix(1.0)
{
}

ProjectionMatrixElement::~ProjectionMatrixElement()
{
}

TransformElement::TransformElement()
  : matrix(1.0)
{
}

TransformElement::~TransformElement()
{
}

void
TransformElement::mult(const mat4 & mat)
{
  this->matrix *= mat;
}

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

void
TextureElement::flush(State * state)
{
  for each (Bindable * bindable in this->statevec) {
    bindable->bind();
  }
}

CacheElement::CacheElement()
  : depth(0),
    isCreatingCache(false)
{

}

CacheElement::~CacheElement()
{

}

void
CacheElement::push()
{
  if (this->depth == 0) {
    this->isCreatingCache = true;
    this->drawlist.clear();
  }
  this->depth++;
}

std::function<void()>
CacheElement::pop()
{
  this->depth--;
  if (this->depth == 0) {
    this->isCreatingCache = false;
    vector<function<void()>> drawlist = this->drawlist;
    return [=]() {
      for each (std::function<void()> draw in drawlist) {
        draw();
      }
    };
  }
  return nullptr;
}

void
CacheElement::append(std::function<void()> draw)
{
  this->drawlist.push_back(draw);
}

