#include <nodes.h>
#include <actions.h>
#include <opengl.h>
#include <state.h>
#include <box3.h>
#include <rendercache.h>
#include <functional>
#include <iostream>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace glm;
using namespace std;

// *************************************************************************************************

Camera::Camera()
{
  this->registerField(this->orientation, "orientation", mat3(1.0));
  this->registerField(this->aspectRatio, "aspectRatio", 4.0f / 3.0f);
  this->registerField(this->fieldOfView, "fieldOfView", 45.0f);
  this->registerField(this->nearPlane, "nearPlane", 0.1f);
  this->registerField(this->farPlane, "farPlane", 10000.0f);
  this->registerField(this->position, "position", vec3(0, 0, 1));
  this->registerField(this->focalDistance, "focalDistance", 1.0f);
}

Camera::~Camera()
{
}

void 
Camera::traverse(RenderAction * action)
{
  State * state = action->state.get();

  state->viewmatrix = glm::transpose(mat4(this->orientation.value));
  state->viewmatrix = glm::translate(state->viewmatrix, -this->position.value);

  state->projmatrix = glm::perspective(this->fieldOfView.value,
                                       this->aspectRatio.value,
                                       this->nearPlane.value,
                                       this->farPlane.value);
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
  unique_ptr<RenderCache> rendercache;
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
  State * state = action->state.get();

  if (this->renderCaching.value == Separator::ON) {
    if (state->rendercache == nullptr) {
      if (self->rendercache.get() == nullptr) {
        self->rendercache.reset(new RenderCache);
        state->rendercache = self->rendercache.get();
        StateScope scope(state);
        Group::traverse(action);
        state->rendercache = nullptr;
      }
      self->rendercache->flush(state);
      return;
    }
  }
  StateScope scope(state);
  Group::traverse(action);
}

void
Separator::traverse(BoundingBoxAction * action)
{
  StateScope scope(action->state.get());
  Group::traverse(action);
}

// *************************************************************************************************

Material::Material()
{
  this->registerField(this->ambient, "ambient", vec3(1, 1, 1));
  this->registerField(this->diffuse, "diffuse", vec3(0, 0, 0));
  this->registerField(this->specular, "specular", vec3(0.5f, 0.5f, 0.5f));
  this->registerField(this->shininess, "shininess", 0.1f);
  this->registerField(this->transparency, "transparency", 0.0f);
}

Material::~Material()
{

}

void
Material::traverse(RenderAction * action)
{
  action->state->material = this;
}


// *************************************************************************************************

ShaderObject::ShaderObject()
{
  this->registerField(this->type, "type", GL_INVALID_VALUE);
  this->registerEnum(this->type, "VERTEX_SHADER", GL_VERTEX_SHADER);
  this->registerEnum(this->type, "TESS_CONTROL_SHADER", GL_TESS_CONTROL_SHADER);
  this->registerEnum(this->type, "TESS_EVALUATION_SHADER", GL_TESS_EVALUATION_SHADER);
  this->registerEnum(this->type, "GEOMETRY_SHADER", GL_GEOMETRY_SHADER);
  this->registerEnum(this->type, "FRAGMENT_SHADER", GL_FRAGMENT_SHADER);
  this->registerField(this->source, "source");
}

ShaderObject::~ShaderObject()
{
}

// *************************************************************************************************

Program::Program() 
{
  this->registerField(this->shaders);
}

Program::~Program() 
{
}

void
Program::traverse(RenderAction * action)
{
  action->state->program = this;
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
  action->state->transform *= matrix;
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

BoundingBox::~BoundingBox()
{
}

void
BoundingBox::traverse(BoundingBoxAction * action) 
{
  box3 bbox(this->min.value, this->max.value);
  bbox.transform(action->state->transform);
  action->extendBy(bbox);
}

// *************************************************************************************************

Shape::Shape()
{
  this->registerField(this->indices, "indices");
  this->registerField(this->vertices, "vertices");
}

Shape::~Shape()
{

}

void
Shape::traverse(RenderAction * action)
{
  if (action->state->rendercache != nullptr) {
    action->state->shape = this;
    action->state->rendercache->capture(action->state.get());
  }
}

void
Shape::traverse(BoundingBoxAction * action)
{
  box3 bbox(vec3(-1), vec3( 1));
  bbox.transform(action->state->transform);
  action->extendBy(bbox);
}

// *************************************************************************************************
