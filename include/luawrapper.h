#pragma once
#include <string>
#include <memory>
#include <lua.hpp>

class Separator;

class Lua {
public:
  static void init();
  static void exit();

  static void dofile(const std::string & file);
  static std::shared_ptr<Separator> doivfile(const std::string & file);
  static void * getuserdata(const std::string name);
  static bool isstring(const char * name);
  static const char * getstring(const char * name);
  static void registerLib(const std::string & name, const luaL_Reg * functions);

private:
  static lua_State * L;
};