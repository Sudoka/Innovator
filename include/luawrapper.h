#pragma once
#include <string>
#include <lua.hpp>

class Lua {
public:
  Lua();
  ~Lua();

  void dofile(const std::string & file);
  void registerFunction(const std::string & name, lua_CFunction f);
  void * getglobaluserdata(const char * name);

  lua_State * L;
};