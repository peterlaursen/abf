#include "libdecoder.h"
#include <iostream>
using namespace ABF;
using namespace std;

int main(int argc, char* argv[]) {
Daisy D(argv[1]);
Decoder Dec(D, "output.raw");
short Buffer[4096];
int Size = 4096, FramesDecoded;
Dec.DecodeSection(Buffer, Size, FramesDecoded);
cout << "Decoded " << FramesDecoded << " frames." << endl;
}
