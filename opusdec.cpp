/* We try to play  a very simple file.
*/
#include <stdio.h>
#include <stdlib.h>
#include <opus.h>
#include <sys/soundcard.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char* argv[]) {
if (argc != 2) {
printf("Error, need at least an input file name.");
return (EXIT_FAILURE);
}
printf("Accepted.\n");
// Let's try to create an encoder.
int Error = 0;
OpusDecoder* Decoder = opus_decoder_create(16000, 1, &Error);
FILE* fin = fopen(argv[1], "rb");
short Buffer[320] = {0};
int fout = open("/dev/dsp", O_WRONLY);
// Try and set up the soundcard.
int Format = AFMT_S16_NE;
ioctl(fout, SNDCTL_DSP_SETFMT, &Format);
Format = 1;
ioctl(fout, SNDCTL_DSP_CHANNELS, &Format);
Format = 16000;
ioctl(fout, SNDCTL_DSP_SPEED, &Format);
while (!feof(fin)) {
unsigned char InputBuffer[2048];
unsigned short Length = 0;
fread(&Length,1,2,fin);
fread(InputBuffer, 1, Length, fin);
int Samples = opus_decode(Decoder, InputBuffer, Length, Buffer, 320, 0);
printf("Decoded %d samples.\n", Samples);
write(fout, Buffer, Samples*2);

}
close(fout);
fclose(fin);
opus_decoder_destroy(Decoder); 

}

