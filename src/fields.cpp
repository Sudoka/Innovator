#include <fields.h>
#include <innovator.h>
#include <luawrapper.h>
#include <nodes.h>

using namespace std;
using namespace glm;

template <typename T>
static void ReadNumber(lua_State * L, T & number, const char * name)
{
  luaL_checktype(L, -1, LUA_TTABLE);
  lua_getfield(L, -1, name);
  if (!lua_isnil(L, -1)) {
    number = static_cast<T>(luaL_checknumber(L, -1));
  }
  lua_pop(L, 1);
}

static void ReadString(lua_State * L, string & value, const char * name)
{
  luaL_checktype(L, -1, LUA_TTABLE);
  lua_getfield(L, -1, name);
  if (!lua_isnil(L, -1)) {
    value = luaL_checkstring(L, -1);
  }
  lua_pop(L, 1);
}

template <typename T>
static void ReadVec3(lua_State * L, detail::tvec3<T> & vec, const char * name)
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
static void ReadMFVec3(lua_State * L, vector<detail::tvec3<T>> & values, const char * name)
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

template <typename ChildType>
static void ReadChildren(lua_State * L, vector<shared_ptr<ChildType>> & children)
{
  luaL_checktype(L, -1, LUA_TTABLE);
  int n = luaL_len(L, -1);

  for (int i = 1; i <= n; i++) {
    lua_rawgeti(L, -1, i);
    luaL_checktype(L, -1, LUA_TLIGHTUSERDATA);
    ChildType * child = static_cast<ChildType *>(lua_touserdata(L, -1));
    children.push_back(shared_ptr<ChildType>(child));
    lua_pop(L, 1);
  }
}

void
SFString::read(lua_State * L)
{
  ReadString(L, this->value, this->name.c_str());
}

void
SFVec3f::read(lua_State * L)
{
  ReadVec3<float>(L, this->value, this->name.c_str());
}

void
SFUint32::read(lua_State * L)
{
  ReadNumber<unsigned int>(L, this->value, this->name.c_str());
}

void
SFFloat::read(lua_State * L)
{
  ReadNumber<float>(L, this->value, this->name.c_str());
}

void
MFVec3f::read(lua_State * L)
{
  ReadMFVec3<float>(L, this->vec, this->name.c_str());
}

void
MFVec3i::read(lua_State * L)
{
  ReadMFVec3<int>(L, this->vec, this->name.c_str());
}

void
SFNode::read(lua_State * L)
{
  luaL_checktype(L, -1, LUA_TTABLE);
  int n = luaL_len(L, -1);
  assert(n == 1);
  lua_rawgeti(L, -1, 1);
  if (!lua_isnil(L, -1)) {
    luaL_checktype(L, -1, LUA_TLIGHTUSERDATA);
    Node * node = static_cast<Node *>(lua_touserdata(L, -1));
    this->value.reset(node);
  }
  lua_pop(L, 1);
}

void
MFNode::read(lua_State * L)
{
  ReadChildren<Node>(L, this->values);
}

void
SFEnum::read(lua_State * L)
{
  string name;
  ReadString(L, name, "mode");
  if (name.empty()) return;
  
  if (this->enums.find(name) == this->enums.end()) {
    Innovator::postError("Invalid Enum: " + name);
    return;
  }
  this->value = this->enums[name];
}
