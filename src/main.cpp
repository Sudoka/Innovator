
#include <glm/glm.hpp>

#include <nodes.h>
#include <viewer.h>

using namespace std;
using namespace glm;

int main(int argc, char * argv[])
{
  shared_ptr<Separator> root(new Separator);

  shared_ptr<InstancedTriangleSet> triangleset(new InstancedTriangleSet);
  triangleset->vertices.push_back(vec3(-1, -1, -1));
  triangleset->vertices.push_back(vec3(-1, -1,  1));
  triangleset->vertices.push_back(vec3(-1,  1, -1));
  triangleset->vertices.push_back(vec3(-1,  1,  1));
  triangleset->vertices.push_back(vec3( 1, -1, -1));
  triangleset->vertices.push_back(vec3( 1, -1,  1));
  triangleset->vertices.push_back(vec3( 1,  1, -1));
  triangleset->vertices.push_back(vec3( 1,  1,  1));
  
  triangleset->instances.push_back(vec3(0, 0, 0));
  triangleset->instances.push_back(vec3(0, 0, 3));
  triangleset->instances.push_back(vec3(0, 3, 0));
  triangleset->instances.push_back(vec3(0, 3, 3));
  triangleset->instances.push_back(vec3(3, 0, 0));
  triangleset->instances.push_back(vec3(3, 0, 3));
  triangleset->instances.push_back(vec3(3, 3, 0));
  triangleset->instances.push_back(vec3(3, 3, 3));

  triangleset->indices.push_back(ivec3(0, 1, 3));
  triangleset->indices.push_back(ivec3(3, 2, 0));
  triangleset->indices.push_back(ivec3(1, 5, 7));
  triangleset->indices.push_back(ivec3(7, 3, 1));
  triangleset->indices.push_back(ivec3(7, 3, 1));
  triangleset->indices.push_back(ivec3(5, 4, 6));
  triangleset->indices.push_back(ivec3(6, 7, 5));
  triangleset->indices.push_back(ivec3(4, 0, 2));
  triangleset->indices.push_back(ivec3(2, 6, 4));
  triangleset->indices.push_back(ivec3(2, 3, 7));
  triangleset->indices.push_back(ivec3(7, 6, 2));
  triangleset->indices.push_back(ivec3(1, 0, 4));
  triangleset->indices.push_back(ivec3(4, 5, 1));
  
  shared_ptr<Transform> transform(new Transform);
  root->addChild(transform);
  root->addChild(triangleset);
  
  unique_ptr<Viewer> viewer(new Viewer(640, 480));
  viewer->setSceneGraph(root);
  viewer->loop();

  exit(1);
}

