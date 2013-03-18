
#include <iostream>
#include <exception>
#include <innovator.h>

using namespace std;

int main(int argc, char * argv[])
{
  if (argc < 2) {
    cout << "No input file." << endl;
  } else {
    try {
      Innovator app(640, 480, argv[1]);
      app.loop();
    } catch (const exception & e) {
      cout << e.what() << endl;
    }
  }
  cout << "press Return to exit" << std::endl;
  cin.get();
  exit(1);
}

