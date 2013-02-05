
#include <iostream>
#include <nodes.h>
#include <viewer.h>
#include <innovator.h>
#include <file.h>

using namespace std;

int main(int argc, char * argv[])
{
  Innovator innovator; // initialize file, lua

  shared_ptr<Separator> test = File::readAll("../../src/scene.lua");

  Viewer viewer(640, 480);
  viewer.setSceneGraph(test);
  viewer.loop();

  cout << "press Return to exit" << endl;
  cin.get();

  exit(1);
}

