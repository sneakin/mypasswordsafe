#include <iostream>
#include "smartptr.hpp"

int main(int argc, char *argv[])
{
  SmartPtr<int> a(new int(3));
  SmartPtr<int> b(a);
  SmartPtr<int> c(new int(5));

  b.set(c);
  a.set(c);

  return 0;
}
