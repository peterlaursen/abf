#include <iostream>
#include <libdaisy.h>
#include <audiere.h>
using namespace std;
using namespace audiere;
using namespace ABF;
int main(int argc, char* argv[]) {
Daisy D(argv[1], false);
D.Validate();
if (!D.IsValid()) {
cout << "Error, this does not appear to be a Daisy book." << endl;
return 1;
}
D.Open();
D.OpenSmil();
AudioDevicePtr Device = OpenDevice();
OutputStreamPtr Stream = OpenSound(Device, D.GetMP3FileName(), true);
Stream->play();
while (1) {
if (Stream->isPlaying()) continue;
D.OpenSmil();
Stream = OpenSound(Device, D.GetMP3FileName(), true);
Stream->play();
}
}
