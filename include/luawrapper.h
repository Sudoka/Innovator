#pragma once
#include <string>
#include <lua.hpp>

class Separator;

class Lua {
public:
  static void init();
  static void exit();

  static bool dofile(const std::string & file);
  static void registerFunction(const std::string & name, lua_CFunction f);
  static void * getglobaluserdata(const char * name);
  static lua_State * L;
};