// This tests the latest functions added to the library.
#include "libdaisy.h"
#include <iostream>
#include <string>
#include <vector>
#pragma comment(lib, "libdaisy.lib")
using namespace std;
using namespace ABF;
int main(int argc, char* argv[]) {
Daisy D(argv[1]);
if (!D.IsValid()) {
cout << "Error, exiting..." << endl;
return 1;
}
cout << "This book contains " << D.GetNumSections() << " sections." << endl;
}