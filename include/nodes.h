#pragma once

#include <glm/glm.hpp>
#include <string>
#include <vector>

class Action;
class RenderAction;
class BoundingBoxAction;

class Node {
public:
  virtual void renderGL(RenderAction * action) = 0;
  virtual void getBoundingBox(BoundingBoxAction * action) {}
};

class Group : public Node {
public:
  Group();
  ~Group();
  virtual void renderGL(RenderAction * action);
  virtual void getBoundingBox(BoundingBoxAction * action);
  void addChild(std::shared_ptr<Node> child);
private:
  class GroupP;
  std::unique_ptr<GroupP> self;
};

class Camera : public Node {
public:
  Camera();
  ~Camera();
  virtual void renderGL(RenderAction * action);
  void zoom(float dz);
  void pan(const glm::vec2 & dx);
  void orbit(const glm::vec2 & dx);
  void moveTo(const glm::vec3 & position);
  void lookAt(const glm::vec3 & focalpoint);
  void viewAll(std::shared_ptr<Node> root);
  glm::vec3 getFocalDir() const;
  void perspective(float fovy, float aspect, float near, float far);

private:
  class CameraP;
  std::unique_ptr<CameraP> self;
};

class Separator : public Group {
public:
  Separator();
  ~Separator();
  virtual void renderGL(RenderAction * action);
  virtual void getBoundingBox(BoundingBoxAction * action);
};

class Shader : public Node {
public:
  Shader();
 ~Shader();
  std::string fileName;
  std::vector<const char *> transformFeedbackVaryings;
  virtual void renderGL(RenderAction * action);
private:
  class ShaderP;
  std::unique_ptr<ShaderP> self;
};

class Transform : public Node {
public:
  Transform();
  ~Transform();
  void doAction(Action * action);
  virtual void renderGL(RenderAction * action);
  virtual void getBoundingBox(BoundingBoxAction * action);
  glm::vec3 translation;
  glm::vec3 scaleFactor;
};

class Triangles : public Node {
public:
  Triangles();
  ~Triangles();
  std::vector<glm::vec3> vertices;
  virtual void renderGL(RenderAction * action);
  virtual void getBoundingBox(BoundingBoxAction * action);

private:
  class TrianglesP;
  std::unique_ptr<TrianglesP> self;
};

class InstancedTriangleSet : public Node {
public:
  InstancedTriangleSet();
  ~InstancedTriangleSet();

  std::vector<glm::ivec3> indices;
  std::vector<glm::vec3> vertices;
  std::vector<glm::vec3> instances;

  virtual void renderGL(RenderAction * action);
  virtual void getBoundingBox(BoundingBoxAction * action);

private:
  class InstancedTriangleSetP;
  std::unique_ptr<InstancedTriangleSetP> self;
};