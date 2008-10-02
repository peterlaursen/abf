/* LIBDECODER.H, Version 1.0.
Copyright (C) 2008 Peter Laursen.

This file contains a preliminary interface to a decoder of Daisy 2.02 books.
*/
#ifndef LIBDECODER_H
#define LIBDECODER_H
#include <cstdio>
#include <libdaisy.h>
namespace ABF {
class Decoder {
Daisy& _Daisy;
FILE* _Output;
bool _Open;
public:
// Assumption: Daisy book is initialized and ready to be decoded.
Decoder(Daisy& Book, const char* Filename);
// We close the _Output file.
~Decoder() { fclose(_Output); }
// This function will decode (and, if possible) store the audio book as audio. Note: It requires a lot of space. In the future, this function will not necessarily store this to a file.
bool Decode();
// Let's say this is enough for now.
};
}
#endif
