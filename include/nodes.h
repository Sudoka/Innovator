#pragma once

#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <glstate.h>

class State;
class Action;
class RenderAction;
class ShaderProgram;
class BoundingBoxAction;

class Node : public Bindable {
public:
  virtual ~Node() {}
  virtual void traverse(RenderAction * action) = 0;
  virtual void traverse(BoundingBoxAction * action) {}

  virtual void bind() {}
  virtual void unbind() {}

  typedef std::shared_ptr<Node> ptr;
};

class Group : public Node {
public:
  Group();
  virtual ~Group();
  virtual void traverse(RenderAction * action);
  virtual void traverse(BoundingBoxAction * action);
  void addChild(Node::ptr child);
  typedef std::shared_ptr<Group> ptr;
private:
  class GroupP;
  std::unique_ptr<GroupP> self;
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
  glm::ivec2 origin;
  glm::ivec2 size;
  virtual ~Viewport();
  virtual void traverse(RenderAction * action);
  typedef std::shared_ptr<Viewport> ptr;
};

class Separator : public Group {
public:
  Separator();
  virtual ~Separator();
  int boundingBoxCaching;
  virtual void traverse(RenderAction * action);
  virtual void traverse(BoundingBoxAction * action);
};

class ShaderObject {
public:
  virtual void attach(ShaderProgram * program) = 0;
  std::string source;
};

class VertexShader : public ShaderObject {
public:
  virtual void attach(ShaderProgram * program);
};

class GeometryShader : public ShaderObject {
public:
  virtual void attach(ShaderProgram * program);
};

class FragmentShader : public ShaderObject {
public:
  virtual void attach(ShaderProgram * program);
};

class Program : public Node {
public:
  Program();
  virtual ~Program();

  std::string fileName;
  std::vector<ShaderObject*> shaders;

  virtual void traverse(RenderAction * action);
  unsigned int getProgramId() const;

private:
  virtual void bind();
  virtual void unbind();

private:
  class ProgramP;
  std::unique_ptr<ProgramP> self;
};

class Transform : public Node {
public:
  Transform();
  virtual ~Transform();
  virtual void traverse(RenderAction * action);
  virtual void traverse(BoundingBoxAction * action);
  glm::vec3 translation;
  glm::vec3 scaleFactor;
private:
  void doAction(Action * action);
};

class IndexBuffer : public Node {
public:
  IndexBuffer();
  virtual ~IndexBuffer();

  virtual void traverse(RenderAction * action);
  std::vector<glm::ivec3> values;

private:
  friend class AttributeElement;
  virtual void bind();
  virtual void unbind();

  class IndexBufferP;
  std::unique_ptr<IndexBufferP> self;
};

class VertexAttribute : public Node {
public:
  VertexAttribute();
  virtual ~VertexAttribute();

  virtual void traverse(RenderAction * action);
  virtual void traverse(BoundingBoxAction * action);
  std::vector<glm::vec3> values;
  unsigned int index;
  unsigned int divisor;

private:
  void doAction(Action * action);
  friend class AttributeElement;
  virtual void bind();
  virtual void unbind();

private:
  class VertexAttributeP;
  std::unique_ptr<VertexAttributeP> self;
};

class Draw : public Node {
public:
  Draw();
  virtual ~Draw();

  enum Mode {
    POINTS,
    TRIANGLES,
    NUM_MODES
  };

  Mode mode;

  virtual void traverse(RenderAction * action);
  virtual void traverse(BoundingBoxAction * action);
  virtual void execute(State * state) = 0;
};

class DrawArrays : public Draw {
public:
  virtual void execute(State * state);
};

class DrawArraysInstanced : public Draw {
public:
  virtual void execute(State * state);
};

class DrawElements : public Draw {
public:
  virtual void execute(State * state);
};

class DrawElementsInstanced : public Draw {
public:
  virtual void execute(State * state);
};

class InstancedTriangleSet : public Node {
public:
  InstancedTriangleSet();
  virtual ~InstancedTriangleSet();

  std::vector<glm::ivec3> indices;
  std::vector<glm::vec3> vertices;
  std::vector<glm::vec3> normals;
  std::vector<glm::vec3> instances;

  virtual void traverse(RenderAction * action);
  virtual void traverse(BoundingBoxAction * action);

private:
  class InstancedTriangleSetP;
  std::unique_ptr<InstancedTriangleSetP> self;
};
