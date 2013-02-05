#include <luawrapper.h>
#include <iostream>
using namespace std;

lua_State * Lua::L = nullptr;

void
Lua::init()
{
  L = luaL_newstate();
  luaL_openlibs(L);
}

void
Lua::exit()
{
  lua_close(L);
}

void 
Lua::registerLib(const std::string & name, const luaL_Reg * functions)
{
  luaL_newlib(L, functions);
  lua_setglobal(L, name.c_str());
}

void
Lua::dofile(const std::string & file)
{
  int error = luaL_dofile(L, file.c_str());
  if (error) {
    cout << "Lua error: " << luaL_checkstring(L, -1) << endl;
  }
}

const char * 
Lua::getstring(const char * name)
{
  lua_getglobal(L, name);
  if (lua_isstring(L, -1)) {
    return lua_tostring(L, -1);
  }
  return nullptr;
}

void * 
Lua::getuserdata(const std::string name)
{
  lua_getglobal(L, name.c_str());
  luaL_checktype(L, -1, LUA_TLIGHTUSERDATA);
  return lua_touserdata(L, -1);
}
