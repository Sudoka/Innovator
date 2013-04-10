#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <luanode.h>
#include <fields.h>
#include <elements.h>

class GLProgram;
class GLBufferObject;
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
  MFNode shaders;
  SFString fileName;
  SFString feedbackVarying;
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
private:
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
