
#include <iostream>
#include <nodes.h>
#include <viewer.h>
#include <innovator.h>
#include <file.h>

using namespace std;

static unique_ptr<Viewer> viewer = nullptr;

static void loadSceneGraph()
{
  shared_ptr<Separator> root = File::readAll("../../src/scene.lua");
  if (root.get()) {
    viewer->setSceneGraph(root);
  }
}

int main(int argc, char * argv[])
{
  Innovator innovator; // initialize file, lua

  viewer.reset(new Viewer(640, 480));
  loadSceneGraph();
  viewer->loop();

  cout << "press Return to exit" << endl;
  cin.get();

  exit(1);
}

