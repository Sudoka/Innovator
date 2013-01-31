
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
  triangles->indices.push_back(ivec3(0, 1, 3));
  triangles->indices.push_back(ivec3(3, 2, 0));
  triangles->indices.push_back(ivec3(1, 5, 7));
  triangles->indices.push_back(ivec3(7, 3, 1));
  triangles->indices.push_back(ivec3(5, 4, 6));
  triangles->indices.push_back(ivec3(6, 7, 5));
  triangles->indices.push_back(ivec3(4, 0, 2));
  triangles->indices.push_back(ivec3(2, 6, 4));
  triangles->indices.push_back(ivec3(2, 3, 7));
  triangles->indices.push_back(ivec3(7, 6, 2));
  triangles->indices.push_back(ivec3(1, 0, 4));
  triangles->indices.push_back(ivec3(4, 5, 1));

  triangles->vertices.push_back(vec3(0, 0, 0));
  triangles->vertices.push_back(vec3(0, 0, 1));
  triangles->vertices.push_back(vec3(0, 1, 0));
  triangles->vertices.push_back(vec3(0, 1, 1));
  triangles->vertices.push_back(vec3(1, 0, 0));
  triangles->vertices.push_back(vec3(1, 0, 1));
  triangles->vertices.push_back(vec3(1, 1, 0));
  triangles->vertices.push_back(vec3(1, 1, 1));


  shared_ptr<Buffer> buffer(new Buffer);
  
  buffer->values.push_back(vec3(0, 0, 0));
  buffer->values.push_back(vec3(0, 0, 1));
  buffer->values.push_back(vec3(0, 1, 0));
  buffer->values.push_back(vec3(0, 1, 1));
  buffer->values.push_back(vec3(1, 0, 0));
  buffer->values.push_back(vec3(1, 0, 1));
  buffer->values.push_back(vec3(1, 1, 0));
  buffer->values.push_back(vec3(1, 1, 1));

  shared_ptr<Shader> program(new Shader);
  program->fileName = "program.lua";

  root->addChild(program);
  root->addChild(buffer);
  root->addChild(triangles);
  
  unique_ptr<Viewer> viewer(new Viewer(640, 480));
  viewer->setSceneGraph(root);
  viewer->loop();

  cout << "press Return to exit" << endl;
  cin.get();

  exit(1);
}

