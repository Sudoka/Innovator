#pragma once

#include <glm/glm.hpp>
#include <string>
#include <vector>

class RenderAction;
class BoundingBoxAction;

class Node {
public:
  virtual void renderGL(RenderAction * action) = 0;
};

class Group : public Node {
public:
  Group();
  ~Group();
  virtual void renderGL(RenderAction * action);
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
  glm::vec3 getFocalDir() const;
  void perspective(float fovy, float aspect, float near, float far);

private:
  class CameraP;
  std::unique_ptr<CameraP> self;
};

class Separator : public Group {
public:
  virtual void renderGL(RenderAction * action);
};

class Shader : public Node {
public:
  Shader();
 ~Shader();
  std::string fileName;
  virtual void renderGL(RenderAction * action);
private:
  class ShaderP;
  std::unique_ptr<ShaderP> self;
};

class Transform : public Node {
public:
  Transform();
  ~Transform();
  virtual void renderGL(RenderAction * action);
  glm::vec3 translation;
  glm::vec3 scaleFactor;
};

class Triangles : public Node {
public:
  Triangles();
  ~Triangles();
  std::vector<glm::vec3> vertices;
  virtual void renderGL(RenderAction * action);

private:
  class TrianglesP;
  std::unique_ptr<TrianglesP> self;
};