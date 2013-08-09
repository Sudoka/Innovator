#include <nodes.h>
#include <actions.h>
#include <opengl.h>
#include <state.h>
#include <box3.h>
#include <map>
#include <functional>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace glm;
using namespace std;

// *************************************************************************************************

class Camera::CameraP {
public:
  mat3 orientation;
  UniformMatrix4f projmatrix;
  UniformMatrix4f viewmatrix;
};

Camera::Camera()
  : self(new CameraP)
{
  this->registerField(this->position, "position", vec3(0, 0, 1));
  this->registerField(this->focalDistance, "focalDistance", 1.0f);

  self->viewmatrix.name.value = "ViewMatrix";
  self->projmatrix.name.value = "ProjectionMatrix";
  self->orientation = mat3(1.0);
}

Camera::~Camera()
{
}

void 
Camera::traverse(RenderAction * action)
{
  action->state->camera = this;
}

void
Camera::flush(State * state)
{
  self->viewmatrix.value.value = glm::transpose(mat4(self->orientation));
  self->viewmatrix.value.value = glm::translate(self->viewmatrix.value.value, -this->position.value);
  self->viewmatrix.flush(state);
  self->projmatrix.flush(state);
}

void
Camera::lookAt(const vec3 & focalpoint)
{
  self->orientation[2] = glm::normalize(this->position.value - focalpoint);
  self->orientation[0] = glm::normalize(glm::cross(self->orientation[1], self->orientation[2]));
  self->orientation[1] = glm::normalize(glm::cross(self->orientation[2], self->orientation[0]));
}

void
Camera::viewAll(Separator * root)
{
  box3 box = BoundingBoxAction::getBoundingBox(root);
  vec3 focalpoint = box.center();
  this->focalDistance.value = box.size();

  this->position.value = focalpoint + self->orientation[2] * this->focalDistance.value;
  this->lookAt(focalpoint);
}

void 
Camera::zoom(float dy)
{
  vec3 focalpoint = this->position.value - self->orientation[2] * this->focalDistance.value;
  this->position.value += self->orientation[2] * dy;
  this->focalDistance.value = glm::length(this->position.value - focalpoint);
}

void
Camera::pan(const vec2 & dx)
{
  this->position.value += self->orientation[0] * dx.x + self->orientation[1] * dx.y;
}

void
Camera::orbit(const vec2 & dx)
{
  vec3 focalpoint = this->position.value - self->orientation[2] * this->focalDistance.value;

  self->orientation = mat3(glm::rotate(mat4(self->orientation), dx.y, vec3(1, 0, 0)));
  self->orientation = mat3(glm::rotate(mat4(self->orientation), dx.x, vec3(0, 1, 0)));

  this->position.value = focalpoint + self->orientation[2] * this->focalDistance.value;
  this->lookAt(focalpoint); // orthogonalizes orientation matrix
}

void 
Camera::perspective(float fovy, float aspect, float near, float far)
{
  self->projmatrix.value.value = glm::perspective(fovy, aspect, near, far);
}

// *************************************************************************************************

Viewport::Viewport()
{
  this->registerField(this->size, "size", ivec2(-1));
  this->registerField(this->origin, "origin", ivec2(-1));
}

Viewport::~Viewport()
{
}

void
Viewport::traverse(RenderAction * action)
{
  action->state->viewport = this;
}

void
Viewport::flush(State * state)
{
  glViewport(this->origin.value.x, this->origin.value.y, this->size.value.x, this->size.value.y);
}

// *************************************************************************************************


Group::Group() 
{
  this->registerField(this->children);
}
Group::~Group() {}

// *************************************************************************************************

void
Group::traverse(RenderAction * action)
{
  for each (const shared_ptr<Node> & node in this->children.values) {
    node->traverse(action);
  }
}

void
Group::traverse(BoundingBoxAction * action)
{
  for each (const shared_ptr<Node> & node in this->children.values) {
    node->traverse(action);
  }
}

// *************************************************************************************************

void
Separator::traverse(RenderAction * action)
{
  StateScope scope(action->state.get());
  Group::traverse(action);
}

void
Separator::traverse(BoundingBoxAction * action)
{
  StateScope scope(action->state.get());
  Group::traverse(action);
}

// *************************************************************************************************

Uniform::Uniform()
{
  this->registerField(this->name, "name");
}

Uniform::~Uniform()
{
}

// *************************************************************************************************

Uniform3f::Uniform3f()
{
  this->registerField(this->value, "value", vec3(0));
}

Uniform3f::~Uniform3f()
{
}

void
Uniform3f::flush(State * state)
{
  GLint location = state->program->getUniformLocation(this->name.value);
  glUniform3fv(location, 1, glm::value_ptr(this->value.value));
}

// *************************************************************************************************


UniformMatrix4f::UniformMatrix4f()
{
  this->registerField(this->value, "value", mat4(1.0));
}

UniformMatrix4f::~UniformMatrix4f()
{
}

void
UniformMatrix4f::flush(State * state)
{
  GLint location = state->program->getUniformLocation(this->name.value);
  glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(this->value.value));
}

// *************************************************************************************************


ShaderObject::ShaderObject()
{
  this->registerField(this->type, "type", GL_INVALID_VALUE);
  this->registerEnum(this->type, "VERTEX_SHADER", GL_VERTEX_SHADER);
  this->registerEnum(this->type, "GEOMETRY_SHADER", GL_GEOMETRY_SHADER);
  this->registerEnum(this->type, "FRAGMENT_SHADER", GL_FRAGMENT_SHADER);
  this->registerField(this->source, "source");
}

ShaderObject::~ShaderObject()
{
}

// *************************************************************************************************

class Program::ProgramP {
public:
  ProgramP(Program * self) {
    this->glprogram.reset(new GLProgram);
    for each (const shared_ptr<ShaderObject> & shader in self->shaders.values) {
      this->glprogram->attach(shader->source.value.c_str(), shader->type.value);
    }
    if (!self->feedbackVarying.value.empty()) {
      const char * varyings[] = { self->feedbackVarying.value.c_str() };
      glTransformFeedbackVaryings(this->glprogram->id, 1, varyings, GL_SEPARATE_ATTRIBS);
    }
    this->glprogram->link();
  }
  unique_ptr<GLProgram> glprogram;
  map<string, GLint> uniformLocations;
};

Program::Program() 
  : self(nullptr)
{
  this->registerField(this->shaders);
  this->registerField(this->uniforms, "uniforms");
  this->registerField(this->feedbackVarying, "feedbackVarying", "");
}

Program::~Program() 
{
}

void
Program::traverse(RenderAction * action)
{
  action->state->program = this;
}

void
Program::flush(State * state)
{
  if (self.get() == nullptr) {
    self.reset(new ProgramP(this));
  }
  self->glprogram->bind();
  for each (const shared_ptr<Uniform> & uniform in this->uniforms.values) {
    uniform->flush(state);
  }
}

GLint
Program::getUniformLocation(const std::string & name)
{
  if (self->uniformLocations.find(name) == self->uniformLocations.end()) {
    self->uniformLocations[name] = glGetUniformLocation(self->glprogram->id, name.c_str());
  }
  return self->uniformLocations[name];
}

// *************************************************************************************************

Transform::Transform()
{
  this->registerField(this->translation, "translation", vec3(0));
  this->registerField(this->scaleFactor, "scaleFactor", vec3(1));
}

Transform::~Transform() {}

void
Transform::doAction(Action * action)
{
  mat4 matrix(1.0);
  matrix = glm::scale(matrix, this->scaleFactor.value);
  matrix = glm::translate(matrix, this->translation.value);
  action->state->transformelem.mult(matrix);
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
  this->registerField(this->type, "type", GL_FLOAT);
  this->registerEnum(this->type, "FLOAT", GL_FLOAT);
  this->registerEnum(this->type, "UNSIGNED_INT", GL_UNSIGNED_INT);

  this->registerField(this->usage, "usage", GL_STATIC_DRAW);
  this->registerEnum(this->usage, "STATIC_DRAW", GL_STATIC_DRAW);
  this->registerEnum(this->usage, "DYNAMIC_DRAW", GL_DYNAMIC_DRAW);

  this->registerField(this->target, "target", GL_ARRAY_BUFFER);
  this->registerEnum(this->target, "ARRAY", GL_ARRAY_BUFFER);
  this->registerEnum(this->target, "ELEMENT_ARRAY", GL_ELEMENT_ARRAY_BUFFER);

  this->registerField(this->count, "count", 0);
  this->registerField(this->values, "values");
}

Buffer::~Buffer()
{
}

void
Buffer::traverse(RenderAction * action)
{
  if (!this->buffer.get()) {
    this->buffer.reset(GLBufferObject::create(this->target.value, 
                                              this->usage.value, 
                                              this->type.value, 
                                              this->count.value,
                                              this->values.vec));
  }
  action->state->vertexelem.set(this->buffer.get());
}

// *************************************************************************************************

VertexAttribute::VertexAttribute()
  : glattrib(nullptr)
{
  this->registerField(this->type, "type", GL_FLOAT);
  this->registerField(this->size, "size", 3);
  this->registerField(this->index, "location", 0);
  this->registerField(this->divisor, "divisor", 0);
}

VertexAttribute::~VertexAttribute() {}

void
VertexAttribute::traverse(RenderAction * action)
{
  if (!this->glattrib.get()) {
    this->glattrib.reset(new GLVertexAttribute(this->index.value, 
                                               this->size.value,
                                               this->type.value,
                                               this->divisor.value));
  }
  action->state->vertexelem.set(this->glattrib.get());
}

// *************************************************************************************************

TextureUnit::TextureUnit()
  : gltexunit(nullptr)
{
  this->registerField(this->unit, "unit", 0);
}

TextureUnit::~TextureUnit()
{
}

void
TextureUnit::traverse(RenderAction * action)
{
  if (this->gltexunit.get() == nullptr) {
    this->gltexunit.reset(new GLTextureUnit(this->unit.value));
  }
  action->state->textureelem.set(this);
}

// *************************************************************************************************

Texture::Texture()
  : gltexture(nullptr)
{
  this->registerField(this->fileName, "fileName");
  this->registerField(this->target, "target", GL_TEXTURE_2D);

  this->registerField(this->width, "width", 0);
  this->registerField(this->height, "height", 0);

  this->registerField(this->level, "level", 0);
  this->registerField(this->border, "border", 0);

  this->registerField(this->format, "format", GL_RGBA);
  this->registerEnum(this->format, "RGBA", GL_RGBA);
  this->registerEnum(this->format, "DEPTH_COMPONENT", GL_DEPTH_COMPONENT);

  this->registerField(this->internalFormat, "internalFormat", GL_RGBA8);
  this->registerEnum(this->internalFormat, "RGBA8", GL_RGBA8);
  this->registerEnum(this->internalFormat, "DEPTH_COMPONENT32", GL_DEPTH_COMPONENT32);
}

Texture::~Texture()
{
}

void
Texture::traverse(RenderAction * action)
{
  if (this->gltexture.get() == nullptr) {
    this->gltexture.reset(new GLTextureObject(this->target.value,
                                              this->level.value, 
                                              this->internalFormat.value,
                                              this->width.value,
                                              this->height.value,
                                              this->border.value,
                                              this->format.value,
                                              this->type.value,
                                              nullptr));
  }
  action->state->textureelem.set(this);
}

// *************************************************************************************************

TextureSampler::TextureSampler()
  : glsampler(nullptr)
{
  this->registerField(this->wrapS, "wrapS", GL_CLAMP_TO_EDGE);
  this->registerField(this->wrapT, "wrapT", GL_CLAMP_TO_EDGE);
  this->registerField(this->wrapR, "wrapR", GL_CLAMP_TO_EDGE);
  
  this->registerField(this->minFilter, "minFilter", GL_NEAREST);
  this->registerField(this->magFilter, "magFilter", GL_NEAREST);
}

TextureSampler::~TextureSampler()
{
}

void
TextureSampler::traverse(RenderAction * action)
{
  if (this->glsampler.get() == nullptr) {
    this->glsampler.reset(new GLTextureSampler(this->wrapS.value,
                                               this->wrapT.value,
                                               this->wrapR.value,
                                               this->minFilter.value,
                                               this->magFilter.value));
  }
  action->state->textureelem.set(this);
}

// *************************************************************************************************

BoundingBox::BoundingBox()
{
  this->registerField(this->min, "min", vec3(-1));
  this->registerField(this->max, "max", vec3( 1));
}

void
BoundingBox::traverse(BoundingBoxAction * action) 
{
  box3 bbox(this->min.value, this->max.value);
  bbox.transform(action->state->transformelem.matrix);
  action->extendBy(bbox);
}

// *************************************************************************************************

DrawCall::DrawCall()
  : vao(nullptr)
{
  this->registerField(this->mode, "mode", GL_POINTS);
  this->registerEnum(this->mode, "POINTS", GL_POINTS);
  this->registerEnum(this->mode, "TRIANGLES", GL_TRIANGLES);
  this->registerEnum(this->mode, "TRIANGLE_STRIP", GL_TRIANGLE_STRIP);
}

DrawCall::~DrawCall()
{
}

void
DrawCall::traverse(RenderAction * action)
{
  if (this->vao.get() == nullptr) {
    this->vao.reset(action->state->vertexelem.createVAO());
  }
  BindScope vao(this->vao.get());
  action->state->flush(this);
}

// *************************************************************************************************

DrawElements::DrawElements()
{
  this->registerField(this->count, "count", 0);
  this->registerField(this->indices, "indices");
  this->registerField(this->type, "type", GL_UNSIGNED_INT);
  this->registerEnum(this->type, "UNSIGNED_INT", GL_UNSIGNED_INT);
  this->registerEnum(this->type, "UNSIGNED_BYTE", GL_UNSIGNED_BYTE);
  this->registerEnum(this->type, "UNSIGNED_SHORT", GL_UNSIGNED_SHORT);
}

DrawElements::~DrawElements()
{
}

void
DrawElements::execute(State * state)
{
  glDrawElements(this->mode.value, this->count.value, this->type.value, nullptr);
}

// *************************************************************************************************

DrawArrays::DrawArrays()
{
  this->registerField(this->first, "first", 0);
  this->registerField(this->count, "count", 0);
}

DrawArrays::~DrawArrays()
{
}

void
DrawArrays::execute(State * state)
{
  glDrawArrays(this->mode.value, this->first.value, this->count.value);
}

// *************************************************************************************************
