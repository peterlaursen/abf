#include <iostream>
#include <cstdio>
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
cout << "Title: " << DB.GetTitle() << ". Author: " << DB.GetAuthor() << endl << "This book consists of " << DB.GetVolumes() << " CDs, of which this is CD number " << DB.GetCurrentVolume() << endl << "The book lasts " << DB.GetTotalTime() << endl;
DB.GetAudioFiles();
cout << "Number of sections: " << DB.GetNumSections() << endl;
if (DB.GetVolumes() > 1) {
string Path;
cout << endl << "This book consists of multiple volumes. Please type in the path." << endl;
getline(cin, Path);
if (DB.NextVolume((char*)Path.c_str())) {
DB.GetAudioFiles();
cout << endl << "Next volume located. The book now contains " << DB.GetNumSections() << " sections." << endl;
}
else {
cout << "Error, cannot locate next volume." << endl;
}

}
FILE* InputFile = fopen(DB.GetSectionFile(0), "rb");
if (!InputFile)
cout << "Could not open file." << endl;
else cout << "The file was opened successfully." << endl;
fclose(InputFile);
return 0;
}
