/* $Id$
Copyright (C) 2008, 2009, 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017 Peter Laursen.

This is our interface to our ABF library. Currently, we focus on upgrading our format slightly, so our player and other things may catch up as time progresses.
For all the previous work on ABF, see /branches/libabf-1.0.
That library is no longer updated and this library is incompatible with it.
Note that this library contain both our ABF Encoder and ABF Decoder.
The API is fairly self-explanatory.

Additional notes:
We remove the ABFDecoder from this test library.
We furthermore rename this libabf implementation to something else.
This API differs slightly from the original ABF API.
We do this so that it will become easier for the converter to start doing multithreading.
*/

#ifndef ABFENCODER_H
#define ABFENCODER_H
#include <cstdio>
#include <string>
#include <vector>
#include <pthread.h>
#include <opus/opus.h>
#include <cstdlib>
#include "abfsection.h"
namespace ABF {
using std::FILE;
using std::string;
using std::vector;
class AbfEncoder {
FILE* fout = nullptr; // Main ABF Audio Book
int* fds = nullptr; // File descriptors for sections
unsigned short HeaderSize = 0;
string _Title;
string _Author;
string _Time;
unsigned short _NumSections = 0;
unsigned short NumMinutes = 0;
int IndexTableStartPosition = 0;
int FramesEncoded = 0;
vector<int> MinutePositions;
void Initialize(const char*);

AbfSection* AbfSections = nullptr; // Our ABFSections
pthread_mutex_t mtx; // Mutex we use.
public:
AbfEncoder(const char* Filename, unsigned short NumSections);
~AbfEncoder();
void SetTitle(const char* Title);
void SetAuthor(const char* Author);
void SetTime(const char* Time);
void WriteHeader();
void Encode(unsigned short Section, const short* Input, int& Length);
void CloseSection(unsigned short Section) { AbfSections[Section].Status = Finished; AbfSections[Section].Close(); }
void SetStatus(unsigned short& Section, EncodingStatus NewStatus) { AbfSections[Section].Status = NewStatus; }
const unsigned short GetNumSections() const { return _NumSections; }
EncodingStatus GetStatus(unsigned short& Section) { return AbfSections[Section].Status; }
void Gather();
void Lock(); // Lock mutex
void Unlock(); // Unlock mutex
};
}
#endif
