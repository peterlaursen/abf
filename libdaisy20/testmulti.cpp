#include <iostream>
#include <string>
#include "libdaisy20.h"
#pragma comment(lib, "libdaisy20.lib")
using namespace std;
using namespace ABF;
int main(int argc, char* argv[]) {
if (argc < 2) {
cout << "Error, not enough arguments provided." << endl;
return 1;
}
DaisyBook DB(argv[1]);
DB.GetMetadata();
if (DB.GetVolumes() == 1) {
cout << "Error, this is a single volume daisy book. This tests multi-volume books." << endl;
return 1;
}
string Path;
if (argc == 2) {
cout << "Type in the second path." << endl;
getline(cin, Path);
DaisyBook NDB((char*)Path.c_str());
NDB.GetMetadata();
if (DB.NextVolume((char*)NDB.GetPath().c_str())) cout << endl << "This is the second volume of the book." << endl;
	
}
return 0;
}

