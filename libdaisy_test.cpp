#include "libdaisy.h"
#include <iostream>
using namespace std;
using namespace ABF;
int main(int argc, char* argv[]) {
if (argc != 2) {
cerr << "Error, only one argument must be passed to this program." << endl;
return 1;
}
Daisy D(argv[1], false);
if (!D.Open()) {
cout << "Something went wrong during opening." << endl;
return 1;
}
cout << D.GetMetaInfo() << endl;
string Meta("dc:date");
cout << D.ExtractMetaInfo(Meta) << endl;
cout << D.GetMP3FileName();
while (1) {
const char* Value = GetNextAudioFile(D);
if (!Value) break;
cout << Value << endl;
}
}
