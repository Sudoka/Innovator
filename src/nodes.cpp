#include <nodes.h>

#include <actions.h>
#include <opengl.h>
#include <state.h>
#include <box3.h>
#include <vector>
#include <math.h>
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
Camera::viewAll(std::shared_ptr<Node> root)
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

class Group::GroupP {
public:
  vector<shared_ptr<Node>> children;
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
Group::addChild(std::shared_ptr<Node> child)
{
  self->children.push_back(child);
}

// *************************************************************************************************

Separator::Separator() {}
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

Transform::Transform()
  : translation(0, 0, 0),
    scaleFactor(1, 1, 1) 
{}

Transform::~Transform() {}

void
Transform::doAction(Action * action)
{
  mat4 & matrix = action->state->modelmatrixelem.matrix;
  matrix = glm::scale(matrix, this->scaleFactor);
  matrix = glm::translate(matrix, this->translation);
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
  : self(nullptr)
{

}

IndexBuffer::~IndexBuffer()
{

}

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

class VertexAttribute::VertexAttributeP {
public:
  VertexAttributeP(VertexAttribute * self) 
    : buffer(new GLBufferObject(GL_ARRAY_BUFFER, GL_STATIC_DRAW, sizeof(vec3) * self->values.size(), self->values.data())) {}
  ~VertexAttributeP() {}
  unique_ptr<GLBufferObject> buffer;
};

VertexAttribute::VertexAttribute()
  : index(0),
    divisor(0),
    self(nullptr)
{
}

VertexAttribute::~VertexAttribute()
{
}

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
  glEnableVertexAttribArray(this->index);
  glVertexAttribPointer(this->index, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glVertexAttribDivisor(this->index, this->divisor);
}

void
VertexAttribute::unbind()
{
  self->buffer->unbind();
  glDisableVertexAttribArray(this->index);
}

// *************************************************************************************************

Shape::Shape() {}
Shape::~Shape() {}

void
Shape::traverse(RenderAction * action)
{
  action->state->flush(this);
}

void
Shape::draw(State * state)
{
  const vector<ivec3> & indices = state->attribelem.getIndices();
  unsigned int num = indices.size() * sizeof(ivec3);
  glDrawElements(GL_TRIANGLES, num, GL_UNSIGNED_INT, 0);
}

void
Shape::traverse(BoundingBoxAction * action)
{
  const vector<vec3> & vertices = action->state->attribelem.getVertices();
  box3 bbox;
  for (size_t i = 0; i < vertices.size(); i++) {
    bbox.extendBy(vertices[i]);
  }
  bbox.transform(action->state->modelmatrixelem.matrix);
  action->extendBy(bbox);
}