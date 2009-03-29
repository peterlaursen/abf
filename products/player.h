/* PLAYER.H
Copyright (C) 2009 Peter Laursen.

This file contains various declarations that are used within the player.
*/
#ifndef PLAYER_H
#define PLAYER_H
#include <audiere.h>
#include "playlist.h"
#ifdef WIN32
#include <windows.h>
#endif
#include <libabf.h>
#ifdef WIN32
void Prebuffer(void*);
#else
void* Prebuffer(void*);
#endif
#ifdef WIN32
typedef HANDLE ThreadType;
#endif
#ifdef LINUX
typedef pthread_t ThreadType;
#endif
#ifdef FREEBSD
typedef thread* ThreadType;
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
#endif
