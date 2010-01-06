/* PLAYER.H
Copyright (C) 2009 Peter Laursen.

This file contains various declarations that are used within the player.
*/
#ifndef PLAYER_H
#define PLAYER_H
#include "playlist.h"
#ifdef WIN32
#include <windows.h>
#include "dsaudio.h"
#else
#include "unixaudio.h"
#endif

#include <libabf.h>
#ifdef WIN32
typedef HANDLE ThreadType;
typedef DSAudio AudioSystem;
#endif
#ifdef LINUX
typedef pthread_t ThreadType;
typedef UnixAudio AudioSystem;
#endif
#ifdef FREEBSD
typedef pthread* ThreadType;
typedef UnixAudio AudioSystem;
#endif
enum PlayerStatus {
Nothing = 0, // A default value only to be used in the beginning
Playing,
Paused,
PreviousBook,
NextBook,
AddBook,
RemoveBook,
Previous,
Next,
VolumeUp,
VolumeDown,
FirstSection,
LastSection,
GoToSection,
GoTime,
BookIsFinished,
Quit
};
void Input();
extern volatile PlayerStatus PS;
#endif
