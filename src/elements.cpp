#include <elements.h>
#include <state.h>
#include <nodes.h>
#include <opengl.h>
#include <iostream>
#include <glm/gtc/type_ptr.hpp>

using namespace glm;
using namespace std;

TextureElement::TextureElement()
  : unit(0)
{
}

TextureElement::~TextureElement()
{
}

void
TextureElement::set(Texture * texture)
{

}

void
TextureElement::set(TextureUnit * texunit)
{
}

void
TextureElement::set(TextureSampler * sampler)
{
}