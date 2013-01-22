
#include <glm/glm.hpp>

#include <nodes.h>
#include <viewer.h>

using namespace std;
using namespace glm;

int main(int argc, char * argv[])
{

  { // scope with current GL context

    shared_ptr<Separator> root(new Separator);
/*
    shared_ptr<Shader> shader(new Shader);
    shader->fileName = "../../src/program.glsl";

    shared_ptr<Triangles> triangles(new Triangles);
    triangles->vertices.push_back(vec3(-1, -1,  0));
    triangles->vertices.push_back(vec3( 1, -1,  0));
    triangles->vertices.push_back(vec3( 0,  1,  0));

    root->addChild(shader);
    {
      shared_ptr<Separator> sep(new Separator);
      shared_ptr<Transform> transform(new Transform);
      transform->translation = vec3(1, 1, 0);
      sep->addChild(transform);
      sep->addChild(triangles);
      root->addChild(sep);
    }

    {
      shared_ptr<Separator> sep(new Separator);
      shared_ptr<Transform> transform(new Transform);
      transform->translation = vec3(-1, -1, 0);
      sep->addChild(transform);
      sep->addChild(triangles);
      root->addChild(sep);
    }
*/
    shared_ptr<InstancedTriangleSet> instances(new InstancedTriangleSet);
    instances->vertices.push_back(vec3(-1, -1, -1));
    instances->vertices.push_back(vec3(-1, -1,  1));
    instances->vertices.push_back(vec3(-1,  1, -1));
    instances->vertices.push_back(vec3(-1,  1,  1));
    instances->vertices.push_back(vec3( 1, -1, -1));
    instances->vertices.push_back(vec3( 1, -1,  1));
    instances->vertices.push_back(vec3( 1,  1, -1));
    instances->vertices.push_back(vec3( 1,  1,  1));

    instances->instances.push_back(vec3(0, 0, 0));
    instances->instances.push_back(vec3(0, 0, 3));
    instances->instances.push_back(vec3(0, 3, 0));
    instances->instances.push_back(vec3(0, 3, 3));
    instances->instances.push_back(vec3(3, 0, 0));
    instances->instances.push_back(vec3(3, 0, 3));
    instances->instances.push_back(vec3(3, 3, 0));
    instances->instances.push_back(vec3(3, 3, 3));

    unsigned int indices[36] = { 0, 1, 3, 3, 2, 0,
                                 1, 5, 7, 7, 3, 1,
                                 5, 4, 6, 6, 7, 5,
                                 4, 0, 2, 2, 6, 4,
                                 2, 3, 7, 7, 6, 2,
                                 1, 0, 4, 4, 5, 1 };
                                 
    for (size_t i = 0; i < 36; i++) {
      instances->indices.push_back(indices[i]);
    }

    shared_ptr<Transform> transform(new Transform);
    root->addChild(transform);
    root->addChild(instances);

    unique_ptr<Viewer> viewer(new Viewer(640, 480));
    viewer->setSceneGraph(root);
    viewer->loop();
  }
  exit(1);
}

