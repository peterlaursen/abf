#include <iostream>
#include <windows.h>
#include <process.h>
#include <libdaisy.h>
#include <audiere.h>
#include <conio.h>
using namespace std;
using namespace audiere;
using namespace ABF;
bool Quit = false;
char* CommandString;
void Input();
void Thread(void*) {
Daisy D(CommandString, false);
D.Validate();
if (!D.IsValid()) {
cout << "Error, this does not appear to be a Daisy book." << endl;
Quit = true;
return;
}
D.Open();
D.OpenSmil();
AudioDevicePtr Device = OpenDevice();
OutputStreamPtr Stream = OpenSound(Device, D.GetMP3FileName(), true);
Stream->play();
while (!Quit) {
if (Stream->isPlaying()) continue;
if (!D.OpenSmil()) {
Quit = true;
break;
}
Stream = OpenSound(Device, D.GetMP3FileName(), true);
Stream->play();
Sleep(300);
}
cout << "Destroying objects" << endl;
return;
}
int main(int argc, char* argv[]) {
CommandString = argv[1];
int tid = _beginthread(Thread, 0, NULL);
while (!Quit) Input();
cout << "Quitting Daisy Player." << endl;
Sleep(3000);
return 0;
}
void Input() {
char Key = getch();
if (Key == 27 || Key == 'x' || Key == 'q') Quit = true;
}
