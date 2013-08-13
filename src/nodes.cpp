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
  mat4 projmatrix;
};

Camera::Camera()
  : self(new CameraP)
{
  this->registerField(this->position, "position", vec3(0, 0, 1));
  this->registerField(this->focalDistance, "focalDistance", 1.0f);
  this->registerField(this->orientation, "orientation", mat3(1.0));
}

Camera::~Camera()
{
}

void 
Camera::traverse(RenderAction * action)
{
  mat4 viewmatrix = glm::transpose(mat4(this->orientation.value));
  viewmatrix = glm::translate(viewmatrix, -this->position.value);

  action->state->viewmatelem.matrix = viewmatrix;
  action->state->projmatelem.matrix = self->projmatrix;
}

void
Camera::lookAt(const vec3 & focalpoint)
{
  this->orientation.value[2] = glm::normalize(this->position.value - focalpoint);
  this->orientation.value[0] = glm::normalize(glm::cross(this->orientation.value[1], this->orientation.value[2]));
  this->orientation.value[1] = glm::normalize(glm::cross(this->orientation.value[2], this->orientation.value[0]));
}

void
Camera::viewAll(Separator * root)
{
  box3 box = BoundingBoxAction::getBoundingBox(root);
  vec3 focalpoint = box.center();
  this->focalDistance.value = box.size();

  this->position.value = focalpoint + this->orientation.value[2] * this->focalDistance.value;
  this->lookAt(focalpoint);
}

void 
Camera::zoom(float dy)
{
  vec3 focalpoint = this->position.value - this->orientation.value[2] * this->focalDistance.value;
  this->position.value += this->orientation.value[2] * dy;
  this->focalDistance.value = glm::length(this->position.value - focalpoint);
}

void
Camera::pan(const vec2 & dx)
{
  this->position.value += this->orientation.value[0] * dx.x + this->orientation.value[1] * dx.y;
}

void
Camera::orbit(const vec2 & dx)
{
  vec3 focalpoint = this->position.value - this->orientation.value[2] * this->focalDistance.value;

  mat4 rot = mat4(this->orientation.value);
  rot = glm::rotate(rot, dx.y, vec3(1, 0, 0));
  rot = glm::rotate(rot, dx.x, vec3(0, 1, 0));
  vec3 look = mat3(rot) * vec3(0, 0, this->focalDistance.value);

  this->position.value = focalpoint + look;
  this->lookAt(focalpoint);
}

void 
Camera::perspective(float fovy, float aspect, float near, float far)
{
  self->projmatrix = glm::perspective(fovy, aspect, near, far);
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

class Separator::SeparatorP {
public:
  SeparatorP() : rendercache(nullptr) {}
  std::function<void()> rendercache;
};

Separator::Separator()
  : self(new SeparatorP)
{
  this->registerField(this->renderCaching, "renderCaching", Separator::ON);
  this->registerEnum(this->renderCaching, "ON", Separator::ON);
  this->registerEnum(this->renderCaching, "OFF", Separator::OFF);
}

Separator::~Separator()
{

}

void
Separator::traverse(RenderAction * action)
{
  StateScope scope(action->state.get());

  if (this->renderCaching.value == Separator::ON) {
    if (self->rendercache == nullptr) {
      action->state->cacheelem.push();
      Group::traverse(action);
      self->rendercache = action->state->cacheelem.pop();
    }
    if (self->rendercache != nullptr) {
      self->rendercache();
    }
  } else {
    Group::traverse(action);
  }
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

// *************************************************************************************************


UniformMatrix4f::UniformMatrix4f()
{
  this->registerField(this->value, "value", mat4(1.0));
}

UniformMatrix4f::~UniformMatrix4f()
{
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
}

Program::~Program() 
{
}

void
Program::traverse(RenderAction * action)
{
  if (self.get() == nullptr) {
    self.reset(new ProgramP(this));
  }
  action->state->programelem.program = self->glprogram->id;
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
  //action->state->textureelem.set(this);
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
  //action->state->textureelem.set(this);
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
  //action->state->textureelem.set(this);
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
  action->state->drawelem.drawcall = this;
  if (action->state->cacheelem.isCreatingCache) {
    action->state->cacheelem.append(action->state->capture());
  } else {
    action->state->capture()();
  }
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
DrawElements::execute()
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
DrawArrays::execute()
{
  glDrawArrays(this->mode.value, this->first.value, this->count.value);
}

// *************************************************************************************************