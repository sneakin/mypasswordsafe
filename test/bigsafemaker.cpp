#include <iostream>
#include "safe.hpp"

using namespace std;

int main(int argc, char *argv[])
{
	Safe safe;
	safe.setPassPhrase("hello");

	int count = 0;
	cout << "How big? ";
	cin >> count;

	SafeItem item("Name", "User", "Password", "Notes\nGo\nHere");
	int i;
	for(i = 0; i != count; i++) {
		if(safe.addItem(item) == NULL) {
			cout << "No more memory" << endl;
			break;
		}
	}

	if(safe.save("bigsafe.dat", "hello", "nolan")) {
		cout << "Created bigsafe.dat with "
			<< i << " entries." << endl;
	}

	return 0;
}
