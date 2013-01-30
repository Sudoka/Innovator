#include <luawrapper.h>
#include <iostream>
#include <nodes.h>
#include <glm/glm.hpp>

using namespace std;
using namespace glm;

Lua::Lua()
{
  L = luaL_newstate();
  luaL_openlibs(L);
}

Lua::~Lua()
{
  lua_close(L);
}

static int new_InstancedTriangleSet(lua_State * L)
{
  lua_pushlightuserdata(L, new InstancedTriangleSet);
  return 1;
}

static int InstancedTriangleSet_setIndices(lua_State * L)
{
  luaL_checktype(L, -1, LUA_TTABLE);
  luaL_checktype(L, -2, LUA_TLIGHTUSERDATA);

  InstancedTriangleSet * node = static_cast<InstancedTriangleSet *>(lua_touserdata(L, -2));
  if (!node) { 
    cout << "invalid argument: node" << endl;
    return 0;
  }

  int n = luaL_len(L, 2);
  node->indices.resize(n / 3);
  int * dataptr = (int *) node->indices.data();

  for (int i = 1; i <= n; i++) {
    lua_rawgeti(L, -1, i);
    dataptr[i-1] = luaL_checkinteger(L, -1);
    lua_pop(L, 1);
  }
  return 0;
}

void 
Lua::registerNodes()
{
  Lua * self = Lua::instance();
  luaL_Reg its[] = {
    { "new", new_InstancedTriangleSet },
    { "setIndices", InstancedTriangleSet_setIndices },
    { nullptr, nullptr }
  };

  luaL_newlib(self->L, its);
  lua_setglobal(self->L, "InstancedTriangleSet2");
}

Lua *
Lua::instance()
{
  static Lua * self = new Lua();
  return self;
}

void
Lua::dofile(const std::string & file)
{
  Lua * self = Lua::instance();
  int error = luaL_dofile(self->L, file.c_str());
  if (error) {
    cout << "Lua error: " << luaL_checkstring(self->L, -1) << endl;
  }
}

std::shared_ptr<Separator>
Lua::doivfile(const std::string & file)
{
  Lua * self = Lua::instance();
  int error = luaL_dofile(self->L, file.c_str());
  if (error) {
    cout << "Lua error: " << luaL_checkstring(self->L, -1) << endl;
  }

  return std::shared_ptr<Separator>(nullptr);
}