#include <iostream>
#include "safe.hpp"
#include "serializers.hpp"

int main(int argc, char *argv[])
{
	Safe *safe = new Safe;
	safe->setPassPhrase("hello");
	SafeEntry *entry = new SafeEntry(safe, "name", "user", "password", "notes");

	Safe::Error err = safe->save("hello.dat", "Password Safe 2.0 (*.dat)", "nolan");
	if(err != Safe::Success) {
		std::cout << "Error: " << err << std::endl;
		return -1;
	}

	err = safe->load("hello.dat", "Password Safe 2.0 (*.dat)", "hello", "nolan");
	if(err != Safe::Success) {
		std::cout << "Error: " << err << std::endl;
		return -2;
	}

	delete safe;

	return 0;
}
