README

DOCUMENTATION FOR THE CONVERTER AND THE PLAYER

Version 0.30-Alpha1
Copyright (C) 2008, 2009 Peter Laursen.

NOTE:

This is Alpha software. Run at your own risk. This software has not been known to eat you alive, but you must expect some bugs. Also, no guarantee is made for correct operation of alpha programs.

What's New in Version 0.30-Alpha1:

* Added a simple playlist to the player.
* When the player is launched from the command line, it is now possible to specify multiple files that the player is to play. The files will be played in the order they are specified.
* Developers may take advantage of the new LibABF.dll file located in the installation directory. Unix-people should be looking for a .so file in /usr/local/lib.
* Fixed a few bugs in the player, having to do with CurrentSection and Jump to Time.
* Fixed a bug in LibDaisy so that the Daisy book is interpreted correctly, if the </body> close tag was put on the same line as the tag containing the link itself.

What's New in Version 0.22:

This release does not have too many new features. But there are a few worth mentioning:
1) If a book's title contained an apostrophe, the position was not saved. This bug has now been resolved.
2) If the audio was below the target sample rate of 16Khz, the converter wouldn't convert the audio properly. This bug has also been resolved.
3) It is now possible to go to a random section with the g key.
4) It is possible to skip to a new time by entering the position in minutes. This feature is still not perfect, but it is better than nothing.

What's New in Version 0.21:

* A lot of minor bug fixes have made it into this release.
* Better handling of the saved position in the player.
* Both programs now internally use the same ABF library. This has mostly been of no consequence to users, but for me (or the ones who looked at the source code), it was hell to work with.
* The converter has also had various smaller bug fixes.
* The old header of the audio format is no longer supported in this player.
* The converter now shows you what file it is currently converting out of the total number of files it needs to process.
* Two new keyboard commands have been added to the player: f takes you directly to the beginning of the book, l takes you to the last section of the book.

WHAT'S NEW IN VERSION 0.2:

Many things have been updated in this latest version of the programs. Here is a short list:

* The format of audio books have been updated with some more data, such as title, author and how long the book lasts. All this information is displayed in the player's window.
* The player now has a pause feature.
* The player automatically stores the last position in your audio book. If you have read the book and quit the player, the position you quit at is stored.
* The player now has a volume control. Press the < or > key to lower or raise the volume.

INTRODUCTION:

Since its inception in August, 2008, ABF has proven its worth as a format for audio booksz. Its elegant design, its robustness and the audio quality have made it a good competitor to formats such as DAISY.

The two formats strive towards the same goal, namely that of being widely used for audio books. This format, ABF, has numerous advantages over DAISY:

* Instead of you having hundreds or even thousands of files in a DAISY book, you only have one if you use ABF.
* The quality is the same as for DAISY.
* If you were to stream the format, bandwidth consumption would be very low compared to DAISYs audio format of choice, namely MP3.

WHAT THIS PACKAGE CONTAINS:

The package you have downloaded contains the following programs:
* Converter: Is a program for converting DAISY books to ABF.
* player: Is the ABF player.

SYSTEM REQUIREMENTS:

Although every effort has been made to minimise dependencies on non-standard libraries, a few exceptions had to be made. Below are the system requirements for the products contained in this package.

* .NET Framework 3.5: Windows users should download .NET framework 3.5.
* Visual Studio 2005 Redistributable: Windows users should also download this package.
* Ncurses library: The users of other operating systems need the Ncurses library.
* A sound card (big surprise)
* 1-2 GB of disk space, for audio data. The programs themselves do not take up more than a megabyte or so.

THE CONVERTER:

The converter's job is to convert a DAISY book into ABF. The converter accomplishes this by reading the DAISY book and transforming the audio files into the more compact ABF.

USING THE CONVERTER:

The converter is a command line application. This means that the application has no buttons, text boxes or other smart features. The reason for this is quite simple: Portability.

No matter what the platform, you invoke the converter like this:

converter <path to daisy book> <ABF output>
where <path to daisy book> specifies where your daisy book is located and <ABF Output> specifies the location and name of your ABF file.

EXAMPLE::

Below is a short example of a typical invocation:

converter e:\ c:\abf\testbook.abf

A couple of assumptions were made above:
1) Your CD-ROM drive was "E:\"
2) You had created a folder called c:\abf.

WHAT TO DO NEXT:

Your DAISY book should take some time to convert. When it has finished, you can open the audio book in the ABF player and listen to it.

THE PLAYER:

This is a very simple player for the ABF audio book format. We do not support any other formats, neither is support for them planned.

THE PLAYER'S DISPLAY:

The player is, like the converter, a console application. Therefore, there are no buttons, text boxes or other smart features.

One advantage to such a setup is that no mouse action works inside this window (which should be good for you anyway). The only means of navigating within the audio book is by using specific hotkeys.

The screen displays a little bit of information. Some of that information is the book's title, the author, the time and the hotkey commands in the player.

keyboard commands:

Key - Description

f - Move to the first section (e.g. beginning) of the audio book.
l - Move to the beginning of the last section in the book.
b - Move to the next section inside the audio book.
Shift+B - Move to the next book in the playlist.
 z - Move to the previous section
Shift+Z - Move to the previous book in the playlist.
 v or c - Pause the player.
x - Resume playback from the last position
< - The less than character lowers the volume.
> - The greater than character raises the volume.
q - Quit the player and save the current position.

LIMITATIONS:

As with every suite of programs, there are some limitations. These limitations are currently known and we are working to solve them. Below they are:

* The converter cannot yet convert multi-volume DAISY books. We are working on this.
* The converter can convert DAISY 2.02 books only. We are also working on this one.
* The player cannot save more than one position per audio book.
* The products for Ubuntu suffer from terrible integration with Pulceaudio. If you must use these, the developers recommend you use Ubuntu 8.10 since it gives a much nicer experience for everyone.

ACKNOWLEDGEMENTS:

We, at ABF, thank the following projects and people:

http://speex.org - The audio format we use.
http://audiere.sf.net - The audio library we use.

CONTACTING US:

To contact us, please visit

http://mosedal.net/abf

