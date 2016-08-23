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
#include <opus/opus.h>

namespace ABFMulti {
using std::ofstream;
using std::string;
using std::ios_base;
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
public:
AbfPart(unsigned int Sequence, const string FileName);
~AbfPart();
AbfPartStatus GetStatus() const { return Status; }
unsigned int GetPartNumber() const { return Sequence; }
void Write(short* Array);
};
}
#endif

