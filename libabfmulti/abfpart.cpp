/* $Id$
Copyright (C) 2016 Peter Laursen.

This file contains the implementation of our various constructors and functions we do not want in our header file.
*/
#include <iostream>
#include "abfpart.h"
namespace ABFMulti {
unsigned AbfPart::LastSequence = 0;
AbfPart::AbfPart(unsigned int Sequence, const string FileName): Sequence(Sequence), PartFile(FileName, ios_base::out|ios_base::binary) {
int Error = 0;
Encoder = opus_encoder_create(16000, 1, OPUS_APPLICATION_VOIP, &Error);
++LastSequence;
}
AbfPart::~AbfPart() {
if (PartFile.is_open()) PartFile.close();
opus_encoder_destroy(Encoder);
Encoder = nullptr;
}
void AbfPart::Write(short* Array) {
unsigned char Output[200] = {0};
short Bytes = opus_encode(Encoder, Array, 320, Output, 200);
PartFile << Bytes;
PartFile.write((const char*)Output, Bytes);
}
void AbfPart::SetPartNumber(unsigned int Sequence) { Sequence = Sequence; }
void AbfPart::SetFileName(const string FileName) {
if (PartFile.is_open()) PartFile.close();
PartFile.open(FileName, ios_base::out|ios_base::binary);
}

}

