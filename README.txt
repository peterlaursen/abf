README

DOCUMENTATION FOR THE CONVERTER AND THE PLAYER

Version 0.30-RC1
Copyright (C) 2008, 2009 Peter Laursen.

What's New in Version 0.30-RC1:

* When the player is launched from the command line, it is now possible to specify multiple files that the player is to play. The files will be played in the order they are specified.
* Developers may take advantage of the new LibABF.dll file located in the installation directory. Unix-people should be looking for a .so file in /usr/local/lib.

What's New in Version 0.22:

This release does not have too many new features. But there are a few worth mentioning:
1) If a book's title contained an apostrophe, the position was not saved. This bug has now been resolved.
2) If the audio was below the target sample rate of 16Khz, the converter wouldn't convert the audio properly. This bug has also been resolved.
3) It is now possible to go to a random section with the g key.
4) It is possible to skip to a new time by entering the position in minutes. This feature is still not perfect, but it is better than nothing.

What's New in Version 0.21?

* A lot of minor bug fixes have made it into this release.
* Better handling of the saved position in the player.
* Both programs now internally use the same ABF library. This has mostly been of no consequence to users, but for me (or the ones who looked at the source code), it was hell to work with.
* The converter has also had various smaller bug fixes.
* The old header of the audio format is no longer supported in this player.
* The converter now shows you what file it is currently converting out of the total number of files it needs to process.
* Two new keyboard commands have been added to the player: f takes you directly to the beginning of the book, l takes you to the last section of the book.

WHAT'S NEW IN VERSION 0.2?

Many things have been updated in this latest version of the programs. Here is a short list:

* The format of audio books have been updated with some more data, such as title, author and how long the book lasts. All this information is displayed in the player's window.
* The player now has a pause feature.
* The player automatically stores the last position in your audio book. If you have read the book and quit the player, the position you quit at is stored.
* The player now has a volume control. Press the < or > key to lower or raise the volume.

INTRODUCTION:

Thank you for downloading and trying out these products. We are confident that they will bring you a lot of satisfaction. 

THE CONVERTER:

Imagine that you could just run a Daisy book through a simple converter program. The converter program would leave you with one file that contains the same navigation as Daisy books (well, almost the same navigation).

The Converter.exe file is such a program. It takes a Daisy book and compresses it into one single file, saving you a lot of space. The books I have tested on show a compression ratio of more than 25%.

To convert any daisy book, type
converter <path to book> <outputfile>

This is best illustrated with a quick example. In the following example, we convert a Daisy book in your CD-ROM drive. We assume this drive is named "e:":

1) Press Windows Key+R to open the run dialog.
Type 
converter e: d:\audiobooks\MyAudioBook.abf
and press enter.
2) A console window should now appear. When that window is gone, your book has been converted. To find it, simply browse to d:\audiobooks and look for the file with the .abf file extension.
While the converter is converting, it shows what file it is currently converting out of the total number of files.

That is all there is to it. If you need help, send me an email 

THE PLAYER:

The player has been heavily updated from the previous version. 
This is a very simple player for the ABF audio book format. We do not support any other formats.

keyboard commands:

Key - Description
f - Move to the first section.
l - Move to the last section.
b - Move to the next section as defined by the Daisy book.
v or c - Pause the ABF book
x - Resume playback
z - Move to the previous section
< - The less than character lowers the volume.
> - The greater than character raises the volume.
q - Quit

The player automatically saves the last position in your audio book, so the next time you start listening to the audio book, you should be right at the position where you left off.
I wish you the best of luck using these two programs. 

Peter.
http://mosedal.net/abf
