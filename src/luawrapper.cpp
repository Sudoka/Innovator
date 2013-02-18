#include <luawrapper.h>
#include <iostream>
#include <innovator.h>

using namespace std;
using namespace glm;

template <typename T>
static void getField(lua_State * L, detail::tvec3<T> & vec, const char * name)
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
static void getField(lua_State * L, T & number, const char * name)
{
  luaL_checktype(L, -1, LUA_TTABLE);
  lua_getfield(L, -1, name);
  if (!lua_isnil(L, -1)) {
    number = luaL_checkinteger(L, -1);
  }
  lua_pop(L, 1);
}

static void getField(lua_State * L, string & string, const char * name)
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
static void getField(lua_State * L, vector<detail::tvec3<T>> & values, const char * name)
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

static int LuaIndexBuffer(lua_State * L)
{
  IndexBuffer * self = new IndexBuffer;
  getField<int>(L, self->values, "values");
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

template <typename NodeType>
static int LuaNode(lua_State * L)
{
  lua_pushlightuserdata(L, NodeType::createInstance(L));
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
  getField<int>(L, self->boundingBoxCaching, "boundingBoxCaching");
  getChildren(L, self);
  lua_pushlightuserdata(L, self);
  return 1;
}

template <typename ShapeType>
static void getDrawMode(lua_State * L, ShapeType * shape) {
  string name;
  getField(L, name, "mode");
  if (name.empty()) return;

  Lua * lua = Innovator::lua();
  if (lua->drawmodes.find(name) == lua->drawmodes.end()) {
    Innovator::postError("Invalid Draw mode: " + name);
    return;
  }
  shape->mode = lua->drawmodes[name];
}

template <typename ShapeType>
static int LuaDraw(lua_State * L)
{
  ShapeType * self = new ShapeType;
  getDrawMode(L, self);
  lua_pushlightuserdata(L, self);
  return 1;
}

Lua::Lua()
{
  L = luaL_newstate();
  luaL_openlibs(L);

  this->registerFunction("DrawArrays", LuaDraw<DrawArrays>);
  this->registerFunction("DrawArraysInstanced", LuaDraw<DrawArraysInstanced>);
  this->registerFunction("DrawElements", LuaDraw<DrawElements>);
  this->registerFunction("DrawElementsInstanced", LuaDraw<DrawElementsInstanced>);
  this->registerFunction("Program", LuaProgram);
  this->registerFunction("Group", LuaGroup);
  this->registerFunction("Separator", LuaSeparator);
  this->registerFunction("Transform", LuaNode<Transform>);
  this->registerFunction("IndexBuffer", LuaIndexBuffer);
  this->registerFunction("VertexAttribute", LuaNode<VertexAttribute>);
  this->registerFunction("VertexShader", LuaNode<VertexShader>);
  this->registerFunction("GeometryShader", LuaNode<GeometryShader>);
  this->registerFunction("FragmentShader", LuaNode<FragmentShader>);
  this->dofile("../../src/file.lua");

  this->drawmodes["POINTS"] = Draw::POINTS;
  this->drawmodes["TRIANGLES"] = Draw::TRIANGLES;
}

Lua::~Lua()
{
  cout << "Lua::exit()" << endl;
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
