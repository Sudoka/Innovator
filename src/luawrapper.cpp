#include <luawrapper.h>
#include <nodes.h>

using namespace std;

Lua::Lua()
{
  L = luaL_newstate();
  luaL_openlibs(L);

  this->registerFunction("Group", Node::CreateInstance<Group>);
  this->registerFunction("Shape", Node::CreateInstance<Shape>);
  this->registerFunction("Texture", Node::CreateInstance<Texture>);
  this->registerFunction("Program", Node::CreateInstance<Program>);
  this->registerFunction("Material", Node::CreateInstance<Material>);
  this->registerFunction("Separator", Node::CreateInstance<Separator>);
  this->registerFunction("Transform", Node::CreateInstance<Transform>);
  this->registerFunction("TextureUnit", Node::CreateInstance<TextureUnit>);
  this->registerFunction("BoundingBox", Node::CreateInstance<BoundingBox>);
  this->registerFunction("ShaderObject", Node::CreateInstance<ShaderObject>);
  this->registerFunction("TextureSampler", Node::CreateInstance<TextureSampler>);
}

Lua::~Lua()
{
  lua_close(L);
}

void
Lua::registerFunction(const string & name, lua_CFunction f)
{
  lua_pushcfunction(L, f);
  lua_setglobal(L, name.c_str());
  lua_pop(L, -1);
}

void
Lua::dofile(const string & file)
{
  if (luaL_dofile(L, file.c_str()) != LUA_OK) {
    string message = luaL_checkstring(L, -1);
    throw runtime_error("Failed to open file: " + message);
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