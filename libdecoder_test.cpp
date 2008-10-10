#include "libdecoder.h"
#include <iostream>
using namespace ABF;
using namespace std;

int main(int argc, char* argv[]) {
Daisy D(argv[1]);
Decoder Dec(D, "output.raw");
short Buffer[4096];
int Size = 4096, FramesDecoded;
bool SectionEnd = false;
for (int i = 0; i < 10; i++) {
cout << "i = " << i << endl;
while (!SectionEnd)
Dec.DecodeSection(Buffer, Size, FramesDecoded, SectionEnd);
SectionEnd = false;
}
cout << "Decoded " << FramesDecoded << " frames." << endl;
}
