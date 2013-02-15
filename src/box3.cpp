#include <box3.h>

using namespace glm;
using namespace std;

box3::box3()
{
  this->makeEmpty();
}

box3::box3(const vec3 & min, const vec3 & max)
  : min(min), max(max) {}
 
box3::~box3() {}

void
box3::makeEmpty()
{
  this->min = vec3( FLT_MAX,  FLT_MAX,  FLT_MAX);
  this->max = vec3(-FLT_MAX, -FLT_MAX, -FLT_MAX);
}

void
box3::extendBy(const vec3 & v)
{  
  this->min = glm::min(v, this->min);
  this->max = glm::max(v, this->max);
}

void
box3::transform(const mat4 & mat)
{
  this->min = vec3(mat * vec4(this->min, 1.0));
  this->max = vec3(mat * vec4(this->max, 1.0));
}

float
box3::size() const
{
  return glm::length(this->span());
}

vec3
box3::span() const
{
  return this->max - this->min;
}

vec3
box3::center() const
{
  return this->min + this->span() / 2.0f;
}
