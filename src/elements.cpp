#include <elements.h>
#include <state.h>
#include <nodes.h>
#include <opengl.h>
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
  GLuint loc = glGetUniformLocation(state->program->id, this->name.c_str());
  glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(this->matrix));
}

AttributeElement::AttributeElement()
  : indexcount(0), 
    attribcount(0),
    instancecount(0),
    arraybuffer(nullptr)
{
}

AttributeElement::~AttributeElement()
{

}

void
AttributeElement::push(Bindable * bindable)
{
  this->statevec.push_back(bindable);
}

void
AttributeElement::push(GLBufferObject * buffer)
{
  if (buffer->target == GL_ELEMENT_ARRAY_BUFFER) {
    this->indexcount = buffer->count;
  } 
  if (buffer->target == GL_ARRAY_BUFFER) {
    this->arraybuffer = buffer;
  }
  this->statevec.push_back(buffer);
}

void
AttributeElement::push(GLVertexAttribute * attrib)
{
  assert(this->arraybuffer);
  if (attrib->divisor == 1) {
    this->instancecount = this->arraybuffer->count;
  } else {
    this->attribcount = this->arraybuffer->count;
  }
  this->statevec.push_back(attrib);
}

void
AttributeElement::bind()
{
  for (unsigned int i = 0; i < this->statevec.size(); i++) {
    this->statevec[i]->bind();
  }
}

void
AttributeElement::unbind()
{
  for (unsigned int i = 0; i < this->statevec.size(); i++) {
    this->statevec[i]->unbind();
  }
}
