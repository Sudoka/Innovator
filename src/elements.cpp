#include <elements.h>
#include <state.h>
#include <nodes.h>
#include <opengl.h>
#include <iostream>
#include <glm/gtc/type_ptr.hpp>

using namespace glm;
using namespace std;

void
UniformElement::add(Uniform * uniform)
{
  this->uniforms.push_back(uniform);
}

void
UniformElement::flush(State * state)
{
  for each (Uniform * uniform in this->uniforms) {
    uniform->flush(state);
  }
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

void
TransformElement::flush(State * state)
{
  glUniformMatrix4fv(state->program->getUniformLocation("ModelMatrix"), 1, GL_FALSE, glm::value_ptr(this->matrix));
}

VertexElement::VertexElement()
  : arraybuffer(nullptr),
    elementbuffer(nullptr),
    instancebuffer(nullptr)
{
}

VertexElement::~VertexElement() 
{
}

void
VertexElement::set(Buffer * buffer)
{
  switch (buffer->target.value) {
  case GL_ELEMENT_ARRAY_BUFFER: {
    this->elementbuffer = buffer;
  } break;
  case GL_ARRAY_BUFFER:
    this->arraybuffer = buffer;
    break;
  default:
    throw std::invalid_argument("Invalid buffer type");
  }
  this->statevec.push_back(buffer->buffer.get());
}

void 
VertexElement::set(VertexAttribute * attrib)
{
  if (attrib->index.value == 0) {
    this->vertexbuffer = this->arraybuffer;
  }
  if (attrib->divisor.value == 1) {
    this->instancebuffer = this->arraybuffer;
  }
  this->statevec.push_back(attrib->glattrib.get());
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