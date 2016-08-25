/* $Id$
Copyright (C) 2016 Peter Laursen.

This class contains an ABF part (e.g. a section).
It contains various flags and other housekeeping information for the ABF encoder.
*/
#ifndef ABFPART_H
#define ABFPART_H
#include <iosfwd>
#include <fstream>
#include <string>
#include <vector>
#include <opus/opus.h>

namespace ABFMulti {
using std::ofstream;
using std::string;
using std::ios_base;
using std::vector;
enum AbfPartStatus {
Waiting,
Encoding,
Finished,
};
class AbfPart {
OpusEncoder* Encoder = nullptr;
ofstream PartFile;
unsigned int Sequence; // number of this part.
AbfPartStatus Status = Waiting;
vector<unsigned int> MinutePositions;
unsigned int FramesEncoded = 0;
static unsigned int LastSequence;
public:
AbfPart(unsigned int Sequence = LastSequence);
AbfPart(const string FileName): AbfPart(LastSequence) { SetFileName(FileName); }
~AbfPart();
AbfPartStatus GetStatus() const { return Status; }
unsigned int GetPartNumber() const { return Sequence; }
void SetFileName(const string FileName);
void SetPartNumber(unsigned int Sequence);
void Write(short* Array);
};
}
#endif
