#include <file.h>
#include <lua.hpp>
#include <nodes.h>
#include <iostream>

using namespace std;

static int InstancedTriangleSet_new(lua_State * L)
{
  cout << "InstancedTriangleSet_new" << endl;
  lua_pushlightuserdata(L, new InstancedTriangleSet);
  luaL_getmetatable(L, "luaL_InstancedTriangleSet");
  lua_setmetatable(L, -2);
  return 1;
}

InstancedTriangleSet * checkInstancedTriangleSet(lua_State * L, int n)
{
  return *(InstancedTriangleSet **)luaL_checkudata(L, n, "luaL_InstancedTriangleSet");
}

static void registerInstancedTriangleSet(lua_State * L)
{
  luaL_Reg sInstancedTriangleSet[] = {
    { "new", InstancedTriangleSet_new },
    { nullptr, nullptr }
  };

  luaL_newmetatable(L, "luaL_InstancedTriangleSet");
  luaL_setfuncs(L, sInstancedTriangleSet, 0);
  lua_pushvalue(L, -1);
  lua_setfield(L, -1, "__index");
  lua_setglobal(L, "InstancedTriangleSet");
}

class File::FileP {
public:
  FileP() 
    : L(luaL_newstate())
  {
    luaL_openlibs(L);
    luaL_dofile(L, "../../src/file.lua");
    registerInstancedTriangleSet(L);
  }
  ~FileP() 
  {
    lua_close(L);
  }

  lua_State * L;
};

File::File()
  : self(new FileP)
{

}

File::~File()
{
}

shared_ptr<Separator> 
File::readAll(const string & filename)
{
  int error = luaL_dofile(self->L, filename.c_str());
  if (error) {
    cout << "Lua error: " << luaL_checkstring(self->L, -1) << endl;
  }
  shared_ptr<Separator> scene(new Separator);
  return scene;
}