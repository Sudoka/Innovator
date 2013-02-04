#include <luawrapper.h>
#include <iostream>
#include <nodes.h>
#include <glm/glm.hpp>

using namespace std;
using namespace glm;

template<class NodeType>
static int newNode(lua_State * L) 
{
  lua_pushlightuserdata(L, new NodeType);
  return 1;
}

static int AddChild(lua_State * L)
{
  luaL_checktype(L, -1, LUA_TLIGHTUSERDATA);
  luaL_checktype(L, -2, LUA_TLIGHTUSERDATA);

  Node * child = static_cast<Node *>(lua_touserdata(L, -1));
  Group * group = static_cast<Group *>(lua_touserdata(L, -2));
  assert(group && child);

  group->addChild(shared_ptr<Node>(child));
  return 0;
}

template<typename NodeType, typename T>
static int set_vec3(lua_State * L)
{
  luaL_checktype(L, -1, LUA_TTABLE);
  luaL_checktype(L, -2, LUA_TLIGHTUSERDATA);

  NodeType * node = static_cast<NodeType *>(lua_touserdata(L, -2));
  assert(node);

  int n = luaL_len(L, 2);
  node->values.resize(n / 3);
  T * dataptr = (T *) node->values.data();

  for (int i = 1; i <= n; i++) {
    lua_rawgeti(L, -1, i);
    dataptr[i-1] = (T)luaL_checknumber(L, -1);
    lua_pop(L, 1);
  }
  return 0;
}

template <typename NodeType>
static void registerNode(lua_State * L, const string & name)
{
  luaL_Reg functions[] = {
    { "new", newNode<NodeType> },
    { nullptr, nullptr } 
  };

  luaL_newlib(L, functions);
  lua_setglobal(L, name.c_str());
}

Lua::Lua()
{
  L = luaL_newstate();
  luaL_openlibs(L);
  int error = luaL_dofile(L, "../../src/file.lua");

  registerNode<Separator>(L, "Separator");
  registerNode<VertexAttribute>(L, "VertexAttribute");
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
