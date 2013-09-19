#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <fields.h>

class State;
class Action;
class RenderAction;
class BoundingBoxAction;

class FieldContainer {
public:
  template <typename T>
  static int CreateInstance(lua_State * L) {
    T * self = new T;
    for each(Field * field in self->fields) {
      field->read(L);
    }
    lua_pushlightuserdata(L, self);
    return 1;
  }
protected:
  void registerField(Field & field) {
    this->fields.push_back(&field);
  }
  void registerField(Field & field, const std::string & name) {
    field.name = name;
    this->fields.push_back(&field);
  }
  template<typename FieldType, typename ValueType>
  void registerField(FieldType & field, const std::string & name, const ValueType & value) {
    field.name = name;
    field.value = value;
    this->fields.push_back(&field);
  }
  void registerEnum(SFEnum & field, const std::string & name, GLenum value) {
    field.enums[name] = value;
  }
  std::vector<Field*> fields;
};

class Node : public FieldContainer {
public:
  virtual ~Node() {}
  virtual void traverse(RenderAction * action) {}
  virtual void traverse(BoundingBoxAction * action) {}
};

class Group : public Node {
public:
  Group();
  virtual ~Group();
  virtual void traverse(RenderAction * action);
  virtual void traverse(BoundingBoxAction * action);
  MFNode children;
};

class Separator : public Group {
public:
  Separator();
  virtual ~Separator();

  virtual void traverse(RenderAction * action);
  virtual void traverse(BoundingBoxAction * action);

  enum RenderCaching { ON, OFF };
  SFEnum renderCaching;

private:
  class SeparatorP;
  std::unique_ptr<SeparatorP> self;
};

class Camera : public Node {
public:
  Camera();
  virtual ~Camera();
  virtual void traverse(RenderAction * action);

  SFFloat farPlane;
  SFVec3f position;
  SFFloat nearPlane;
  SFFloat aspectRatio;
  SFFloat fieldOfView;
  SFFloat focalDistance;
  SFMatrix3f orientation;

  void zoom(float dz);
  void viewAll(Separator * root);
  void pan(const glm::vec2 & dx);
  void orbit(const glm::vec2 & dx);
  void lookAt(const glm::vec3 & focalpoint);
};

class ShaderObject : public FieldContainer {
public:
  ShaderObject();
  virtual ~ShaderObject();
  SFEnum type;
  SFString source;
};

class Program : public Node {
public:
  Program();
  virtual ~Program();
  virtual void traverse(RenderAction * action);
  MFShader shaders;
};

class Material : public Node {
public:
  Material();
  virtual ~Material();
  virtual void traverse(RenderAction * action);

  SFVec3f ambient;
  SFVec3f diffuse;
  SFVec3f specular;
  SFFloat shininess;
  SFFloat transparency;
};

class Transform : public Node {
public:
  Transform();
  virtual ~Transform();
  virtual void traverse(RenderAction * action);
  virtual void traverse(BoundingBoxAction * action);

  SFVec3f translation;
  SFVec3f scaleFactor;
private:
  void doAction(Action * action);
};

class TextureUnit : public Node {
public:
  TextureUnit();
  virtual ~TextureUnit();
  virtual void traverse(RenderAction * action);

  SFUint unit;
};

class Texture : public Node {
public:
  Texture();
  virtual ~Texture();
  virtual void traverse(RenderAction * action);

  SFString fileName;
  SFEnum target;
  SFInt level;
  SFUint width;
  SFUint height;
  SFEnum format;
  SFInt border;
  SFEnum type;
  SFEnum internalFormat;
};

class TextureSampler : public Node {
public:
  TextureSampler();
  ~TextureSampler();

  SFEnum wrapS;
  SFEnum wrapT;
  SFEnum wrapR;
  SFEnum minFilter;
  SFEnum magFilter;

  virtual void traverse(RenderAction * action);
};

class BoundingBox : public Node {
public:
  BoundingBox();
  virtual ~BoundingBox();
  virtual void traverse(BoundingBoxAction * action);

  SFVec3f min;
  SFVec3f max;
};

class Shape : public Node {
public:
  Shape();
  virtual ~Shape();
  virtual void traverse(RenderAction * action);
  virtual void traverse(BoundingBoxAction * action);

  MFNumber indices;
  MFNumber vertices;
};