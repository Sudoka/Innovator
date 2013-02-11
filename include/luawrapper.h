#pragma once
#include <string>
#include <lua.hpp>

class Separator;

class Lua {
public:
  static void init();
  static void exit();

  static void dofile(const std::string & file);
  static void registerFunction(const std::string & name, lua_CFunction f);
  static lua_State * L;
};