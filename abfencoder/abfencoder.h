/* $Id$
Copyright (C) 2008, 2009, 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019  Peter Laursen.

This is our interface to our ABF library. Currently, we focus on upgrading our format slightly, so our player and other things may catch up as time progresses.
For all the previous work on ABF, see /branches/libabf-1.0.
That library is no longer updated and this library is incompatible with it.
The API is fairly self-explanatory.

Additional notes:
We remove the ABFDecoder from this library.
This API differs slightly from the original ABF API.
We do this so that it will become easier for the converter to start doing multithreading.
*/

#ifndef ABFENCODER_H
#define ABFENCODER_H
#include <cstdio>
#include <string>
#include <vector>
#include <mutex>
#include <opus/opus.h>
#include <cstdlib>
#include "abfsection.h"
namespace ABF {
using std::FILE;
using std::mutex;
using std::string;
using std::vector;
class AbfEncoder {
FILE* fout = nullptr; // Main ABF Audio Book
int* fds = nullptr; // File descriptors for sections
unsigned short HeaderSize = 0;
string _Title;
string _Author;
unsigned short SamplingRate;
unsigned short _NumSections = 0;
unsigned short NumMinutes = 0;
int IndexTableStartPosition = 0;
int FramesEncoded = 0;
vector<int> MinutePositions;
AbfSection* AbfSections = nullptr; // Our ABFSections
mutex mtx; // Mutex we use.
bool Initialized = false;
public:
AbfEncoder(const char* Filename, unsigned short NumSections, unsigned short SamplingRate = 16000, bool AbfInit = true);
~AbfEncoder();
void Initialize(const char*);
void SetTitle(const char* Title);
void SetAuthor(const char* Author);
const unsigned short GetSamplingRate() const;
void WriteHeader();
void Encode(unsigned short Section, const short* Input, int& Length);
void CloseSection(unsigned short Section) { AbfSections[Section].Status = Finished; AbfSections[Section].Close(); }
void SetStatus(unsigned short& Section, EncodingStatus NewStatus) { AbfSections[Section].Status = NewStatus; }
const unsigned short GetNumSections() const { return _NumSections; }
EncodingStatus GetStatus(unsigned short& Section) { return AbfSections[Section].Status; }
void Gather();
void Lock(); // Lock mutex
void Unlock(); // Unlock mutex
void Cleanup(); // Clean up our ABFSection parts.
};
}
#endif
