#pragma once

#include <glm/glm.hpp>

class box3 {
public:
  box3();
  box3(const glm::vec3 & min, const glm::vec3 & max);
  ~box3();

  void makeEmpty();
  void extendBy(const glm::vec3 & v);
  void transform(const glm::mat4 & mat);

  float size() const;
  glm::vec3 span() const;
  glm::vec3 center() const;
  
  glm::vec3 min;
  glm::vec3 max;
};