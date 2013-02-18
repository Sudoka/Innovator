#include <nodes.h>

#include <actions.h>
#include <opengl.h>
#include <state.h>
#include <box3.h>
#include <vector>
#include <math.h>
#include <string>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace glm;
using namespace std;

// *************************************************************************************************

class Camera::CameraP {
public:
  vec3 up, pos, fpt;
  mat4 viewmatrix;
  mat4 projmatrix;
  mat4 orientation;
  mat4 translation;
};

Camera::Camera()
  : self(new CameraP)
{
  self->up  = vec3(0, 1, 0);
  self->pos = vec3(0, 0, 1);
  self->fpt = vec3(0, 0, 0);
}

Camera::~Camera()
{
}

void 
Camera::traverse(RenderAction * action)
{
  self->viewmatrix = glm::transpose(self->orientation) * self->translation;
  action->state->viewmatrixelem.matrix = self->viewmatrix;
  action->state->projmatrixelem.matrix = self->projmatrix;
}

vec3
Camera::getFocalDir() const
{
  return self->pos - self->fpt;
}

void
Camera::lookAt(const vec3 & focalpoint)
{
  self->fpt = focalpoint;

  vec3 z = glm::normalize(this->getFocalDir());
  vec3 x = glm::normalize(glm::cross(self->up, z));
  vec3 y = glm::normalize(glm::cross(z, x));

  self->orientation[0] = vec4(x, 0.0);
  self->orientation[1] = vec4(y, 0.0);
  self->orientation[2] = vec4(z, 0.0);
}

void
Camera::viewAll(Node::ptr root)
{
  BoundingBoxAction action;
  action.apply(root);
  box3 box = action.getBoundingBox();
  float focaldist = box.size();
  vec3 focalpoint = box.center();
  vec3 focaldir = glm::normalize(this->getFocalDir()) * focaldist;

  this->moveTo(focalpoint + focaldir);
  this->lookAt(focalpoint);
}

void
Camera::moveTo(const vec3 & position)
{
  self->pos = position;
  self->translation = glm::translate(mat4(1.0), -position);
}

void 
Camera::zoom(float dy)
{
  vec3 direction = -this->getFocalDir();
  float oldfocaldist = length(direction);
  float newfocaldist = oldfocaldist * exp(dy);
  vec3 dpos = normalize(direction) * (newfocaldist - oldfocaldist);
  this->moveTo(self->pos + dpos);
}

void
Camera::pan(const vec2 & dx)
{
  vec3 x = vec3(self->orientation * vec4(1, 0, 0, 0));
  vec3 y = vec3(self->orientation * vec4(0, 1, 0, 0));
  vec3 dpos = x * dx.x + y * dx.y;
  this->moveTo(self->pos + dpos);
  self->fpt += dpos;
}

void
Camera::orbit(const vec2 & dx)
{
  self->orientation = glm::rotate(self->orientation, dx.y, vec3(1, 0, 0));
  self->orientation = glm::rotate(self->orientation, dx.x, vec3(0, 1, 0));
  float z = glm::length(this->getFocalDir());
  vec3 newdir = vec3(self->orientation * vec4(0, 0, z, 0));
  this->moveTo(self->fpt + newdir);
}

void 
Camera::perspective(float fovy, float aspect, float near, float far)
{
  self->projmatrix = glm::perspective(fovy, aspect, near, far);
}

// *************************************************************************************************

Viewport::Viewport()
  : origin(0), size(100)
{
}

Viewport::~Viewport()
{
}

void
Viewport::traverse(RenderAction * action)
{
  action->state->viewportelem.size = this->size;
  action->state->viewportelem.origin = this->origin;
}

// *************************************************************************************************

class Group::GroupP {
public:
  vector<Node::ptr> children;
};

Group::Group() : self(new GroupP) {}
Group::~Group() {}

// *************************************************************************************************

void
Group::traverse(RenderAction * action)
{
  for (size_t i = 0; i < self->children.size(); i++) {
    self->children[i]->traverse(action);
  }
}

void
Group::traverse(BoundingBoxAction * action)
{
  for (size_t i = 0; i < self->children.size(); i++) {
    self->children[i]->traverse(action);
  }
}

void 
Group::addChild(Node::ptr child)
{
  self->children.push_back(child);
}

// *************************************************************************************************

Separator::Separator() 
  : boundingBoxCaching(0)
{}

Separator::~Separator() {}

void
Separator::traverse(RenderAction * action)
{
  action->state->push();
  Group::traverse(action);
  action->state->pop();
}

void
Separator::traverse(BoundingBoxAction * action)
{
  action->state->push();
  Group::traverse(action);
  action->state->pop();
}

// *************************************************************************************************

LUA_NODE_SOURCE(Transform);

Transform::Transform()
{
  LUA_NODE_ADD_FIELD(&this->translation, "translation");
  LUA_NODE_ADD_FIELD(&this->scaleFactor, "scaleFactor");
}

Transform::~Transform() {}

void
Transform::doAction(Action * action)
{
  mat4 & matrix = action->state->modelmatrixelem.matrix;
  matrix = glm::scale(matrix, this->scaleFactor.value);
  matrix = glm::translate(matrix, this->translation.value);
}

void
Transform::traverse(RenderAction * action)
{
  this->doAction(action);
}

void
Transform::traverse(BoundingBoxAction * action)
{
  this->doAction(action);
}

// *************************************************************************************************

class IndexBuffer::IndexBufferP {
public:
  IndexBufferP(IndexBuffer * self) 
    : buffer(new GLBufferObject(GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW, sizeof(ivec3) * self->values.size(), self->values.data()))  {}
  ~IndexBufferP() {}

  unique_ptr<GLBufferObject> buffer;
};

IndexBuffer::IndexBuffer()
  : self(nullptr) {}
IndexBuffer::~IndexBuffer() {}

void
IndexBuffer::traverse(RenderAction * action)
{
  if (self.get() == nullptr) {
    self.reset(new IndexBufferP(this));
  }
  action->state->attribelem.set(this);
}

void
IndexBuffer::bind()
{
  self->buffer->bind();
}

void
IndexBuffer::unbind()
{
  self->buffer->unbind();
}

// *************************************************************************************************

LUA_NODE_SOURCE(VertexAttribute);

class VertexAttribute::VertexAttributeP {
public:
  VertexAttributeP(VertexAttribute * self) 
    : buffer(new GLBufferObject(GL_ARRAY_BUFFER, GL_STATIC_DRAW, 
                                sizeof(vec3) * self->values.vec.size(), 
                                self->values.vec.data())) {}
  ~VertexAttributeP() {}
  unique_ptr<GLBufferObject> buffer;
};

VertexAttribute::VertexAttribute()
  : self(nullptr)
{
  // TODO: default value for fields when registering
  this->index.value = 0;
  this->divisor.value = 0;
  LUA_NODE_ADD_FIELD(&this->values, "values");
  LUA_NODE_ADD_FIELD(&this->index, "index");
  LUA_NODE_ADD_FIELD(&this->divisor, "divisor");
}

VertexAttribute::~VertexAttribute() {}

void
VertexAttribute::doAction(Action * action)
{
  if (self.get() == nullptr) {
    self.reset(new VertexAttributeP(this));
  }
  action->state->attribelem.set(this);
}

void
VertexAttribute::traverse(RenderAction * action)
{
  this->doAction(action);
}

void
VertexAttribute::traverse(BoundingBoxAction * action)
{
  this->doAction(action);
}

void
VertexAttribute::bind()
{
  self->buffer->bind();
  glEnableVertexAttribArray(this->index.value);
  glVertexAttribPointer(this->index.value, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glVertexAttribDivisor(this->index.value, this->divisor.value);
}

void
VertexAttribute::unbind()
{
  self->buffer->unbind();
  glDisableVertexAttribArray(this->index.value);
}

// *************************************************************************************************

static GLenum glMode(Draw::Mode mode) 
{
  switch (mode) {
  case Draw::POINTS: return GL_POINTS;
  case Draw::TRIANGLES: return GL_TRIANGLES;
  default: 
    throw std::runtime_error("invalid draw mode: " + std::to_string((__int64)mode));
  }
};

Draw::Draw() : mode(POINTS) {}
Draw::~Draw() {}

void
Draw::traverse(RenderAction * action)
{
  action->state->flush(this);
}

void
Draw::traverse(BoundingBoxAction * action)
{
  VertexAttribute * attrib = action->state->attribelem.get(0);
  if (attrib) {
    box3 bbox;
    for (size_t i = 0; i < attrib->values.vec.size(); i++) {
      bbox.extendBy(attrib->values.vec[i]);
    }
    bbox.transform(action->state->modelmatrixelem.matrix);
    action->extendBy(bbox);
  }
}

// *************************************************************************************************
void
DrawArrays::execute(State * state)
{
  VertexAttribute * attrib = state->attribelem.get(0);
  assert(attrib);
  glDrawArrays(glMode(this->mode), 0, attrib->values.vec.size());
}

// *************************************************************************************************
void
DrawElements::execute(State * state)
{
  IndexBuffer * indices = state->attribelem.getIndexBuffer();
  assert(indices);
  GLuint num = indices->values.size() * sizeof(ivec3);
  glDrawElements(glMode(this->mode), num, GL_UNSIGNED_INT, 0);
}

// *************************************************************************************************
void
DrawElementsInstanced::execute(State * state)
{
  IndexBuffer * indices = state->attribelem.getIndexBuffer();
  unsigned int primcount = state->attribelem.getInstanceCount();
  assert(indices && primcount > 0);
  GLuint count = indices->values.size() * sizeof(ivec3);
  glDrawElementsInstanced(glMode(this->mode), count, GL_UNSIGNED_INT, 0, primcount);
}

// *************************************************************************************************
void
DrawArraysInstanced::execute(State * state)
{
  VertexAttribute * attrib = state->attribelem.get(0);
  unsigned int primcount = state->attribelem.getInstanceCount();
  assert(attrib && primcount > 0);
  GLuint count = attrib->values.vec.size();
  glDrawArraysInstanced(glMode(this->mode), 0, count, primcount);
}
