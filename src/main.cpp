
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
  shared_ptr<Separator> root = file.readAll("../../src/scene.lua");
  
  unique_ptr<Viewer> viewer(new Viewer(640, 480));
  viewer->setSceneGraph(root);
  viewer->loop();

  cout << "press Return to exit" << endl;
  cin.get();

  exit(1);
}

