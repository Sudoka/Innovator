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

VertexElement::VertexElement()
  : indexcount(0),
    vertexcount(0),
    instancecount(0), 
    arraybuffer(nullptr),
    elementbuffer(nullptr)
 {}
VertexElement::~VertexElement() {}

void
VertexElement::set(ArrayBuffer * buffer)
{
  this->arraybuffer = buffer;
}

void
VertexElement::set(ElementBuffer * buffer)
{
  this->indexcount = buffer->values.vec.size();
  this->elementbuffer = buffer;
}

void 
VertexElement::set(VertexAttribute * attrib)
{
  ArrayBuffer * buffer = attrib->buffer.value.get();
  if (!buffer) {
    buffer = this->arraybuffer;
  }
  assert(buffer);
  this->statevec.push_back(buffer->buffer.get());

  if (attrib->index.value == 0) {
    this->vertexcount = this->arraybuffer->values.vec.size();
  }
  if (attrib->divisor.value == 1) {
    this->instancecount = 8;
  }
  this->attributes.push_back(attrib);
  this->statevec.push_back(attrib->attribute.get());
}

VertexAttribute * 
VertexElement::get(unsigned int index)
{
  for (size_t i = 0; i < this->attributes.size(); i++) {
    if (this->attributes[i]->index.value == index) {
      return this->attributes[i];
    }
  }
  return nullptr;
}

unsigned int 
VertexElement::getIndexCount() const
{
  return this->indexcount;
}

unsigned int
VertexElement::getVertexCount() const
{
  return this->vertexcount;
}

unsigned int
VertexElement::getInstanceCount() const
{
  return this->instancecount;
}

void
VertexElement::bind()
{
  if (this->elementbuffer) {
    this->elementbuffer->buffer->bind();
  }

  for (unsigned int i = 0; i < this->attributes.size(); i++) {
    VertexAttribute * attrib = this->attributes[i];
    GLBufferObject * glbuffer = attrib->buffer.value->buffer.get();
    GLVertexAttribute * glattrib = attrib->attribute.get();
    glbuffer->bind();
    glattrib->bind();
  }
}

void
VertexElement::unbind()
{
  if (this->elementbuffer) {
    this->elementbuffer->buffer->unbind();
  }

  for (unsigned int i = 0; i < this->attributes.size(); i++) {
    VertexAttribute * attrib = this->attributes[i];
    GLBufferObject * glbuffer = attrib->buffer.value->buffer.get();
    GLVertexAttribute * glattrib = attrib->attribute.get();
    glbuffer->unbind();
    glattrib->unbind();
  }
}
