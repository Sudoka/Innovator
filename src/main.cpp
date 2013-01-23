
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
  
  unsigned int indices[36] = { 0, 1, 3, 3, 2, 0,
                               1, 5, 7, 7, 3, 1,
                               5, 4, 6, 6, 7, 5,
                               4, 0, 2, 2, 6, 4,
                               2, 3, 7, 7, 6, 2,
                               1, 0, 4, 4, 5, 1 };
  
  for (size_t i = 0; i < 36; i++) {
    triangleset->indices.push_back(indices[i]);
  }
  
  shared_ptr<Transform> transform(new Transform);
  root->addChild(transform);
  root->addChild(triangleset);
  
  unique_ptr<Viewer> viewer(new Viewer(640, 480));
  viewer->setSceneGraph(root);
  viewer->loop();

  exit(1);
}

