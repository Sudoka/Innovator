#include <elements.h>
#include <state.h>
#include <nodes.h>
#include <glm/gtc/type_ptr.hpp>

using namespace glm;
using namespace std;

ViewportElement::ViewportElement()
  : origin(-1), size(-1)
{
}

void
ViewportElement::updateGL(State * state)
{
  if (size == ivec2(-1) || origin == ivec2(-1)) {
    throw std::invalid_argument("invalid viewport");
  }
  glViewport(origin.x, origin.y, size.x, size.y);
}

void
MatrixElement::updateGL(State * state)
{
  GLuint loc = glGetUniformLocation(state->program->getProgramId(), this->name.c_str());
  glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(this->matrix));
}

AttributeElement::AttributeElement()
  : indices(nullptr),
    instancecount(0)
{
  this->attributes.resize(8);
  std::fill(this->attributes.begin(), this->attributes.end(), nullptr);
}

AttributeElement::~AttributeElement()
{

}

void
AttributeElement::set(VertexAttribute * attribute)
{
  assert(attribute->index.value >= 0 && attribute->index.value < this->attributes.size());
  if (attribute->divisor.value == 1) {
    if (this->instancecount > 0) { 
      assert(this->instancecount == attribute->values.vec.size());
    } else {
      this->instancecount = attribute->values.vec.size();
    }
  }
  this->attributes[attribute->index.value] = attribute;
}

void
AttributeElement::set(IndexBuffer * indices)
{
  this->indices = indices;
}

IndexBuffer * 
AttributeElement::getIndexBuffer() const
{
  return this->indices;
}

unsigned int
AttributeElement::getInstanceCount() const
{
  return this->instancecount;
}

VertexAttribute * 
AttributeElement::get(const int index) const
{
  assert (this->attributes.size() > (size_t) index);
  return this->attributes[index];
}

void
AttributeElement::bind()
{
  if (this->indices) {
    this->indices->bind();
  }
  for (unsigned int i = 0; i < this->attributes.size(); i++) {
    if (this->attributes[i]) {
      this->attributes[i]->bind();
    }
  }
}

void
AttributeElement::unbind()
{
  if (this->indices) {
    this->indices->unbind();
  }
  for (unsigned int i = 0; i < this->attributes.size(); i++) {
    if (this->attributes[i]) {
      this->attributes[i]->unbind();
    }
  }
}
