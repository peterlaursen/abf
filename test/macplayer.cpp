#include <iostream>
#include <cstdlib>
#include <libabf.h>
#include <unistd.h>
#include <AudioToolbox/AudioToolbox.h>
#include "compat.h"
using namespace std;
using namespace ABF;
AbfDecoder* AD;

void ABFCallback(void* UserData, AudioQueueRef AQ, AudioQueueBufferRef AB) {
cout << "Inside AudioQueue Callback." << endl << "Capacity: " << AB->mAudioDataBytesCapacity << endl;

AD->Decode((short*)AB->mAudioData);
AB->mAudioDataByteSize=640;
AudioQueueEnqueueBuffer(AQ, AB, 0, NULL);

}
void PrintStatus(int Status) {
cout << "Status = " << Status << endl;
}

int main(int argc, char* argv[]) {
if (argc != 2) {
cerr << "Error, no ABF audio book specified." << endl;
return -1;
}
AD = new AbfDecoder(argv[1]);
if (!AD->IsValid()) {
cerr << "Error, not a valid ABF Audio Book." << endl;
delete AD;
return -1;
}

AudioStreamBasicDescription ADesc;
FillOutASBDForLPCM(ADesc, 16000, 1, 16,16, false, false);
AudioQueueRef OutQueue;
int Status;
Status = AudioQueueNewOutput(&ADesc, ABFCallback, 0, 0, 0, 0, &OutQueue);
PrintStatus(Status);
AudioQueueBufferRef Buffer;
Status = AudioQueueAllocateBuffer(OutQueue, 640, &Buffer);
PrintStatus(Status);
AD->Decode((short*)Buffer->mAudioData);
Buffer->mAudioDataBytesSize = 640;
Status = AudioQueuePrime(OutQueue, 0, &Prepared);
PrintStatus(Status);
Status=AudioQueueEnqueueBuffer(OutQueue, Buffer, 0, 0);
PrintStatus(Status);
unsigned int Prepared; 

cout << "Prepared: " << Prepared << endl;
Status = AudioQueueStart(OutQueue, NULL);
PrintStatus(Status);

sleep(10);
delete AD;
return 0;
}

