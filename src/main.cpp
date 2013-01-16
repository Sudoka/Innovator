
#include <glm/glm.hpp>

#include <nodes.h>
#include <viewer.h>

using namespace std;
using namespace glm;

int main(int argc, char * argv[])
{

  { // scope with current GL context

    shared_ptr<Separator> root(new Separator);

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

    unique_ptr<Viewer> viewer(new Viewer(640, 480));
    viewer->setSceneGraph(root);
    viewer->loop();
  }
  exit(1);
}

