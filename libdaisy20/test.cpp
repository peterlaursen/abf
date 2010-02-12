#include <iostream>
#include <string>
#include "libdaisy20.h"
#pragma comment(lib, "libdaisy20.lib")
using namespace std;
using namespace ABF;
int main(int argc, char* argv[]) {
if (argc != 2) {
cerr << "Error, no Daisy book specified." << endl;
return 1;
}
DaisyBook DB(argv[1]);
DB.GetMetadata();
cout << "Title: " << DB.GetTitle() << endl << "Author: " << DB.GetAuthor() << endl << "This book consists of " << DB.GetVolumes() << " CDs." << endl;

return 0;
}

