#pragma once

#include <string>
#include <lua.hpp>

class Field {
public:
  virtual void read(lua_State * L) = 0;
  std::string name;
};

class StringField : public Field {
public:
  virtual void read(lua_State * L);
  std::string value;
};