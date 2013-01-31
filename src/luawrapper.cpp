#include <luawrapper.h>
#include <iostream>
#include <nodes.h>
#include <glm/glm.hpp>

using namespace std;
using namespace glm;

static int new_Separator(lua_State * L)
{
  lua_pushlightuserdata(L, new Separator);
  return 1;
}

static int new_InstancedTriangleSet(lua_State * L)
{
  lua_pushlightuserdata(L, new InstancedTriangleSet);
  return 1;
}

static int Separator_addChild(lua_State * L)
{
  luaL_checktype(L, -1, LUA_TLIGHTUSERDATA);
  luaL_checktype(L, -2, LUA_TLIGHTUSERDATA);

  Node * node = static_cast<Node *>(lua_touserdata(L, -1));
  Separator * sep = static_cast<Separator *>(lua_touserdata(L, -2));
  assert(node && sep);

  sep->addChild(shared_ptr<Node>(node));

  return 0;
}

static int InstancedTriangleSet_setIndices(lua_State * L)
{
  luaL_checktype(L, -1, LUA_TTABLE);
  luaL_checktype(L, -2, LUA_TLIGHTUSERDATA);

  InstancedTriangleSet * node = static_cast<InstancedTriangleSet *>(lua_touserdata(L, -2));
  assert(node);

  int n = luaL_len(L, 2);
  node->indices.resize(n / 3);
  int * dataptr = (int *) node->indices.data();

  for (int i = 1; i <= n; i++) {
    lua_rawgeti(L, -1, i);
    dataptr[i-1] = (int)luaL_checkinteger(L, -1);
    lua_pop(L, 1);
  }
  return 0;
}

static int InstancedTriangleSet_setVertices(lua_State * L)
{
  luaL_checktype(L, -1, LUA_TTABLE);
  luaL_checktype(L, -2, LUA_TLIGHTUSERDATA);

  InstancedTriangleSet * node = static_cast<InstancedTriangleSet *>(lua_touserdata(L, -2));
  assert(node);

  int n = luaL_len(L, 2);
  node->vertices.resize(n / 3);
  float * dataptr = (float *) node->vertices.data();

  for (int i = 1; i <= n; i++) {
    lua_rawgeti(L, -1, i);
    dataptr[i-1] = (float)luaL_checknumber(L, -1);
    lua_pop(L, 1);
  }
  return 0;
}

Lua::Lua()
{
  L = luaL_newstate();
  luaL_openlibs(L);
  int error = luaL_dofile(L, "../../src/file.lua");
  luaL_Reg its[] = {
    { "new", new_InstancedTriangleSet },
    { "setIndices", InstancedTriangleSet_setIndices },
    { "setVertices", InstancedTriangleSet_setVertices },
    { nullptr, nullptr }
  };
  luaL_Reg sep[] = {
    { "new", new_Separator },
    { "addChild", Separator_addChild },
    { nullptr, nullptr }
  };
  
  luaL_newlib(L, its);
  lua_setglobal(L, "InstancedTriangleSet2");
  luaL_newlib(L, sep);
  lua_setglobal(L, "Separator2");
}

Lua::~Lua()
{
  lua_close(L);
}

Lua *
Lua::instance()
{
  static Lua * self = new Lua;
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

void * 
Lua::getuserdata(const std::string name)
{
  Lua * self = Lua::instance();
  lua_getglobal(self->L, name.c_str());
  luaL_checktype(self->L, -1, LUA_TLIGHTUSERDATA);
  return lua_touserdata(self->L, -1);
}
