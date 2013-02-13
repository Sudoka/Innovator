#include <file.h>
#include <lua.hpp>
#include <luawrapper.h>
#include <nodes.h>
#include <iostream>

using namespace std;
using namespace glm;

template <typename T>
static void getVec3(lua_State * L, detail::tvec3<T> & vec, const char * name)
{
  luaL_checktype(L, -1, LUA_TTABLE);
  lua_getfield(L, -1, name);

  luaL_checktype(L, -1, LUA_TTABLE);
  if (!lua_isnil(L, -1)) {
    int n = luaL_len(L, -1);
    assert(n == 3);
    
    for (int i = 0; i < n; i++) {
      lua_rawgeti(L, -1, i + 1);
      vec[i] = (T)luaL_checknumber(L, -1);
      lua_pop(L, 1);
    }
  }
  lua_pop(L, 1);
}

template <typename T>
static void getNumber(lua_State * L, T & number, const char * name)
{
  luaL_checktype(L, -1, LUA_TTABLE);
  lua_getfield(L, -1, name);
  if (!lua_isnil(L, -1)) {
    number = luaL_checkinteger(L, -1);
  }
  lua_pop(L, 1);
}

static void getString(lua_State * L, string & string, const char * name)
{
  luaL_checktype(L, -1, LUA_TTABLE);
  lua_getfield(L, -1, name);
  if (!lua_isnil(L, -1)) {
    string = luaL_checkstring(L, -1);
  }
  lua_pop(L, 1);
}

static void getChildren(lua_State * L, Group * group)
{
  luaL_checktype(L, -1, LUA_TTABLE);
  int n = luaL_len(L, -1);

  for (int i = 1; i <= n; i++) {
    lua_rawgeti(L, -1, i);
    luaL_checktype(L, -1, LUA_TLIGHTUSERDATA);
    Node * node = static_cast<Node *>(lua_touserdata(L, -1));
    group->addChild(shared_ptr<Node>(node));
    lua_pop(L, 1);
  }
}

static void getShaders(lua_State * L, Program * program)
{
  luaL_checktype(L, -1, LUA_TTABLE);
  int n = luaL_len(L, -1);

  for (int i = 1; i <= n; i++) {
    lua_rawgeti(L, -1, i);
    luaL_checktype(L, -1, LUA_TLIGHTUSERDATA);
    ShaderObject * shader = static_cast<ShaderObject *>(lua_touserdata(L, -1));
    assert(shader);
    program->shaders.push_back(shader);
    lua_pop(L, 1);
  }
}

template <typename T>
static void getVector(lua_State * L, vector<detail::tvec3<T>> & values, const char * name)
{
  luaL_checktype(L, -1, LUA_TTABLE);
  lua_getfield(L, -1, name);
  int n = luaL_len(L, -1);

  values.resize(n / 3);
  T * dataptr = (T *) values.data();
  
  for (int i = 1; i <= n; i++) {
    lua_rawgeti(L, -1, i);
    dataptr[i-1] = (T)luaL_checknumber(L, -1);
    lua_pop(L, 1);
  }
  lua_pop(L, 1);
}

static int LuaVertexAttribute(lua_State * L)
{
  VertexAttribute * self = new VertexAttribute;
  getNumber<unsigned int>(L, self->index, "index");
  getNumber<unsigned int>(L, self->divisor, "divisor");
  getVector<float>(L, self->values, "values");
  lua_pushlightuserdata(L, self);
  return 1;
}

static int LuaIndexBuffer(lua_State * L)
{
  IndexBuffer * self = new IndexBuffer;
  getVector<int>(L, self->values, "values");
  lua_pushlightuserdata(L, self);
  return 1;
}

static int LuaTransform(lua_State * L)
{
  Transform * self = new Transform;
  getVec3(L, self->translation, "translation");
  getVec3(L, self->scaleFactor, "scaleFactor");
  lua_pushlightuserdata(L, self);
  return 1;
}

static int LuaProgram(lua_State * L)
{
  Program * self = new Program;
  getShaders(L, self);
  lua_pushlightuserdata(L, self);
  return 1;
}

template <typename ShaderType>
static int LuaShader(lua_State * L)
{
  ShaderType * self = new ShaderType;
  getString(L, self->source, "source");
  lua_pushlightuserdata(L, self);
  return 1;
}

static int LuaGroup(lua_State * L)
{
  Group * self = new Group;
  getChildren(L, self);
  lua_pushlightuserdata(L, self);
  return 1;
}

static int LuaSeparator(lua_State * L)
{
  Separator * self = new Separator;
  getNumber<int>(L, self->boundingBoxCaching, "boundingBoxCaching");
  getChildren(L, self);
  lua_pushlightuserdata(L, self);
  return 1;
}

static Draw::Mode
DrawMode(lua_State * L) {
  string name;
  getString(L, name, "mode");
  if (!name.empty()) {
    if (name == "POINTS") {
      return Draw::POINTS;
    } else if (name == "TRIANGLES") {
      return Draw::TRIANGLES;
    } else {
      throw std::runtime_error("Invalid Draw mode");
    }
  }
  return Draw::POINTS;
}

template <typename ShapeType>
static int LuaDraw(lua_State * L)
{
  Draw::Mode mode = DrawMode(L);
  ShapeType * self = new ShapeType;
  self->mode = mode;
  lua_pushlightuserdata(L, self);
  return 1;
}

void
File::init()
{
  Lua::registerFunction("DrawArrays", LuaDraw<DrawArrays>);
  Lua::registerFunction("DrawArraysInstanced", LuaDraw<DrawArraysInstanced>);
  Lua::registerFunction("DrawElements", LuaDraw<DrawElements>);
  Lua::registerFunction("DrawElementsInstanced", LuaDraw<DrawElementsInstanced>);
  Lua::registerFunction("Program", LuaProgram);
  Lua::registerFunction("Group", LuaGroup);
  Lua::registerFunction("Separator", LuaSeparator);
  Lua::registerFunction("Transform", LuaTransform);
  Lua::registerFunction("IndexBuffer", LuaIndexBuffer);
  Lua::registerFunction("VertexAttribute", LuaVertexAttribute);
  Lua::registerFunction("VertexShader", LuaShader<VertexShader>);
  Lua::registerFunction("GeometryShader", LuaShader<GeometryShader>);
  Lua::registerFunction("FragmentShader", LuaShader<FragmentShader>);
  Lua::dofile("../../src/file.lua");
}

void
File::exit()
{

}

Separator::ptr
File::readAll(const string & filename)
{
  Lua::dofile(filename);
   
  Separator::ptr root(static_cast<Separator*>(Lua::getglobaluserdata("root")));

  // if there was no global userdata sep named root, see if
  // there is a separator on top of the stack
  if (!root.get() && !lua_isnil(Lua::L, -1) && lua_islightuserdata(Lua::L, -1)) {
    root.reset(static_cast<Separator*>(lua_touserdata(Lua::L, -1)));
  }

  return root;
}