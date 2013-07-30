#include <elements.h>
#include <state.h>
#include <nodes.h>
#include <opengl.h>
#include <iostream>
#include <glm/gtc/type_ptr.hpp>

using namespace glm;
using namespace std;

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

TransformFeedbackElement::TransformFeedbackElement()
  : queryobject(nullptr), transformfeedback(nullptr)
{
}

TransformFeedbackElement::~TransformFeedbackElement()
{
}

void
TransformFeedbackElement::set(GLQueryObject * queryobject)
{
  this->queryobject = queryobject;
}

void 
TransformFeedbackElement::set(GLTransformFeedback * transformfeedback)
{
  this->transformfeedback = transformfeedback;
}

void
TransformFeedbackElement::begin()
{
  if (this->queryobject) {
    this->queryobject->bind();
  }
  if (this->transformfeedback) {
    this->transformfeedback->bind();
  }
}
 
void
TransformFeedbackElement::end()
{
  if (this->queryobject) {
    this->queryobject->unbind();
  }
  if (this->transformfeedback) {
    this->transformfeedback->unbind();
  }
}

