
#include <glm/glm.hpp>
#include <iostream>
#include <nodes.h>
#include <viewer.h>
#include <file.h>

using namespace std;
using namespace glm;

int main(int argc, char * argv[])
{
  File file;
  //shared_ptr<Separator> root = file.readAll("../../src/scene.lua");
  shared_ptr<Separator> root(new Separator);
  shared_ptr<Triangles> triangles(new Triangles);

  shared_ptr<IndexBuffer> indices(new IndexBuffer);

  indices->values.push_back(ivec3(0, 1, 3));
  indices->values.push_back(ivec3(3, 2, 0));
  indices->values.push_back(ivec3(1, 5, 7));
  indices->values.push_back(ivec3(7, 3, 1));
  indices->values.push_back(ivec3(5, 4, 6));
  indices->values.push_back(ivec3(6, 7, 5));
  indices->values.push_back(ivec3(4, 0, 2));
  indices->values.push_back(ivec3(2, 6, 4));
  indices->values.push_back(ivec3(2, 3, 7));
  indices->values.push_back(ivec3(7, 6, 2));
  indices->values.push_back(ivec3(1, 0, 4));
  indices->values.push_back(ivec3(4, 5, 1));
  
  shared_ptr<VertexAttribute> buffer(new VertexAttribute);
  
  buffer->values.push_back(vec3(0, 0, 0));
  buffer->values.push_back(vec3(0, 0, 1));
  buffer->values.push_back(vec3(0, 1, 0));
  buffer->values.push_back(vec3(0, 1, 1));
  buffer->values.push_back(vec3(1, 0, 0));
  buffer->values.push_back(vec3(1, 0, 1));
  buffer->values.push_back(vec3(1, 1, 0));
  buffer->values.push_back(vec3(1, 1, 1));

  shared_ptr<Program> program(new Program);
  program->fileName = "program.lua";

  root->addChild(program);
  root->addChild(indices);
  root->addChild(buffer);
  root->addChild(triangles);
  
  unique_ptr<Viewer> viewer(new Viewer(640, 480));
  viewer->setSceneGraph(root);
  viewer->loop();

  cout << "press Return to exit" << endl;
  cin.get();

  exit(1);
}

