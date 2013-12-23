/* We try to encode a very simple file. Afterwards, we try to decode the same file.
*/
#include <stdio.h>
#include <stdlib.h>
#include <opus.h>
int main(int argc, char* argv[]) {
if (argc != 2) {
printf("Error, need at least an input file name.");
return (EXIT_FAILURE);
}
printf("Accepted.\n");
// Let's try to create an encoder.
int Error = 0;
OpusEncoder* Encoder = opus_encoder_create(16000, 1, OPUS_APPLICATION_VOIP, &Error);
FILE* fin = fopen(argv[1], "rb");
short Buffer[320] = {0};
FILE* fout = fopen("output.opus", "wb");
while (!feof(fin)) {
unsigned char OutputBuffer[2048];
int Samples = fread(Buffer, sizeof(short), 320, fin);
unsigned short Length = opus_encode(Encoder, Buffer, Samples, OutputBuffer, 2048);

if (Length > 100) break;
printf("First frame: %d bytes\n", Length);
fwrite(&Length,1,sizeof(unsigned short),fout);
fwrite(OutputBuffer, 1, Length, fout);
}
fclose(fout);
fclose(fin);
opus_encoder_destroy(Encoder); 

}

