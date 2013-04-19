#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <luanode.h>
#include <fields.h>
#include <elements.h>

class GLProgram;
class GLTextureUnit;
class GLBufferObject;
class GLTextureObject;
class GLTextureSampler;
class GLVertexAttribute;
class GLTransformFeedback;
class GLVertexArrayObject;

class State;
class Action;
class RenderAction;
class BoundingBoxAction;

class Node {
public:
  virtual ~Node() {}
  virtual void traverse(RenderAction * action) {}
  virtual void traverse(BoundingBoxAction * action) {}
  typedef std::shared_ptr<Node> ptr;
protected:
  std::vector<Field*> fields;
};

class Group : public Node {
  LUA_NODE_HEADER(Group);
public:
  Group();
  virtual ~Group();
  static void initClass();
  virtual void traverse(RenderAction * action);
  virtual void traverse(BoundingBoxAction * action);
  typedef std::shared_ptr<Group> ptr;
  MFNode children;
};

class Viewport : public Node {
  LUA_NODE_HEADER(Viewport);
public:
  Viewport();
  virtual ~Viewport();
  static void initClass();
  SFVec2i origin;
  SFVec2i size;
  virtual void traverse(RenderAction * action);
  void flush(State * state);
  typedef std::shared_ptr<Viewport> ptr;
};

class Separator : public Group {
  LUA_NODE_HEADER(Separator);
public:
  static void initClass();
  virtual void traverse(RenderAction * action);
  virtual void traverse(BoundingBoxAction * action);
  typedef std::shared_ptr<Separator> ptr;
};

class Camera : public Node {
public:
  Camera();
  virtual ~Camera();
  virtual void traverse(RenderAction * action);
  void flush(State * state);
  void zoom(float dz);
  void pan(const glm::vec2 & dx);
  void orbit(const glm::vec2 & dx);
  void moveTo(const glm::vec3 & position);
  void lookAt(const glm::vec3 & focalpoint);
  void viewAll(Separator::ptr root);
  glm::vec3 getFocalDir() const;
  void perspective(float fovy, float aspect, float near, float far);
  typedef std::shared_ptr<Camera> ptr;
private:
  class CameraP;
  std::unique_ptr<CameraP> self;
};

class ShaderObject : public Node {
  LUA_NODE_HEADER(ShaderObject);
public:
  ShaderObject();
  virtual ~ShaderObject();
  static void initClass();
  SFEnum type;
  SFString source;
};

class Program : public Node {
  LUA_NODE_HEADER(Program);
public:
  Program();
  virtual ~Program();
  static void initClass();
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

class Uniform : public Node {
public:
  Uniform();
  virtual ~Uniform();
  virtual void traverse(RenderAction * action);
  virtual void flush(State * state) = 0;
  SFInt location;
  SFString name;
};

class Uniform3f : public Uniform {
  LUA_NODE_HEADER(Uniform3f);
public:
  Uniform3f();
  virtual ~Uniform3f();
  static void initClass();
  virtual void flush(State * state);
  SFVec3f value;
};

class UniformMatrix4f : public Uniform {
  LUA_NODE_HEADER(UniformMatrix4f);
public:
  UniformMatrix4f();
  virtual ~UniformMatrix4f();
  static void initClass();
  virtual void flush(State * state);
  SFMatrix4f value;
};

class Transform : public Node {
  LUA_NODE_HEADER(Transform);
public:
  Transform();
  virtual ~Transform();
  static void initClass();
  virtual void traverse(RenderAction * action);
  virtual void traverse(BoundingBoxAction * action);
  SFVec3f translation;
  SFVec3f scaleFactor;
private:
  void doAction(Action * action);
};

class Buffer : public Node {
  LUA_NODE_HEADER(Buffer);
public:
  static void initClass();
  Buffer();
  virtual ~Buffer();
  SFInt count;
  SFEnum type;
  SFEnum usage;
  SFEnum target;
  MFNumber values;
  virtual void traverse(RenderAction * action);
  virtual void traverse(BoundingBoxAction * action);
  virtual GLuint getCount() const;
  std::unique_ptr<GLBufferObject> buffer;
};

class FeedbackBuffer : public Buffer {
  LUA_NODE_HEADER(FeedbackBuffer);
public:
  FeedbackBuffer();
  virtual ~FeedbackBuffer();
  static void initClass();
  SFSeparator scene;
  virtual void traverse(RenderAction * action);
  virtual GLuint getCount() const;

  void beginTransformFeedback();
  void endTransformFeedback();
private:
  friend class TransformFeedbackElement;
  class FeedbackBufferP;
  std::unique_ptr<FeedbackBufferP> self;
};

class VertexAttribute : public Node {
  LUA_NODE_HEADER(VertexAttribute);
public:
  VertexAttribute();
  virtual ~VertexAttribute();
  static void initClass();

  SFEnum type;
  SFUint size;
  SFUint index;
  SFUint divisor;
  SFBuffer buffer;

  virtual void traverse(RenderAction * action);
  virtual void traverse(BoundingBoxAction * action);

private:
  friend class VertexElement;
  std::unique_ptr<GLVertexAttribute> glattrib;
};

class TextureUnit : public Node {
  LUA_NODE_HEADER(TextureUnit);
public:
  TextureUnit();
  virtual ~TextureUnit();
  static void initClass();
  
  SFUint unit;
  virtual void traverse(RenderAction * action);

private:
  friend class TextureElement;
  std::unique_ptr<GLTextureUnit> gltexunit;
};

class Texture : public Node {
  LUA_NODE_HEADER(Texture);
public:
  Texture();
  virtual ~Texture();
  static void initClass();

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
  LUA_NODE_HEADER(TextureSampler);
public:
  TextureSampler();
  ~TextureSampler();
  static void initClass();

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

class SceneTexture : public Node {
  LUA_NODE_HEADER(SceneTexture);
public:
  SceneTexture();
  virtual ~SceneTexture();
  static void initClass();
  virtual void traverse(RenderAction * action);
  SFVec2i size;
  SFSeparator scene;
private:
  class SceneTextureP;
  std::unique_ptr<SceneTextureP> self;
};

class BoundingBox : public Node {
  LUA_NODE_HEADER(BoundingBox);
public:
  BoundingBox();
  static void initClass();
  SFVec3f min;
  SFVec3f max;
  virtual void traverse(BoundingBoxAction * action);
};

class Draw : public Node {
public:
  Draw();
  SFEnum mode;
  virtual void traverse(RenderAction * action);
  virtual void execute(State * state) = 0;
protected:
  std::unique_ptr<GLVertexArrayObject> vao;
};

class DrawArrays : public Draw {
  LUA_NODE_HEADER(DrawArrays);
public:
  static void initClass();
  virtual void execute(State * state);
};

class DrawElements : public Draw {
  LUA_NODE_HEADER(DrawElements);
public:
  static void initClass();
  virtual void execute(State * state);
};

class DrawArraysInstanced : public Draw {
  LUA_NODE_HEADER(DrawArraysInstanced);
public:
  static void initClass();
  virtual void execute(State * state);
};

class DrawElementsInstanced : public Draw {
  LUA_NODE_HEADER(DrawElementsInstanced);
public:
  static void initClass();
  virtual void execute(State * state);
};
