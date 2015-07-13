/* $Id$
Copyright (C) 2009-2015 Peter Laursen.
Code taken from in_raw.cpp

** Example Winamp .ABF input plug-in
** Copyright (c) 2009 Peter Laursen.
**
**  This plugin is heavily based on the .RAW input plugin that comes with the Winamp SDK. I am pleased to see that the plugin works for Winamp as it is now, but several things can be improved.
**
*/
// Include the necessary files
#include <windows.h>

#include <winamp/in2.h>
#include "..\libabf\libabf.h"
#include "..\products\database.h"
using namespace ABF;
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "kernel32.lib")

// post this to the main window at end of file (after playback as stopped)
#define WM_WA_MPEG_EOF WM_USER+2


/* ABF configuration.

ABF is, at least in its current form, an audio format compressed at 16000 herz, mono. This might change later.
*/

#define NCH 1
#define SAMPLERATE 16000
#define BPS 16


extern In_Module mod;			// the output module (filled in near the bottom of this file)

char lastfn[MAX_PATH];	// currently playing file (used for getting info on the current file)

int file_length;		// file length, in bytes
int decode_pos_ms;		// current decoding position, in milliseconds. 
						// Used for correcting DSP plug-in pitch changes
int paused;				// are we paused?
volatile int seek_needed; // if != -1, it is the point that the decode 
						  // thread should seek to, in ms.

AbfDecoder* AD = NULL; // Pointer to our decoder
volatile int killDecodeThread=0;			// the kill switch for the decode thread
HANDLE thread_handle=INVALID_HANDLE_VALUE;	// the handle to the decode thread


DWORD WINAPI DecodeThread(LPVOID b); // the decode thread procedure

void config(HWND hwndParent)
{
	MessageBox(hwndParent,
		"This plugin has no configuration. .ABF has a fixed file format and can therefore not be configured.",
		"No Configuration",MB_OK);
	// if we had a configuration box we'd want to write it here (using DialogBox, etc)
}
void about(HWND hwndParent)
{
	MessageBox(hwndParent,"ABF Plugin for Winamp 5.5, Version 0.03\r\nCopyright (C) 2009-2015 Peter Laursen\r\n\r\nThis plugin only plays ABF 2.0 books. It does not allow you to navigate or anything else. For more information, visit http://tdlsoftware.net/abf/\r\n\r\nBased on version 2.1 of LibABF.",
		"About ABF Winamp Player",MB_OK);
}

void init() { 
	/* any one-time initialization goes here (configuration reading, etc) */ 
}

void quit() { 
	/* one-time deinit, such as memory freeing */ 
}

int isourfile(const char *fn) { 
// used for detecting URL streams.. unused here. 
// return !strncmp(fn,"http://",7); to detect HTTP streams, etc
	return 0; 
} 


// called when winamp wants to play a file
int play(const char *fn) 
{ 
	int maxlatency;
	int thread_id;

	paused=0;
	decode_pos_ms=0;
	seek_needed=-1;

	
	
// Set up ABF so that the files can be played
AD = new AbfDecoder((char*)fn);
if (!AD->IsValid()) {
		// we return error. 1 means to keep going in the playlist, -1
		// means to stop the playlist. In ABF, we just continue to the next file.
		return 1;
	}

	file_length=0; // GetFileSize(input_file,NULL);
int LastPosition = GetLastPosition(AD->GetTitle());
if (LastPosition) AD->Seek(LastPosition, SEEK_SET);

	
	strcpy(lastfn,fn);

	// -1 and -1 are to specify buffer and prebuffer lengths.
	// -1 means to use the default, which all input plug-ins should
	// really do.
	maxlatency = mod.outMod->Open(SAMPLERATE,NCH,BPS, -1,-1); 

	// maxlatency is the maximum latency between a outMod->Write() call and
	// when you hear those samples. In ms. Used primarily by the visualization
	// system.

	if (maxlatency < 0) // error opening device
	{
delete AD;
		return 1;
	}
	// dividing by 1000 for the first parameter of setinfo makes it
	// display 'H'... for hundred.. i.e. 14H Kbps.
	mod.SetInfo((SAMPLERATE*BPS*NCH)/1000,SAMPLERATE/1000,NCH,1);

	// initialize visualization stuff
	mod.SAVSAInit(maxlatency,SAMPLERATE);
	mod.VSASetInfo(SAMPLERATE,NCH);

	// set the output plug-ins default volume.
	// volume is 0-255, -666 is a token for
	// current volume.
	mod.outMod->SetVolume(-666); 

	// launch decode thread
	killDecodeThread=0;
	thread_handle = (HANDLE) 
		CreateThread(NULL,0,(LPTHREAD_START_ROUTINE) DecodeThread,NULL,0,(LPDWORD)&thread_id);
	
	return 0; 
}

// standard pause implementation
void pause() { paused=1; mod.outMod->Pause(1); }
void unpause() { paused=0; mod.outMod->Pause(0); }
int ispaused() { return paused; }


// stop playing.
void stop() { 
	if (thread_handle != INVALID_HANDLE_VALUE)
	{
		killDecodeThread=1;
		if (WaitForSingleObject(thread_handle,10000) == WAIT_TIMEOUT)
		{
			MessageBox(mod.hMainWindow,"error asking thread to die!\n",
				"error killing decode thread",0);
			TerminateThread(thread_handle,0);
		}
		CloseHandle(thread_handle);
		thread_handle = INVALID_HANDLE_VALUE;
	}

	// close output system
	mod.outMod->Close();

	// deinitialize visualization
	mod.SAVSADeInit();
	

// Clean up ABF stuff.
int LastPosition = AD->ftell();
if (LastPosition < 0) LastPosition = 0;
if (!AD->feof()) SaveLastPosition(AD->GetTitle(), LastPosition);
else DeletePosition(AD->GetTitle());

delete AD;
AD = NULL;

}


// returns length of playing track
int getlength() {
return -1;
}


// returns current output position, in ms.
// you could just use return mod.outMod->GetOutputTime(),
// but the dsp plug-ins that do tempo changing tend to make
// that wrong.
int getoutputtime() { 
	return decode_pos_ms+
		(mod.outMod->GetOutputTime()-mod.outMod->GetWrittenTime()); 
}


// called when the user releases the seek scroll bar.
// usually we use it to set seek_needed to the seek
// point (seek_needed is -1 when no seek is needed)
// and the decode thread checks seek_needed.
void setoutputtime(int time_in_ms) { 
	seek_needed=time_in_ms; 
}


// standard volume/pan functions
void setvolume(int volume) { mod.outMod->SetVolume(volume); }
void setpan(int pan) { mod.outMod->SetPan(pan); }

// this gets called when the user hits Alt+3 to get the file info.
// if you need more info, ask me :)

int infoDlg(const char *fn, HWND hwnd)
{
char Message[1024];
sprintf(Message, "ABF INFORMATION\r\nTitle: %s\r\nAuthor: %s\r\nThis book lasts %s\r\n", AD->GetTitle(), AD->GetAuthor(), AD->GetTime());
MessageBox(hwnd, Message, "Information", MB_OK);
	return 0;
}


// this is an odd function. it is used to get the title and/or
// length of a track.
// if filename is either NULL or of length 0, it means you should
// return the info of lastfn. Otherwise, return the information
// for the file in filename.
// if title is NULL, no title is copied into it.
// if length_in_ms is NULL, no length is copied into it.
void getfileinfo(const char *filename, char *title, int *length_in_ms)
{
	if (!filename || !*filename)  // currently playing file
	{
		if (length_in_ms) *length_in_ms=getlength();
		if (title) title = (char*)AD->GetTitle();
	}
	else // some other file
	{
		if (length_in_ms) // calculate length
		{
			HANDLE hFile;
			hFile = CreateFile(filename,GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,
				OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
			if (hFile != INVALID_HANDLE_VALUE)
			{
				*length_in_ms = (GetFileSize(hFile,NULL)*10)/(SAMPLERATE/100*NCH*(BPS/8));
				CloseHandle(hFile);
			}
			else *length_in_ms=-1000; // the default is unknown file length (-1000).
		}
		if (title) // get non path portion of filename
		{
AbfDecoder Temp((char*)filename);
title = (char*)Temp.GetTitle();
}
	}
}

void eq_set(int on, char data[10], int preamp) 
{ 
	// most plug-ins can't even do an EQ anyhow.. I'm working on writing
	// a generic PCM EQ, but it looks like it'll be a little too CPU 
	// consuming to be useful :)
	// if you _CAN_ do EQ with your format, each data byte is 0-63 (+20db <-> -20db)
	// and preamp is the same. 
}


// render 640 samples into buf. 
// this function is only used by DecodeThread. 

// note that if you adjust the size of sample_buffer, for say, 1024
// sample blocks, it will still work, but some of the visualization 
// might not look as good as it could. Stick with 640 sample blocks
// if you can, and have an additional auxiliary (overflow) buffer if 
// necessary.. 
int get_640_samples(char *buf)
{
	int l;
	// Get 640 ABF samples. This is 2 frames of ABF
short Output[320];
static short AllOutput[640];
for (int i = 0; i < 640; i += 320) {
AD->Decode(Output);
for (int j = 0; j < 320; j++) AllOutput[i+j] = Output[j];
}
return 640;
}



/*
DWORD WINAPI DecodeThread(LPVOID b)
{
	int done=0; // set to TRUE if decoding has finished
	while (!killDecodeThread) 
	{
		if (seek_needed != -1) // seek is needed.
		{
			int offs;
			decode_pos_ms = seek_needed;
			seek_needed=-1;
			done=0;
			mod.outMod->Flush(decode_pos_ms); // flush output device and set 
											  // output position to the seek position
			offs = MulDiv(decode_pos_ms,SAMPLERATE,1000); // decode_pos_ms*SAMPLERATE/1000
		}

		if (done) // done was set to TRUE during decoding, signaling eof
		{
			mod.outMod->CanWrite();		// some output drivers need CanWrite
									    // to be called on a regular basis.

			if (!mod.outMod->IsPlaying()) 
			{
				// we're done playing, so tell Winamp and quit the thread.
				PostMessage(mod.hMainWindow,WM_WA_MPEG_EOF,0,0);
				return 0;	// quit thread
			}
			Sleep(10);		// give a little CPU time back to the system.
		}
		else if (mod.outMod->CanWrite() >= ((640*NCH*(BPS/8))*(mod.dsp_isactive()?2:1)))
			// CanWrite() returns the number of bytes you can write, so we check that
			// to the block size. the reason we multiply the block size by two if 
			// mod.dsp_isactive() is that DSP plug-ins can change it by up to a 
			// factor of two (for tempo adjustment).
		{	
			int l=640*NCH*(BPS/8);			       // block length in bytes
			static char sample_buffer[640*NCH*(BPS/8)*2]; 
												   // sample buffer. twice as 
												   // big as the blocksize

			l=get_640_samples(sample_buffer);	   // retrieve samples
			if (!l)			// no samples means we're at eof
			{
				done=1;
			}
			else	// we got samples!
			{
				// give the samples to the vis subsystems
				mod.SAAddPCMData((char *)sample_buffer,NCH,BPS,decode_pos_ms);	
				mod.VSAAddPCMData((char *)sample_buffer,NCH,BPS,decode_pos_ms);
				// adjust decode position variable
				decode_pos_ms+=(640*1000)/SAMPLERATE;	

				// if we have a DSP plug-in, then call it on our samples
				if (mod.dsp_isactive()) 
					l=mod.dsp_dosamples(
						(short *)sample_buffer,l/NCH/(BPS/8),BPS,NCH,SAMPLERATE
					  ) // dsp_dosamples
					  *(NCH*(BPS/8));

				// write the pcm data to the output system
				mod.outMod->Write(sample_buffer,l);
			}
		}
		else Sleep(20); 
		// if we can't write data, wait a little bit. Otherwise, continue 
		// through the loop writing more data (without sleeping)
	}
	return 0;
}
*/
// The above function was pure code. It was the original thread function. Remove it at some time when I understand everything it does. The ABF decode function is below.

DWORD WINAPI DecodeThread(LPVOID b) {
short Output[320];
while (!killDecodeThread) {
AD->Decode(Output);
mod.outMod->Write((char*)Output, 640);
while (mod.outMod->IsPlaying());
while (paused);
if (AD->feof()) {
PostMessage(mod.hMainWindow,WM_WA_MPEG_EOF,0,0);
break;
}
}
return 0;
}

// module definition.

In_Module mod = 
{
	IN_VER,	// defined in IN2.H
	"ABF Plugin Player v0.03"
	// winamp runs on both alpha systems and x86 ones. :)
#ifdef __alpha
	"(AXP)"
#else
	"(x86)"
#endif
	,
	0,	// hMainWindow (filled in by winamp)
	0,  // hDllInstance (filled in by winamp)
	"ABF\0ABF Audio Book (*.ABF)\0"
	// this is a double-null limited list. "EXT\0Description\0EXT\0Description\0" etc.
	,
	0,	// is_seekable
	1,	// uses output plug-in system
	config,
	about,
	init,
	quit,
	getfileinfo,
	infoDlg,
	isourfile,
	play,
	pause,
	unpause,
	ispaused,
	stop,
	
	getlength,
	getoutputtime,
	setoutputtime,

	setvolume,
	setpan,

	0,0,0,0,0,0,0,0,0, // visualization calls filled in by winamp

	0,0, // dsp calls filled in by winamp

	0,

	NULL,		// setinfo call filled in by winamp

	0 // out_mod filled in by winamp

};

// exported symbol. Returns output module.

extern "C" __declspec( dllexport ) In_Module * winampGetInModule2()
{
	return &mod;
}

