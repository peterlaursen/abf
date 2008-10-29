README

DOCUMENTATION FOR THE CONVERTER AND THE PLAYER

Version 0.21
Copyright (C) 2008 Peter Laursen.

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
