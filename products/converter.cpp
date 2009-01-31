#ifdef WIN32
#include <windows.h>
#include <conio.h>
#endif
#include <libabf.h>
#include <libdaisy.h>
#include <audiere.h>
#include <speex/speex_resampler.h>
#include <speex/speex.h>
#include <iostream>
#include <cstdio> // for FILE
#include <cstdlib>
#pragma comment(lib, "audiere.lib")
#pragma comment(lib, "libdaisy.lib")
#pragma comment(lib, "libspeexdsp_imp.lib")
#pragma comment(lib, "libspeex_static.lib")
#pragma comment(lib, "libabf.lib")
using namespace ABF;
using namespace std;
using namespace audiere;
int main(int argc, char* argv[]) {
if (argc != 3) {
cerr << "Usage: " << argv[0] << " <path to daisy book> <output file>" << endl;
return 1;
}
Daisy D(argv[1]);
if (!D.IsValid()) {
cerr << "Error, not a valid daisy book." << endl;
return 1;
}
AbfEncoder AE(argv[2]);
string Meta = "dc:title";
string StrTitle = D.ExtractMetaInfo(Meta);
AE.SetTitle(StrTitle.c_str());
Meta = "dc:creator";
string StrAuthor = D.ExtractMetaInfo(Meta);
AE.SetAuthor(StrAuthor.c_str());
Meta = "ncc:totalTime";
string StrTime = D.ExtractMetaInfo(Meta);
AE.SetTime(StrTime.c_str());
AE.SetNumSections(D.GetNumSections());
AE.WriteHeader();
unsigned short File = 0;
string Filename;
while (D.OpenSmil()) {
AE.WriteSection();
++File;
cout << "Converting file " << File << " of " << D.GetNumSections() << endl;
char* TempFile = DecodeToRawAudio(D.GetMP3FileName());
EncodeABF(AE, TempFile);
}
cout << "The book has been converted successfully." << endl;
return 0;
}
