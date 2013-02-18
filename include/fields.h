#pragma once

#include <string>
#include <vector>
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

class SFFloat : public Field {
public:
  virtual void read(lua_State * L);
  float value;
};

class SFUint32 : public Field {
public:
  virtual void read(lua_State * L);
  unsigned int value;
};

class MFVec3f : public Field {
public:
  virtual void read(lua_State * L);
  std::vector<glm::vec3> vec;
};
