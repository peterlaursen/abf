#ifdef WIN32
#include <windows.h>
#include <conio.h>
#endif
#include <libabf.h>
#include <libdaisy20.h>
#include <audiere.h>
#include <speex/speex_resampler.h>
#include <speex/speex.h>
#include <iostream>
#include <cstdio> // for FILE
#include <cstdlib>
#pragma comment(lib, "audiere.lib")
#pragma comment(lib, "libdaisy.lib")
#pragma comment(lib, "libspeexdsp.lib")
#pragma comment(lib, "libspeex.lib")
#pragma comment(lib, "libabf.lib")
using namespace ABF;
using namespace std;
using namespace audiere;
int main(int argc, char* argv[]) {
if (argc != 3) {
cerr << "Usage: " << argv[0] << " <path to daisy book> <output file>" << endl;
return 1;
}
DaisyBook D(argv[1]);
if (!D.BookIsValid()) {
cerr << "Error, not a valid daisy book." << endl;
return 1;
}
AbfEncoder AE(argv[2]);
D.GetMetadata();
D.GetAudioFiles();

AE.SetTitle(D.GetTitle().c_str());
AE.SetAuthor(D.GetAuthor().c_str());
AE.SetTime(D.GetTotalTime().c_str());
AE.SetNumSections(D.GetNumSections());
AE.WriteHeader();
unsigned short File = 0;
string Filename;
while (File < D.GetNumSections()) {
AE.WriteSection();
Filename = D.GetSectionFile(File);
++File;
cout << "Converting file " << File << " of " << D.GetNumSections() << endl;
AE.WriteSection();

char* TempFile = DecodeToRawAudio(Filename.c_str(), AE);
if (!TempFile) {
cout << "Error, no tempfile returned." << endl;
}

//EncodeABF(AE, TempFile);
}
cout << "The book has been converted successfully." << endl;
return 0;
}
