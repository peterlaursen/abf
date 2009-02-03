#include <libabf.h>
#include <iostream>
#include <windows.h>
#include <process.h>
using namespace std;
using ABF::AbfDecoder;
void Decode(void* File) {
AbfDecoder AD((char*)File);
AD.Validate();
AD.ReadHeader();
short Samples[320];
FILE* fout = fopen("samples.raw", "wb");
for (int i = 0; i < 128000; i++) {
AD.Decode(Samples);
fwrite(Samples, 2, 320, fout);
}
fclose(fout);
}
int main(int argc, char* argv[]) {
if (argc != 2) {
cout << "You gave me no book to play." << endl;
return 1;
}
HANDLE Thread = (HANDLE)_beginthread(Decode, 0, argv[1]);
WaitForSingleObject(Thread, INFINITE);
return 0;
}
