#pragma once
#include <memory>
#include <string>
#include <lua.hpp>

class Separator;

class Lua {
public:
  Lua();
  ~Lua();

  void dofile(const std::string & file);
  void registerFunction(const std::string & name, lua_CFunction f);
  void * getglobaluserdata(const char * name);

  std::shared_ptr<Separator> readAll(const std::string & filename);

  lua_State * L;
};