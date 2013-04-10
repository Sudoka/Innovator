#include <elements.h>
#include <state.h>
#include <nodes.h>
#include <opengl.h>
#include <iostream>
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
Uniform3fElement::updateGL(State * state)
{
  if (!this->name.empty()) {
    GLint loc = glGetUniformLocation(state->program->id, this->name.c_str());
    if (loc != -1) {
      glUniform3fv(loc, 1, glm::value_ptr(this->value));
    } else {
      //cout << "Uniform3fElement::updateGL(State * state): uniform " << name << " location not found in program";
    }
  }
}

void
MatrixElement::updateGL(State * state)
{
  if (this->name.empty()) {
    throw std::runtime_error("MatrixElement::updateGL(State * state): name is empty");
  }
  GLint loc = glGetUniformLocation(state->program->id, this->name.c_str());
  if (loc != -1) {
    glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(this->matrix));
  } else {
    cout << "MatrixElement::updateGL(State * state): uniform " << name << " location not found in program";
  }
}

VertexElement::VertexElement()
  : arraybuffer(nullptr),
    elementbuffer(nullptr),
    instancebuffer(nullptr)
{}

VertexElement::~VertexElement() 
{}

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

Buffer * 
VertexElement::getArrayBuffer() const
{
  return this->arraybuffer;
}

Buffer * 
VertexElement::getVertexBuffer() const
{
  return this->vertexbuffer;
}

Buffer * 
VertexElement::getElementBuffer() const
{
  return this->elementbuffer;
}

Buffer * 
VertexElement::getInstanceBuffer() const
{
  return this->instancebuffer;
}

GLVertexArrayObject * 
VertexElement::createVAO()
{
  GLVertexArrayObject * vao = new GLVertexArrayObject;
  {
    BindScope vao_scope(vao);
    this->bind();
  }
  this->unbind();
  return vao;
}

void
VertexElement::bind()
{
  for (unsigned int i = 0; i < this->statevec.size(); i++) {
    this->statevec[i]->bind();
  }
}

void
VertexElement::unbind()
{
  for (unsigned int i = 0; i < this->statevec.size(); i++) {
    this->statevec[i]->unbind();
  }
}
