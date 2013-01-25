
#include <glm/glm.hpp>
#include <iostream>
#include <nodes.h>
#include <viewer.h>
#include <file.h>

using namespace std;
using namespace glm;

shared_ptr<Node> getInstances()
{
  shared_ptr<InstancedTriangleSet> triangleset(new InstancedTriangleSet);
  triangleset->vertices.push_back(vec3(-1, -1, -1));
  triangleset->vertices.push_back(vec3(-1, -1,  1));
  triangleset->vertices.push_back(vec3(-1,  1, -1));
  triangleset->vertices.push_back(vec3(-1,  1,  1));
  triangleset->vertices.push_back(vec3( 1, -1, -1));
  triangleset->vertices.push_back(vec3( 1, -1,  1));
  triangleset->vertices.push_back(vec3( 1,  1, -1));
  triangleset->vertices.push_back(vec3( 1,  1,  1));
  
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

  triangleset->instances.push_back(vec3(0, 0, 0));
/*  triangleset->instances.push_back(vec3(0, 0, 3));
  triangleset->instances.push_back(vec3(0, 3, 0));
  triangleset->instances.push_back(vec3(0, 3, 3));
  triangleset->instances.push_back(vec3(3, 0, 0));
  triangleset->instances.push_back(vec3(3, 0, 3));
  triangleset->instances.push_back(vec3(3, 3, 0));
  triangleset->instances.push_back(vec3(3, 3, 3));
  */
  return triangleset;
}

shared_ptr<Node> getSphere()
{
	shared_ptr<InstancedTriangleSet> triangleset(new InstancedTriangleSet);

	float t = (1.0f + sqrtf(5.0f)) / 2.0f; // golden ratio
	triangleset->vertices.push_back(vec3(-1,  0,  t));
	triangleset->vertices.push_back(vec3( 1,  0,  t));
	triangleset->vertices.push_back(vec3(-1,  0, -t));
	triangleset->vertices.push_back(vec3( 1,  0, -t));
	triangleset->vertices.push_back(vec3( 0,  t,  1));
	triangleset->vertices.push_back(vec3( 0,  t, -1));
	triangleset->vertices.push_back(vec3( 0, -t,  1));
	triangleset->vertices.push_back(vec3( 0, -t, -1));
	triangleset->vertices.push_back(vec3( t,  1,  0));
	triangleset->vertices.push_back(vec3(-t,  1,  0));
	triangleset->vertices.push_back(vec3( t, -1,  0));
	triangleset->vertices.push_back(vec3(-t, -1,  0));

	triangleset->indices.push_back(ivec3(1, 4, 0));
	triangleset->indices.push_back(ivec3(4, 9, 0));
	triangleset->indices.push_back(ivec3(4, 5, 9));
	triangleset->indices.push_back(ivec3(8, 5, 4));
	triangleset->indices.push_back(ivec3(1, 8, 4));

	triangleset->indices.push_back(ivec3(1, 10, 8));
	triangleset->indices.push_back(ivec3(10, 3, 8));
	triangleset->indices.push_back(ivec3(8,  3, 5));
	triangleset->indices.push_back(ivec3(3,  2, 5));
	triangleset->indices.push_back(ivec3(3,  7, 2));

	triangleset->indices.push_back(ivec3(3, 10, 7));
	triangleset->indices.push_back(ivec3(10, 6, 7));
	triangleset->indices.push_back(ivec3(6, 11, 7));
	triangleset->indices.push_back(ivec3(6, 0, 11));
	triangleset->indices.push_back(ivec3(6,  1, 0));

	triangleset->indices.push_back(ivec3(10, 1, 6));
	triangleset->indices.push_back(ivec3(11, 0, 9));
	triangleset->indices.push_back(ivec3(2, 11, 9));
	triangleset->indices.push_back(ivec3(5, 2,  9));
	triangleset->indices.push_back(ivec3(11, 2, 7));

	triangleset->instances.push_back(vec3(0, 0, 0));
	return triangleset;
}

shared_ptr<Separator> openFile(const std::string & filename)
{

    shared_ptr<Separator> root(new Separator);
    return root;
}

int main(int argc, char * argv[])
{

  File file;
  shared_ptr<Separator> root = file.readAll("../../src/scene.lua");
  
  //shared_ptr<Separator> root(new Separator);
  //root->addChild(getSphere());
  
  //unique_ptr<Viewer> viewer(new Viewer(640, 480));
  //viewer->setSceneGraph(root);
  //viewer->loop();

  cout << "press Return to exit" << endl;
  cin.get();

  exit(1);
}

