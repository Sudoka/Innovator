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
}

void
Lua::dofile(const std::string & file)
{
  int error = luaL_dofile(L, file.c_str());
  if (error) {
    cout << "Lua error: " << luaL_checkstring(L, -1) << endl;
  }
}
