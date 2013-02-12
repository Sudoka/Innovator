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
Lua::registerFunction(const std::string & name, lua_CFunction f)
{
  lua_pushcfunction(L, f);
  lua_setglobal(L, name.c_str());
  lua_pop(L, -1);
}

bool
Lua::dofile(const std::string & file)
{
  int error = luaL_dofile(L, file.c_str());
  if (error) {
    cout << "Lua error: " << luaL_checkstring(L, -1) << endl;
    return false;
  }
  return true;
}

void * 
Lua::getglobaluserdata(const char * name)
{
  void * value = nullptr;
  lua_getglobal(L, name);
  if (!lua_isnil(L, -1)) {
    luaL_checktype(L, -1, LUA_TLIGHTUSERDATA);
    value = lua_touserdata(L, -1);
  }
  lua_pop(L, -1);
  return value;
}
