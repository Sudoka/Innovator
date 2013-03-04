#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <luanode.h>
#include <fields.h>
#include <elements.h>

class GLBufferObject;
class GLVertexAttribute;

class State;
class Action;
class RenderAction;
class BoundingBoxAction;

class FieldContainer {
protected:
  std::vector<Field*> fields;
};

class Node : public FieldContainer {
public:
  virtual ~Node() {}
  virtual void traverse(RenderAction * action) {}
  virtual void traverse(BoundingBoxAction * action) {}
  typedef std::shared_ptr<Node> ptr;
};

class Group : public Node {
  LUA_NODE_HEADER(Group);
public:
  Group();
  virtual ~Group();
  static void initClass();
  virtual void traverse(RenderAction * action);
  virtual void traverse(BoundingBoxAction * action);
  void addChild(Node::ptr child);
  typedef std::shared_ptr<Group> ptr;
  MFNode children;
};

class Camera : public Node {
public:
  Camera();
  virtual ~Camera();
  virtual void traverse(RenderAction * action);
  void zoom(float dz);
  void pan(const glm::vec2 & dx);
  void orbit(const glm::vec2 & dx);
  void moveTo(const glm::vec3 & position);
  void lookAt(const glm::vec3 & focalpoint);
  void viewAll(Node::ptr root);
  glm::vec3 getFocalDir() const;
  void perspective(float fovy, float aspect, float near, float far);
  typedef std::shared_ptr<Camera> ptr;
private:
  class CameraP;
  std::unique_ptr<CameraP> self;
};

class Viewport : public Node {
public:
  Viewport();
  virtual ~Viewport();
  static void initClass();
  glm::ivec2 origin;
  glm::ivec2 size;
  virtual void traverse(RenderAction * action);
  typedef std::shared_ptr<Viewport> ptr;
};

class Separator : public Group {
  LUA_NODE_HEADER(Separator);
public:
  Separator();
  virtual ~Separator();
  static void initClass();
  virtual void traverse(RenderAction * action);
  virtual void traverse(BoundingBoxAction * action);
  typedef std::shared_ptr<Separator> ptr;
};

class ShaderObject : public Node {
public:
  ShaderObject(unsigned int type);
  SFString source;
  unsigned int type;
};

class VertexShader : public ShaderObject {
  LUA_NODE_HEADER(VertexShader);
public:
  VertexShader();
  static void initClass();
};

class GeometryShader : public ShaderObject {
  LUA_NODE_HEADER(GeometryShader);
public:
  GeometryShader();
  static void initClass();
};

class FragmentShader : public ShaderObject {
  LUA_NODE_HEADER(FragmentShader);
public:
  FragmentShader();
  static void initClass();
};

class Program : public Node {
  LUA_NODE_HEADER(Program);
public:
  Program();
  virtual ~Program();
  static void initClass();
  SFString fileName;
  MFNode shaders;

  virtual void traverse(RenderAction * action);

private:
  class ProgramP;
  std::unique_ptr<ProgramP> self;
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

template <typename MFValue>
class Buffer : public Node {
  LUA_NODE_HEADER(Buffer);
public:
  Buffer();
  virtual ~Buffer();
  enum Target {
    ELEMENT_ARRAY = GL_ELEMENT_ARRAY_BUFFER,
    ARRAY         = GL_ARRAY_BUFFER
  };
  enum Usage {
    STATIC_DRAW  = GL_STATIC_DRAW,
    DYNAMIC_DRAW = GL_DYNAMIC_DRAW
  };
  SFEnum target;
  SFEnum usage;
  MFValue values;

  virtual void traverse(RenderAction * action);

protected:
  void doAction(Action * action);
  std::unique_ptr<GLBufferObject> buffer;
};

class Vec3Buffer : public Buffer<MFVec3f> {
  LUA_NODE_HEADER(Vec3Buffer);
public:
  static void initClass();
  virtual void traverse(BoundingBoxAction * action);
};

class IntBuffer : public Buffer<MFint> {
  LUA_NODE_HEADER(IntBuffer);
public:
  static void initClass();
};

class VertexAttribute : public Node {
  LUA_NODE_HEADER(VertexAttribute);
public:
  VertexAttribute();
  virtual ~VertexAttribute();
  static void initClass();
  SFint index;
  SFint divisor;

  virtual void traverse(RenderAction * action);
  virtual void traverse(BoundingBoxAction * action);

private:
  void doAction(Action * action);
  std::unique_ptr<GLVertexAttribute> attribute;
};

class Draw : public Node {
public:
  Draw();
  virtual ~Draw();

  enum Mode {
    POINTS = GL_POINTS,
    TRIANGLES = GL_TRIANGLES
  };

  SFEnum mode;

  virtual void traverse(RenderAction * action);
  virtual void execute(State * state) = 0;
};

class DrawArrays : public Draw {
  LUA_NODE_HEADER(DrawArrays);
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

class DrawElements : public Draw {
  LUA_NODE_HEADER(DrawElements);
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
