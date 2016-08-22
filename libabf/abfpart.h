/* $Id$
Copyright (C) 2016 Peter Laursen.

This class contains an ABF part (e.g. a section).
It contains various flags and other housekeeping information for the ABF encoder.
*/
#ifndef ABFPART_H
#define ABFPART_H
#include <iosfwd>
#include <iostream>
#include <fstream>
#include <string>
namespace ABFMulti {
using std::ofstream;
using std::string;
enum AbfPartStatus {
Waiting,
Encoding,
Finished,
};
class AbfPart {
ofstream PartFile;
unsigned int Sequence; // number of this part.
AbfPartStatus Status = Waiting;
public:
AbfPart(unsigned int Sequence, const string FileName): Sequence(Sequence), PartFile(FileName), Status(Waiting) {}
~AbfPart();
AbfPartStatus GetStatus() const { return Status; }
unsigned int GetPartNumber() const { return Sequence; }
};
}
#endif

