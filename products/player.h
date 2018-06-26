/* $Id$
Copyright (C) 2008, 2009, 2010, 2011, 2012, 2013, 2014, 2015 Peter Laursen.

This file contains various declarations that are used within the player.
*/
#ifndef PLAYER_H
#define PLAYER_H
#include "playlist.h"
#include "../abfdecoder/abfdecoder.h"
#ifdef FREEBSD
#include "unixaudio.h"
#elif defined(PULSE)
#include "linuxaudio.h"
#endif
#include <thread>
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
