#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <fields.h>
#include <elements.h>
#include <functional>

class GLProgram;
class GLTextureUnit;
class GLBufferObject;
class GLTextureObject;
class GLTextureSampler;
class GLVertexAttribute;
class GLVertexArrayObject;

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

private:
  std::unique_ptr<GLMatrix> viewmatrix;
  std::unique_ptr<GLMatrix> projmatrix;
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
private:
  std::unique_ptr<GLProgram> glprogram;
};

class Uniform : public FieldContainer {
public:
  Uniform();
  virtual ~Uniform();
  virtual void flush(State * state) {};
  SFString name;
};

class Uniform3f : public Uniform {
public:
  Uniform3f();
  virtual ~Uniform3f();
  SFVec3f value;
};

class UniformMatrix4f : public Uniform {
public:
  UniformMatrix4f();
  virtual ~UniformMatrix4f();
  SFMatrix4f value;
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

private:
  std::unique_ptr<GLMaterial> glmaterial;
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

class Buffer : public Node {
public:
  Buffer();
  virtual ~Buffer();
  virtual void traverse(RenderAction * action);

  SFInt count;
  SFEnum type;
  SFEnum usage;
  SFEnum target;
  MFNumber values;

private:
  std::unique_ptr<GLBufferObject> buffer;
};

class VertexAttribute : public Node {
public:
  VertexAttribute();
  virtual ~VertexAttribute();
  virtual void traverse(RenderAction * action);

  SFEnum type;
  SFUint size;
  SFUint index;
  SFUint divisor;

private:
  friend class VertexElement;
  std::unique_ptr<GLVertexAttribute> glattrib;
};

class TextureUnit : public Node {
public:
  TextureUnit();
  virtual ~TextureUnit();
  virtual void traverse(RenderAction * action);

  SFUint unit;

private:
  friend class TextureElement;
  std::unique_ptr<GLTextureUnit> gltexunit;
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

private:
  friend class TextureElement;
  std::unique_ptr<GLTextureObject> gltexture;
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

private:
  friend class TextureElement;
  std::unique_ptr<GLTextureSampler> glsampler;
};

class BoundingBox : public Node {
public:
  BoundingBox();
  virtual ~BoundingBox();
  virtual void traverse(BoundingBoxAction * action);

  SFVec3f min;
  SFVec3f max;
};

class DrawCall : public Node {
public:
  DrawCall();
  virtual ~DrawCall();

  SFEnum mode;
};

/*
class DrawArrays : public DrawCall {
public:
  DrawArrays();
  virtual ~DrawArrays();
  virtual void execute();

  SFInt first;
  SFInt count;
};
*/

class DrawElements : public DrawCall {
public:
  DrawElements();
  virtual ~DrawElements();
  virtual void traverse(RenderAction * action);

  SFEnum type;
  SFInt count;
  MFNumber indices;

private:
  class DrawElementsP;
  std::unique_ptr<DrawElementsP> self;
};
