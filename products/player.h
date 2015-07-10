/* $Id$
$Copyright$

This file contains various declarations that are used within the player.
*/
#ifndef PLAYER_H
#define PLAYER_H
#include "playlist.h"
#include "../libabf/libabf.h"
#ifdef WIN32
#include <windows.h>
#include "dsaudio.h"
#else
#include "unixaudio.h"
#endif

#ifdef WIN32
typedef HANDLE ThreadType;
#endif
#ifdef LINUX
typedef pthread_t ThreadType;
#endif
#ifdef FREEBSD
typedef pthread* ThreadType;
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
