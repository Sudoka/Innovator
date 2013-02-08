#include <file.h>
#include <lua.hpp>
#include <luawrapper.h>
#include <nodes.h>
#include <iostream>

using namespace std;
using namespace glm;

template<typename NodeType> 
class LuaNode {
public:
  LuaNode(const string & name) : name(name) {
    this->function("new", newNode);
  }

  virtual ~LuaNode() {
    this->function(nullptr, nullptr);
    Lua::registerLib(this->name, this->functions.data());
  }

  static int newNode(lua_State * L) 
  {
    lua_pushlightuserdata(L, new NodeType);
    return 1;
  }

  void function(const char * name, lua_CFunction func) 
  {
    luaL_Reg f = { name, func };
    functions.push_back(f);
  }

  static NodeType * getUserData(lua_State * L, int index)
  {
    luaL_checktype(L, index, LUA_TLIGHTUSERDATA);
    NodeType * node = static_cast<NodeType *>(lua_touserdata(L, index));
    assert(node);
    return node;
  }

  static int getInteger(lua_State * L, int index)
  {
    luaL_checktype(L, index, LUA_TNUMBER);
    return lua_tointeger(L, index);
  }

  static const char * getString(lua_State * L, int index)
  {
    luaL_checktype(L, index, LUA_TSTRING);
    return lua_tostring(L, index);
  }

  static vec3 getVec3(lua_State * L, int index)
  {
    luaL_checktype(L, index, LUA_TTABLE);
    int n = luaL_len(L, index);
    assert(n == 3);
    vec3 vec;
    for (int i = 1; i <= n; i++) {
      lua_rawgeti(L, -1, i);
      vec[i-1] = (float)luaL_checknumber(L, -1);
      lua_pop(L, 1);
    }
    return vec;
  }

  template<typename T, int nc>
  static int setVector(lua_State * L)
  {
    NodeType * self = LuaNode::getUserData(L, -2);
    luaL_checktype(L, -1, LUA_TTABLE);
    int n = luaL_len(L, -1);
    self->values.resize(n / nc);
    T * dataptr = (T *) self->values.data();
    
    for (int i = 1; i <= n; i++) {
      lua_rawgeti(L, -1, i);
      dataptr[i-1] = (T)luaL_checknumber(L, -1);
      lua_pop(L, 1);
    }
    return 0;
  }

protected:
  string name;
  vector<luaL_Reg> functions;
};

template <typename ShaderType>
class LuaShader : public LuaNode<ShaderType> {
public:
  LuaShader(const string & name) : LuaNode(name) {
    this->function("setSource", LuaShader::setSource);
  }
  static int setSource(lua_State * L) {
    ShaderObject * self = LuaShader::getUserData(L, -2);
    self->source = LuaNode::getString(L, -1);
    return 0;
  }
};

class LuaProgram : public LuaNode<Program> {
public:
  LuaProgram() : LuaNode<Program>("LuaProgram") {
    this->function("addShader", addShader);
  }
  static int addShader(lua_State * L) {
    Program * self = LuaProgram::getUserData(L, -2);
    ShaderObject * shader = LuaNode<ShaderObject>::getUserData(L, -1);
    self->shaders.push_back(shader);
    return 0;
  }
};

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

static void getChildren(lua_State * L, Group * group, const char * name)
{
  luaL_checktype(L, -1, LUA_TTABLE);
  lua_getfield(L, -1, name);
  int n = luaL_len(L, -1);

  for (int i = 1; i <= n; i++) {
    lua_rawgeti(L, -1, i);
    luaL_checktype(L, -1, LUA_TLIGHTUSERDATA);
    Node * node = static_cast<Node *>(lua_touserdata(L, -1));
    group->addChild(shared_ptr<Node>(node));
    lua_pop(L, 1);
  }
}

static void getShaders(lua_State * L, Program * program, const char * name)
{
  luaL_checktype(L, -1, LUA_TTABLE);
  lua_getfield(L, -1, name);
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
}

static int LuaVertexAttribute(lua_State * L)
{
  VertexAttribute * self = new VertexAttribute;
  getNumber<unsigned int>(L, self->index, "index");
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

template<typename NodeType>
static int LuaGroup(lua_State * L)
{
  NodeType * self = new NodeType;
  getChildren(L, self, "children");
  lua_pushlightuserdata(L, self);
  return 1;
}

static int LuaShape(lua_State * L)
{
  lua_pushlightuserdata(L, new Shape);
  return 1;
}

void
File::init()
{
  Lua::dofile("../../src/file.lua");

  LuaProgram program;

  Lua::registerFunction("Shape", LuaShape);
  Lua::registerFunction("Group", LuaGroup<Group>);
  Lua::registerFunction("Separator", LuaGroup<Separator>);
  Lua::registerFunction("Transform", LuaTransform);
  Lua::registerFunction("IndexBuffer", LuaIndexBuffer);
  Lua::registerFunction("VertexAttribute", LuaVertexAttribute);

  LuaShader<VertexShader> vertex("LuaVertexShader");
  LuaShader<GeometryShader> geometry("LuaGeometryShader");
  LuaShader<FragmentShader> fragment("LuaFragmentShader");

  //LuaGroup<Group> group("LuaGroup");
  //LuaGroup<Separator> separator("LuaSeparator");
}

void
File::exit()
{

}

shared_ptr<Separator> 
File::readAll(const string & filename)
{
  Lua::dofile(filename);
  Separator * scene = static_cast<Separator *>(Lua::getuserdata("root"));
  return std::shared_ptr<Separator>(scene);
}