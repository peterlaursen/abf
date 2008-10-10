#include <libdaisy.h>
#include <audiere.h>
#include <iostream>
#pragma comment(lib, "audiere.lib")
#pragma comment(lib, "libdaisy.lib")
using namespace ABF;
using namespace std;
using namespace audiere;
void Decode(Daisy&);
int main(int argc, char* argv[]) {
Daisy D(argv[1]);
if (!D.IsValid()) {
cout << "Error, not a valid daisy book." << endl;
return 1;
}
while (D.OpenSmil()) Decode(D);
return 0;
}
void Decode(Daisy& D) {
SampleSourcePtr Source = OpenSampleSource(D.GetMP3FileName());
short Buffer[4096];
while (1) {
int FramesRead = Source->read(4096, Buffer);
if (FramesRead < 4096) break;
}
return;
}
