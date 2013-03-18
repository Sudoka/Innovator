#include <luawrapper.h>
#include <innovator.h>
#include <glm/glm.hpp>
#include <nodes.h>

using namespace std;
using namespace glm;

Lua::Lua()
{
  L = luaL_newstate();
  luaL_openlibs(L);

  this->dofile("src/file.lua");
}

Lua::~Lua()
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

void
Lua::dofile(const std::string & file)
{
  if (luaL_dofile(L, file.c_str()) != LUA_OK) {
    string message = luaL_checkstring(L, -1);
    throw std::runtime_error("Failed to open file: " + message);
  }
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

Separator::ptr
Lua::readAll(const string & filename)
{
  this->dofile(filename);
  Separator::ptr root(static_cast<Separator*>(this->getglobaluserdata("root")));

  // if there was no global userdata sep named root, see if
  // there is a separator on top of the stack
  if (!root.get() && !lua_isnil(this->L, -1) && lua_islightuserdata(this->L, -1)) {
    root.reset(static_cast<Separator*>(lua_touserdata(this->L, -1)));
  }
  return root;
}
