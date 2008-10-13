// This tests a decoder of the audio. Later, we'll test the other things.
#include <speex/speex.h>
#include <cstdio>
using namespace std;
int main() {
FILE* fin = fopen("output.raw", "rb");
FILE* fout = fopen("output.decoded", "wb");
SpeexBits Bits;
void* Decoder;
speex_bits_init(&Bits);
Decoder = speex_decoder_init(&speex_wb_mode);
int FrameSize;
speex_decoder_ctl(Decoder, SPEEX_GET_FRAME_SIZE, &FrameSize);
short* Wave = new short[FrameSize];
char Input[320];
unsigned short BytesToRead;
for (int i = 0; i < 16384; i++) {
// Get the size of the bits
fread(&BytesToRead, sizeof(short), 1, fin);
fread(Input, 1, BytesToRead, fin);
speex_bits_read_from(&Bits, Input, BytesToRead);
speex_decode_int(Decoder, &Bits, Wave);
fwrite(Wave, sizeof(short), FrameSize, fout);
}
speex_bits_destroy(&Bits);
speex_decoder_destroy(Decoder);
fclose(fin);
fclose(fout);
}
