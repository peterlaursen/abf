#include <libdaisy.h>
#include <audiere.h>
#include <speex/speex_resampler.h>
#include <speex/speex.h>
#include <iostream>
#include <cstdio> // for FILE
#pragma comment(lib, "audiere.lib")
#pragma comment(lib, "libdaisy.lib")
#pragma comment(lib, "libspeexdsp_imp.lib")
#pragma comment(lib, "libspeex_static.lib")
using namespace ABF;
using namespace std;
using namespace audiere;
void Decode(Daisy&, FILE*);
void Encode(short*, unsigned int, FILE*);
int main(int argc, char* argv[]) {
Daisy D(argv[1]);
if (!D.IsValid()) {
cout << "Error, not a valid daisy book." << endl;
return 1;
}
FILE* fout = fopen("output.raw", "wb");
while (D.OpenSmil()) Decode(D, fout);
fclose(fout);
return 0;
}
void Decode(Daisy& D, FILE* fout) {
unsigned int Size = 4096, Processed = 4096;
short Resampled[4096];
SpeexResamplerState* State = speex_resampler_init(1, 22050, 16000, 3, 0);
SampleSourcePtr Source = OpenSampleSource(D.GetMP3FileName());
short Buffer[4096];
while (1) {
unsigned int FramesRead = Source->read(4096, Buffer);
if (FramesRead <= 0) break;
speex_resampler_process_int(State, 0, Buffer, &FramesRead, Resampled, &Processed);
Encode(Resampled, Processed, fout);
}
speex_resampler_destroy(State);
return;
}
void Encode(short* Input, unsigned int Size, FILE* fout) {
// Initialize Speex.
void* encoder = speex_encoder_init(&speex_wb_mode);
SpeexBits Bits;
speex_bits_init(&Bits);
int FrameSize;
speex_encoder_ctl(encoder, SPEEX_GET_FRAME_SIZE, &FrameSize);
// This is rather complicated. We have our array and we must pass this into speex_encode.
// We'll read this array in chunks.
short* Samples = new short[FrameSize];
int Position = 0, BytesToRead = 0;
char Output[400];
while (Position < Size) {
if (Position + FrameSize > Size) BytesToRead = Size - Position;
else BytesToRead = FrameSize;
for (int i = Position, j = 0; i < Position + BytesToRead; i++, j++)
Samples[j] = Input[i];
speex_bits_reset(&Bits);
speex_encode_int(encoder, Samples, &Bits);
unsigned short Bytes = speex_bits_write(&Bits, Output, 400);
fwrite(&Bytes, sizeof(short), 1, fout);
fwrite(Output, sizeof(char), Bytes, fout);
Position += BytesToRead;
}
speex_bits_destroy(&Bits);
speex_encoder_destroy(encoder);

delete[] Samples; Samples = 0;
}
