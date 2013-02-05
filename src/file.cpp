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
  LuaNode(const string & name) 
    : name(name) 
  {
    this->function("new", newNode);
  }

  void function(const char * name, lua_CFunction func) 
  {
    luaL_Reg f = { name, func };
    functions.push_back(f);
  }

  void registerLua()
  {
    this->function(nullptr, nullptr);
    Lua::registerLib(this->name, this->functions.data());
  }

  static NodeType * getUserData(lua_State * L, int index)
  {
    luaL_checktype(L, index, LUA_TLIGHTUSERDATA);
    return static_cast<NodeType *>(lua_touserdata(L, index));
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

  static int newNode(lua_State * L)
  {
    lua_pushlightuserdata(L, new NodeType);
    return 1;
  }

  static int addChild(lua_State * L)
  {
    Node * child = LuaNode<Node>::getUserData(L, -1);
    Group * group = LuaNode<Group>::getUserData(L, -2);
    if (group && child) {
      group->addChild(shared_ptr<Node>(child));
    }
    return 0;
  }

  static int setFilename(lua_State * L)
  {
    Program * program = LuaNode<Program>::getUserData(L, -2);
    if (program) {
      program->fileName = LuaNode::getString(L, -1);
    }
    return 0;
  }

  static int setScaleFactor(lua_State * L)
  {
    Transform * transform = LuaNode<Transform>::getUserData(L, -2);
    if (transform) {
      transform->scaleFactor = LuaNode::getVec3(L, -1);
    }
    return 0;
  }

  static int setTranslation(lua_State * L)
  {
    Transform * transform = LuaNode<Transform>::getUserData(L, -2);
    if (transform) {
      transform->translation = LuaNode::getVec3(L, -1);
    }
    return 0;
  }

  template<typename T, int nc>
  static int setVector(lua_State * L)
  {
    NodeType * node = LuaNode::getUserData(L, -2);
    if (!node) return 0;

    luaL_checktype(L, -1, LUA_TTABLE);
    int n = luaL_len(L, -1);
    node->values.resize(n / nc);
    T * dataptr = (T *) node->values.data();
    
    for (int i = 1; i <= n; i++) {
      lua_rawgeti(L, -1, i);
      dataptr[i-1] = (T)luaL_checknumber(L, -1);
      lua_pop(L, 1);
    }
    return 0;
  }

private:
  string name;
  vector<luaL_Reg> functions;
};

void
File::init()
{
  Lua::dofile("../../src/file.lua");

  LuaNode<Shape> shape("Innovator_Shape");
  shape.registerLua();
  
  LuaNode<Program> program("Innovator_Program");
  program.function("setFilename", LuaNode<Program>::setFilename);
  program.registerLua();

  LuaNode<Transform> transform("Innovator_Transform");
  transform.function("setScaleFactor", LuaNode<Transform>::setScaleFactor);
  transform.function("setTranslation", LuaNode<Transform>::setTranslation);
  transform.registerLua();
  
  LuaNode<Group> group("Innovator_Group");
  group.function("addChild", LuaNode<Group>::addChild);
  group.registerLua();
  
  LuaNode<Separator> separator("Innovator_Separator");
  separator.function("addChild", LuaNode<Separator>::addChild);
  separator.registerLua();
  
  LuaNode<IndexBuffer> indexbuffer("Innovator_IndexBuffer");
  indexbuffer.function("setiVec3", LuaNode<IndexBuffer>::setVector<int, 3>);
  indexbuffer.registerLua();
  
  LuaNode<VertexAttribute> vertexbuffer("Innovator_VertexAttribute");
  vertexbuffer.function("setVec3", LuaNode<VertexAttribute>::setVector<float, 3>);
  vertexbuffer.registerLua();
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
  assert(scene);
  return std::shared_ptr<Separator>(scene);
}