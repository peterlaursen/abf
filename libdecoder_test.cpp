#include "libdecoder.h"
using namespace ABF;
int main(int argc, char* argv[]) {
Daisy D(argv[1]);
Decoder Dec(D, "output.raw");
Dec.Decode();
}
