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
Camera::viewAll(Separator::ptr root)
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

LUA_NODE_SOURCE(Group);

void
Group::initClass()
{
  LUA_NODE_INIT_CLASS(Group, "Group");
}

Group::Group() 
{
  LUA_NODE_ADD_FIELD_1(this->children);
}
Group::~Group() {}

// *************************************************************************************************

void
Group::traverse(RenderAction * action)
{
  for (size_t i = 0; i < this->children.values.size(); i++) {
    this->children.values[i]->traverse(action);
  }
}

void
Group::traverse(BoundingBoxAction * action)
{
  for (size_t i = 0; i < this->children.values.size(); i++) {
    this->children.values[i]->traverse(action);
  }
}

// *************************************************************************************************

LUA_NODE_SOURCE(Separator);

void
Separator::initClass()
{
  LUA_NODE_INIT_CLASS(Separator, "Separator");
}

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

void
Transform::initClass()
{
  LUA_NODE_INIT_CLASS(Transform, "Transform");
}

Transform::Transform()
{
  LUA_NODE_ADD_FIELD_3(this->translation, "translation", vec3(0));
  LUA_NODE_ADD_FIELD_3(this->scaleFactor, "scaleFactor", vec3(1));
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

Buffer::Buffer()
  : buffer(nullptr)
{
  LUA_NODE_ADD_FIELD_3(this->target, "target", Buffer::ARRAY);
  LUA_NODE_ADD_FIELD_3(this->usage, "usage", Buffer::STATIC_DRAW);
  LUA_ENUM_DEFINE_VALUE(this->target, "ARRAY", Buffer::ARRAY);
  LUA_ENUM_DEFINE_VALUE(this->target, "ELEMENT_ARRAY", Buffer::ELEMENT_ARRAY);
  LUA_ENUM_DEFINE_VALUE(this->usage, "STATIC_DRAW", Buffer::STATIC_DRAW);
  LUA_ENUM_DEFINE_VALUE(this->usage, "DYNAMIC_DRAW", Buffer::DYNAMIC_DRAW);
}

// *************************************************************************************************

LUA_NODE_SOURCE(ArrayBuffer);

void
ArrayBuffer::initClass()
{
  LUA_NODE_INIT_CLASS(ArrayBuffer, "ArrayBuffer");
}

ArrayBuffer::ArrayBuffer()
{
  LUA_NODE_ADD_FIELD_2(this->values, "values");
}

void
ArrayBuffer::traverse(RenderAction * action)
{
  if (this->buffer.get() == nullptr) {
    this->buffer.reset(new GLBufferObject(GL_ARRAY_BUFFER, this->usage.value, this->values.vec));
  }
  action->state->vertexelem.set(this);
}

void
ArrayBuffer::traverse(BoundingBoxAction * action)
{
  if (this->buffer.get() == nullptr) {
    this->buffer.reset(new GLBufferObject(GL_ARRAY_BUFFER, this->usage.value, this->values.vec));
  }
  action->state->vertexelem.set(this);
}

// *************************************************************************************************

LUA_NODE_SOURCE(ElementBuffer);

void
ElementBuffer::initClass()
{
  LUA_NODE_INIT_CLASS(ElementBuffer, "ElementBuffer");
}

ElementBuffer::ElementBuffer()
{
  LUA_NODE_ADD_FIELD_2(this->values, "values");
}

void
ElementBuffer::traverse(RenderAction * action)
{
  if (this->buffer.get() == nullptr) {
    this->buffer.reset(new GLBufferObject(GL_ELEMENT_ARRAY_BUFFER, this->usage.value, this->values.vec));
  }
  action->state->vertexelem.set(this);
}

void
ElementBuffer::traverse(BoundingBoxAction * action)
{
  if (this->buffer.get() == nullptr) {
    this->buffer.reset(new GLBufferObject(GL_ELEMENT_ARRAY_BUFFER, this->usage.value, this->values.vec));
  }
  action->state->vertexelem.set(this);
}

// *************************************************************************************************

LUA_NODE_SOURCE(VertexAttribute);

void
VertexAttribute::initClass()
{
  LUA_NODE_INIT_CLASS(VertexAttribute, "VertexAttribute");
}


VertexAttribute::VertexAttribute()
  : attribute(nullptr)
{
  LUA_NODE_ADD_FIELD_3(this->index, "location", 0);
  LUA_NODE_ADD_FIELD_3(this->divisor, "divisor", 0);
  LUA_NODE_ADD_FIELD_3(this->buffer, "buffer", nullptr);
}

VertexAttribute::~VertexAttribute() {}

void
VertexAttribute::traverse(RenderAction * action)
{
  if (!this->attribute.get()) {
    this->attribute.reset(new GLVertexAttribute(this->index.value, this->divisor.value));
  }
  action->state->vertexelem.set(this);
}

void
VertexAttribute::traverse(BoundingBoxAction * action)
{
  if (!this->attribute.get()) {
    this->attribute.reset(new GLVertexAttribute(this->index.value, this->divisor.value));
  }
  action->state->vertexelem.set(this);
}

// *************************************************************************************************

LUA_NODE_SOURCE(Shape);

void
Shape::initClass()
{
  LUA_NODE_INIT_CLASS(Shape, "Shape"); 
}

Shape::Shape() {}

void
Shape::traverse(RenderAction * action)
{
  action->state->flush(this);
}

void
Shape::draw(State * state)
{
  unsigned int elemcount = state->vertexelem.getIndexCount();
  unsigned int vertexcount = state->vertexelem.getVertexCount();
  unsigned int instancecount = state->vertexelem.getInstanceCount();

  if (elemcount > 0) {
    if (instancecount > 0) {
      glDrawElementsInstanced(GL_TRIANGLES, elemcount, GL_UNSIGNED_INT, 0, instancecount);
    } else {
      glDrawElements(GL_TRIANGLES, elemcount, GL_UNSIGNED_INT, nullptr);
    }
  } else {
    if (instancecount > 0) {
      glDrawArraysInstanced(GL_TRIANGLES, 0, vertexcount, instancecount);
    } else {
      glDrawArrays(GL_TRIANGLES, 0, vertexcount);
    }
  }
}

void
Shape::traverse(BoundingBoxAction * action)
{
  assert(action->state->vertexelem.get(0));
  VertexAttribute * attrib = action->state->vertexelem.get(0);
  ArrayBuffer * buffer = attrib->buffer.value.get();

  box3 bbox;
  for (size_t i = 0; i < buffer->values.vec.size(); i += 3) {
    bbox.extendBy(vec3(buffer->values.vec[i + 0],
                       buffer->values.vec[i + 1],
                       buffer->values.vec[i + 2]));
  }
  bbox.transform(action->state->modelmatrixelem.matrix);
  action->extendBy(bbox);
}


// *************************************************************************************************

Draw::Draw() 
{
  LUA_NODE_ADD_FIELD_3(this->mode, "mode", Draw::POINTS);
  LUA_ENUM_DEFINE_VALUE(this->mode, "POINTS", Draw::POINTS);
  LUA_ENUM_DEFINE_VALUE(this->mode, "TRIANGLES", Draw::TRIANGLES);
}

Draw::~Draw() {}

void
Draw::traverse(RenderAction * action)
{
  //action->state->flush(this);
}
