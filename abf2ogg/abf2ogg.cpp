#ifdef WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif
#include <libabf.h>
#include <iostream>
#include <cstdio>
#include <cstdlib>
using namespace ABF;
using namespace std;
int main(int argc, char* argv[]) {
if (argc != 3) {
cerr << "Usage: " << argv[0] << " <ABF Book> <output dir>" << endl;
return 1;
}
AbfDecoder AD(argv[1]);
int* Sections = AD.GetSections();
FILE* Output = fopen("decoded.raw", "wb");
short Decoded[320];
cout << "Decoding " << argv[1] << endl;
while (!AD.feof()) {
AD.Decode(Decoded);
fwrite(Decoded, sizeof(short), 320, Output);
}
fclose(Output);
//system("oggenc -r -C 1 -R 16000 -o abftest.ogg --quiet decoded.raw");
//cout << "Finished decoding." << endl;
#ifndef WIN32
unlink("decoded.raw");
#endif
return 0;
}

