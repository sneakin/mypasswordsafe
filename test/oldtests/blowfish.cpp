#include <iostream>
#include "BlowFish.h"

using namespace std;

int main()
{
	BlowFish *f1 = new BlowFish((unsigned char *)"hello", 5);
	delete f1;
	BlowFish *f2 = new BlowFish((unsigned char *)"hello", 5);
return 0;
}
