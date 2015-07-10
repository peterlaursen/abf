#include "dsaudio.h"
#include <dsound.h>
#include "../libabf/libabf.h"
#include <windows.h>
#include <process.h>
#include "player.h"
#include "audiosystem.h"
#pragma comment(lib, "dsound.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "kernel32.lib")
#pragma comment(lib, "dxguid.lib")
using namespace std;
namespace ABF {
void DSAudio::SetupWindow() {
WNDCLASS Class;
Class.style = 0;
Class.lpfnWndProc    = DefWindowProc;
Class.cbClsExtra = 0;
Class.cbWndExtra = 0;
Class.hInstance = GetModuleHandle(NULL);
Class.hCursor = NULL; 
Class.hIcon = 0;
Class.hbrBackground = NULL;
Class.lpszMenuName = 0;
Class.lpszClassName = "DXHiddenWindow";
RegisterClass(&Class);
WindowHandle = CreateWindow("DXHiddenWindow", "", WS_POPUP, 0,0,0,0, 0, 0, GetModuleHandle(NULL), 0);
}
HRESULT DSAudio::CreateBasicBuffer(IDirectSound8* lpDirectSound, IDirectSoundBuffer8** ppDsb8)
{
WAVEFORMATEX wfx;
DSBUFFERDESC dsbdesc;
LPDIRECTSOUNDBUFFER pDsb = NULL;
HRESULT hr;
// Set up WAV format structure.
memset(&wfx, 0, sizeof(WAVEFORMATEX));
wfx.wFormatTag = WAVE_FORMAT_PCM;
wfx.nChannels = 1;
wfx.nSamplesPerSec = 16000;
wfx.nBlockAlign = 2;
wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;
wfx.wBitsPerSample = 16;
// Set up DSBUFFERDESC structure.
memset(&dsbdesc, 0, sizeof(DSBUFFERDESC));
dsbdesc.dwSize = sizeof(DSBUFFERDESC);
dsbdesc.dwFlags =
DSBCAPS_CTRLPAN | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLFREQUENCY
| DSBCAPS_GLOBALFOCUS|DSBCAPS_GETCURRENTPOSITION2;
dsbdesc.dwBufferBytes = 2 * wfx.nAvgBytesPerSec;
dsbdesc.lpwfxFormat = &wfx;
// Create buffer.
hr = lpDirectSound->CreateSoundBuffer(&dsbdesc, &pDsb, NULL);
if (SUCCEEDED(hr))
{
hr = pDsb->QueryInterface(IID_IDirectSoundBuffer8, (LPVOID*) ppDsb8);
pDsb->Release();
}
return hr;
}

DSAudio::DSAudio() {
AD = nullptr;
IsPlaying = false;
DirectSoundCreate8(NULL, &Device, NULL);
SetupWindow();
Device->SetCooperativeLevel(WindowHandle, DSSCL_PRIORITY);
}
void DSAudio::Play() {
	IsPlaying = true;
CreateBasicBuffer(Device, &Buffer);
bool Initialized = false;
	while (!AD->feof() && PS == Playing) {
if (PS == Playing)
DSAudio::LastPosition = AD->ftell() - 7200;
if (LastPosition < 0) LastPosition = 0;
DWORD PlayPosition = 0;
do {
if (!Initialized) break;
Buffer->GetCurrentPosition(&PlayPosition, 0);
} while (PlayPosition <= 32000);
short* DirectXBuffer;
unsigned long BufferLength;
Buffer->Lock(0, 32000, (LPVOID*)&DirectXBuffer, &BufferLength, 0, 0, 0);
short Decoded[320];
for (int i = 0; i < 16000; i+= 320) {
AD->Decode(Decoded);
for (int j = 0; j < 320; j++) DirectXBuffer[i+j] = Decoded[j];
}
Buffer->Unlock((LPVOID*)DirectXBuffer, BufferLength, 0, 0);
if (PS == Quit) {
Buffer->Stop();
break;
}
do {
if (!Initialized) break;

Buffer->GetCurrentPosition(&PlayPosition, NULL);
} while (PlayPosition >= 32000);
Buffer->Lock(32000, 32000, (LPVOID*)&DirectXBuffer, &BufferLength, 0, 0, 0);
for (int i = 0; i < 16000; i+= 320) {
AD->Decode(Decoded);
for (int j = 0; j < 320; j++) DirectXBuffer[i+j] = Decoded[j];
}
Buffer->Unlock((LPVOID*)DirectXBuffer, BufferLength, 0, 0);
Buffer->Play(0, 0, DSBPLAY_LOOPING);
Initialized = true;
}
Buffer->Stop();
Buffer->SetCurrentPosition(0);
Buffer->Release();
Buffer = 0;
}
void DSAudio::Stop() {
IsPlaying = false;
}
DSAudio::~DSAudio() {
DestroyWindow(WindowHandle);
Device->Release();
}
int DSAudio::LastPosition = 0;
}
