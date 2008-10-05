/* LIBDECODER.H, Version 1.0.
Copyright (C) 2008 Peter Laursen.

This file contains a preliminary interface to a decoder of Daisy 2.02 books.
*/
#ifndef LIBDECODER_H
#define LIBDECODER_H
#include <cstdio>
#include <libdaisy.h>
#include <audiere.h>
namespace ABF {
class Decoder {
Daisy& _Daisy;
FILE* _Output;
audiere::SampleSourcePtr _Source;
bool _Open;
public:
// Assumption: Daisy book is initialized and ready to be decoded.
Decoder(Daisy& Book, const char* Filename);
// We close the _Output file.
~Decoder() { fclose(_Output); }
// This function will decode (and, if possible) store the audio book as audio. Note: It requires a lot of space. In the future, this function will not necessarily store this to a file.
bool DecodeBook();
// This function will decode part of the audio book and return it to you in an array for parsing to, say, speex_encode. It is noted that the array may be larger than the Speex framesize.
// Parameters: short* array, int& size, int& FramesDecoded, bool& SectionEnd.
void DecodeSection(short* Output, int& _Size, int& FramesDecoded, bool& SectionEnd);
};
}
#endif
