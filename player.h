/* PLAYER.H
Copyright (C) 2009 Peter Laursen.

This file contains various declarations that are used within the player.
*/
#ifndef PLAYER_H
#define PLAYER_H
#include <audiere.h>
#include <windows.h>
#include <libabf.h>
#ifdef WIN32
void Prebuffer(void*);
#else
void* Prebuffer(void*);
#endif
enum PlayerStatus {
Nothing = 0, // A default value only to be used in the beginning
Playing,
Paused,
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
