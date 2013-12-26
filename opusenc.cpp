/* We try to encode a very simple file. Afterwards, we try to decode the same file.
*/
#include <cstdio>
#include <cstdlib>
#include <sys/stat.h>
#include <opus/opus.h>
int main(int argc, char* argv[]) {
if (argc != 2) {
printf("Error, need at least an input file name.");
return (EXIT_FAILURE);
}
printf("Accepted.\n");
// Let's try to create an encoder.
int Error = 0;
OpusEncoder* Encoder = opus_encoder_create(16000, 1, OPUS_APPLICATION_VOIP, &Error);
// Get a little information about our encoder
int Bitrate = 1;
opus_encoder_ctl(Encoder, OPUS_SET_COMPLEXITY(&Bitrate));

struct stat fileinfo;
stat(argv[1], &fileinfo);

FILE* fin = fopen(argv[1], "rb");
printf("File size of input file: %lld\n", fileinfo.st_size);
unsigned char* FileContents = new unsigned char[fileinfo.st_size];
fread(FileContents, 1, fileinfo.st_size, fin);
fclose(fin);
fin = fmemopen(FileContents, fileinfo.st_size, "rb");
short Buffer[320] = {0};
FILE* fout = fopen("output.opus", "wb");
while (!feof(fin)) {
unsigned char OutputBuffer[2048];
int Samples = fread(Buffer, sizeof(short), 320, fin);
unsigned short Length = opus_encode(Encoder, Buffer, Samples, OutputBuffer, 2048);

if (Length > 100) break;
//printf("First frame: %d bytes\n", Length);
fwrite(&Length,1,sizeof(unsigned short),fout);
fwrite(OutputBuffer, 1, Length, fout);
}
fclose(fout);
fclose(fin);
printf("Our bitrate is %d\n", Bitrate);
opus_encoder_destroy(Encoder); 
delete [] FileContents;
FileContents = 0;
}

