#include <fields.h>
#include <nodes.h>

#include <map>

using namespace std;
using namespace glm;

template <typename T> T * 
ReadUserData(lua_State * L)
{
  luaL_checktype(L, -1, LUA_TLIGHTUSERDATA);
  T * userdata = static_cast<T *>(lua_touserdata(L, -1));
  assert(userdata != nullptr);
  return userdata;
}

shared_ptr<Node>
FieldValue<shared_ptr<Node>>::readValue(lua_State * L)
{
  return shared_ptr<Node>(ReadUserData<Node>(L));
}

double
FieldValue<double>::readValue(lua_State * L)
{
  return luaL_checknumber(L, -1);
}

GLfloat
FieldValue<GLfloat>::readValue(lua_State * L)
{
  return (GLfloat)luaL_checknumber(L, -1);
}

GLint
FieldValue<GLint>::readValue(lua_State * L)
{
  return luaL_checkinteger(L, -1);
}

GLuint
FieldValue<GLuint>::readValue(lua_State * L)
{
  return luaL_checkinteger(L, -1);
}

string
FieldValue<string>::readValue(lua_State * L)
{
  return luaL_checkstring(L, -1);
}

