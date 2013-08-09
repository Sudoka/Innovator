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

class Viewport : public Node {
public:
  Viewport();
  virtual ~Viewport();
  SFVec2i origin;
  SFVec2i size;
  virtual void traverse(RenderAction * action);
  void flush(State * state);
};

class Separator : public Group {
public:
  virtual void traverse(RenderAction * action);
  virtual void traverse(BoundingBoxAction * action);
};

class Camera : public Node {
public:
  Camera();
  virtual ~Camera();
  SFVec3f position;
  SFFloat focalDistance;
  virtual void traverse(RenderAction * action);
  void flush(State * state);
  void zoom(float dz);
  void pan(const glm::vec2 & dx);
  void orbit(const glm::vec2 & dx);
  void lookAt(const glm::vec3 & focalpoint);
  void viewAll(Separator * root);
  void perspective(float fovy, float aspect, float near, float far);
private:
  class CameraP;
  std::unique_ptr<CameraP> self;
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
  MFShader shaders;
  MFUniform uniforms;
  SFString fileName;
  SFString feedbackVarying;
  virtual void traverse(RenderAction * action);
  void flush(State * state);
  GLint getUniformLocation(const std::string & name);
private:
  class ProgramP;
  std::unique_ptr<ProgramP> self;
};

class Uniform : public FieldContainer {
public:
  Uniform();
  virtual ~Uniform();
  virtual void flush(State * state) = 0;
  SFString name;
};

class Uniform3f : public Uniform {
public:
  Uniform3f();
  virtual ~Uniform3f();
  virtual void flush(State * state);
  SFVec3f value;
};

class UniformMatrix4f : public Uniform {
public:
  UniformMatrix4f();
  virtual ~UniformMatrix4f();
  virtual void flush(State * state);
  SFMatrix4f value;
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
  SFInt count;
  SFEnum type;
  SFEnum usage;
  SFEnum target;
  MFNumber values;
  virtual void traverse(RenderAction * action);
  std::unique_ptr<GLBufferObject> buffer;
};

class VertexAttribute : public Node {
public:
  VertexAttribute();
  virtual ~VertexAttribute();

  SFEnum type;
  SFUint size;
  SFUint index;
  SFUint divisor;

  virtual void traverse(RenderAction * action);
private:
  friend class VertexElement;
  std::unique_ptr<GLVertexAttribute> glattrib;
};

class TextureUnit : public Node {
public:
  TextureUnit();
  virtual ~TextureUnit();
  
  SFUint unit;
  virtual void traverse(RenderAction * action);

private:
  friend class TextureElement;
  std::unique_ptr<GLTextureUnit> gltexunit;
};

class Texture : public Node {
public:
  Texture();
  virtual ~Texture();

  SFString fileName;
  SFEnum target;
  SFInt level;
  SFUint width;
  SFUint height;
  SFEnum format;
  SFInt border;
  SFEnum type;
  SFEnum internalFormat;

  virtual void traverse(RenderAction * action);

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
  SFVec3f min;
  SFVec3f max;
  virtual void traverse(BoundingBoxAction * action);
};

class DrawCall : public Node {
public:
  DrawCall();
  virtual ~DrawCall();
  SFEnum mode;
  virtual void traverse(RenderAction * action);
  virtual void execute(State * state) = 0;
protected:
  std::unique_ptr<GLVertexArrayObject> vao;
};

class DrawArrays : public DrawCall {
public:
  DrawArrays();
  virtual ~DrawArrays();
  SFInt first;
  SFInt count;
  virtual void execute(State * state);
};

class DrawElements : public DrawCall {
public:
  DrawElements();
  virtual ~DrawElements();
  SFEnum type;
  SFInt count;
  MFNumber indices;
  virtual void execute(State * state);
};
