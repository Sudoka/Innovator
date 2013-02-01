#include <elements.h>
#include <state.h>
#include <nodes.h>
#include <glm/gtc/type_ptr.hpp>

using namespace glm;
using namespace std;

AttributeElement::AttributeElement()
  : indices(nullptr)
{
  this->attributes.resize(10);
  std::fill(this->attributes.begin(), this->attributes.end(), nullptr);
}

AttributeElement::~AttributeElement()
{

}

void
AttributeElement::set(VertexAttribute * attribute)
{
  assert(attribute->index >= 0 && attribute->index < this->attributes.size());
  this->attributes[attribute->index] = attribute;
}

void
AttributeElement::set(IndexBuffer * indices)
{
  this->indices = indices;
}


const vector<vec3> & 
AttributeElement::getVertices()
{
  assert(this->attributes.size() > 0);
  return this->attributes[0]->values;
}
const vector<ivec3> & 
AttributeElement::getIndices()
{
  return this->indices->values;
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

void
MatrixElement::updateGL(State * state)
{
  GLuint loc = glGetUniformLocation(state->program->getProgramId(), this->name.c_str());
  glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(this->matrix));
}