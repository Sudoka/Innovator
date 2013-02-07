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

template <typename GroupType>
class LuaGroup : public LuaNode<GroupType> {
public:
  LuaGroup(const string & name) : LuaNode(name) {
    this->function("addChild", LuaGroup::addChild);
  }
  static int addChild(lua_State * L) {
    Node * child = LuaNode<Node>::getUserData(L, -1);
    Group * self = LuaNode<Group>::getUserData(L, -2);
    self->addChild(shared_ptr<Node>(child));
    return 0;
  }
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

class LuaTransform : public LuaNode<Transform> {
public:
  LuaTransform() : LuaNode("LuaTransform") {
    this->function("setScaleFactor", setScaleFactor);
    this->function("setTranslation", setTranslation);
  }
  static int setScaleFactor(lua_State * L) {
    Transform * self = LuaTransform::getUserData(L, -2);
    self->scaleFactor = LuaNode::getVec3(L, -1);
    return 0;
  }
  static int setTranslation(lua_State * L) {
    Transform * self = LuaTransform::getUserData(L, -2);
    self->translation = LuaNode::getVec3(L, -1);
    return 0;
  }
};

class LuaIndexBuffer : public LuaNode<IndexBuffer> {
public:
  LuaIndexBuffer() : LuaNode("LuaIndexBuffer") {
    this->function("setiVec3", LuaNode::setVector<int, 3>);
  }
};

class LuaVertexAttribute : public LuaNode<VertexAttribute> {
public:
  LuaVertexAttribute() : LuaNode("LuaVertexAttribute") {
    this->function("setVec3", LuaNode::setVector<float, 3>);
    this->function("setIndex", LuaVertexAttribute::setIndex);
  }
  static int setIndex(lua_State * L)
  {
    VertexAttribute * self = LuaNode<VertexAttribute>::getUserData(L, -2);
    self->index = LuaNode::getInteger(L, -1);
    return 0;
  }
};

void
File::init()
{
  Lua::dofile("../../src/file.lua");

  LuaProgram program;
  LuaTransform transform;
  LuaIndexBuffer indexbuffer;
  LuaVertexAttribute vertexattrib;

  LuaNode<Shape> shape("LuaShape");
  LuaShader<VertexShader> vertex("LuaVertexShader");
  LuaShader<GeometryShader> geometry("LuaGeometryShader");
  LuaShader<FragmentShader> fragment("LuaFragmentShader");

  LuaGroup<Group> group("LuaGroup");
  LuaGroup<Separator> separator("LuaSeparator");
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