#pragma once

#include <string>
#include <lua.hpp>
#include <glm/glm.hpp>

class Field {
public:
  virtual void read(lua_State * L) = 0;
  std::string name;
};

class SFString : public Field {
public:
  virtual void read(lua_State * L);
  std::string value;
};

class SFVec3f : public Field {
public:
  virtual void read(lua_State * L);
  glm::vec3 value;
};