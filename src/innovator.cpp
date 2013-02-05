#include <innovator.h>
#include <file.h>
#include <luawrapper.h>

Innovator::Innovator()
{
  Lua::init();
  File::init();
}

Innovator::~Innovator()
{
  Lua::exit();
  File::exit();
}