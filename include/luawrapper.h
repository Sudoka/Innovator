#pragma once
#include <string>
#include <memory>
#include <lua.hpp>

class Separator;

class Lua {
public:
  static std::shared_ptr<Separator> doivfile(const std::string & file);
  static void dofile(const std::string & file);
  static void * getuserdata(const std::string name);

private:
  static Lua * instance();

  Lua();
  ~Lua();

  lua_State * L;
};