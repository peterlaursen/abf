/* $Id$
Copyright (C) 2009, 2010, 2011, 2012 Peter Laursen.

This is a program to convert a Daisy book into an ABF file.
It runs on Windows and FreeBSD.
*/

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
#ifndef WIN32
#include <signal.h>
#endif

#ifdef WIN32
#pragma comment(lib, "audiere.lib")
#pragma comment(lib, "libdaisy.lib")
#pragma comment(lib, "libspeexdsp.lib")
#pragma comment(lib, "libspeex.lib")
#pragma comment(lib, "libabf.lib")
#endif
using namespace ABF;
using namespace std;
using namespace audiere;
#ifndef WIN32
/*
Since we define a signal handler for our FreeBSD systems, we store our 
ABF audio book file name in this global variable.
*/
const char* AudioBookFileName = nullptr;
void Cleanup(int Signal) {
printf("Inside signal handler.\nCleaning up by removing temporary files. Removing temporary files.\n");
system("rm /tmp/ABFConv*");
unlink(AudioBookFileName);
exit(Signal);
}
#endif

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
cout << "Before obtaining metadata." << endl;
if (!D.GetMetadata()) {
cerr << "Error: Cannot obtain metadata." << endl;
return -1;
}
cout << "We have our metadata." << endl << "Before audio files." << 
endl;
D.GetAudioFiles();
cout << "After metadata." << endl;
AbfEncoder AE(argv[2]);
#ifndef WIN32
AudioBookFileName = argv[2];
signal(SIGINT, &Cleanup);
#endif
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
char* TempFile = DecodeToRawAudio(Filename.c_str());
if (!TempFile) {
cout << "Error, no tempfile returned." << endl;
}

EncodeABF(AE, TempFile);
}
cout << "The book has been converted successfully." << endl;
return 0;
}
