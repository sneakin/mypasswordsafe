#include <iostream>
#include "safe.hpp"

using namespace std;
//using namepsace semanticgap::myps;

int main(int argc, char *argv[])
{
  Safe safe;
  string path;
  string filter;
  string password;
  Safe::Error error_code = Safe::Failed;

  cout << "File? ";
  getline(cin, path, '\n');
  cout << SafeSerializer::getTypes();
  cout << "Filter?" << endl;
  getline(cin, filter, '\n');
  cout << "Password? ";
  getline(cin, password, '\n');

  error_code = safe.load(path.c_str(), filter.c_str(), EncryptedString(password), "nolan");
  if(error_code != Safe::Success) {
    cout << "Error: " << error_code << endl;
    return 0;
  }

  cout << endl << "Safe opened" << endl;
  // print entries

  return 0;
}
