README

DOCUMENTATION FOR THE CONVERTER AND THE PLAYER

Version 0.1
Copyright (C) 2008 Peter Laursen.

Thank you for downloading and trying out these products. We are confident that they will bring you a lot of satisfaction. If not, we're not prepared to give you your money back (you didn't pay for these programs, I trust).

THE CONVERTER:

Imagine that you could just run a Daisy book through a simple converter program. The converter program would leave you with one file that contains the same navigation as Daisy books (well, almost the same navigation).

The Converter.exe file is such a program. It takes a Daisy book and compresses it into one single file, saving you a lot of space. The books I have tested on show a compression ratio of more than 25%.

To convert any daisy book, type
converter <path to book> <outputfile>

This is best illustrated with a quick example. In the following example, we convert a Daisy book in your CD-ROM drive. We assume this drive is named "e:":

1) Since you are reading this file, you must already have downloaded and extracted this file into an empty folder We call this folder c:\abf for now, but feel free to extract it another place.
2) Press Windows Key+R to open the Run dialog box.
3) Type 
c:\abf\converter e: c:\abf\MyFirstAudioBook.abf
and press enter.
3) A console window should now appear. When that window is gone, your book has been converted. To find it, simply browse to c:\abf and look for the file with the .abf file type.

That is all there is to it. If you need help, send me an email (or catch me on Messenger or Skype).

THE PLAYER:

This is a very simple player for the ABF audio book format. We do not support any other formats.

The player is also just a console window, but here, Windows comes to the rescue. For those of you sick and tired of command lines, we have a one-time solution.

Before I describe that, let me briefly run through the player's keyboard commands:

Key - Description
b - Move to the next section as defined by the Daisy book.
z - Move to the previous section
q - Quit

Opening Audio Books:

To make Windows automatically open your ABF audio books, do the following:

1) Open Windows Explorer and find the book. If you're still using our previous example, it is located in c:\abf.
2) Press Enter on the book. An "Open With..." dialog should appear.
3) Tab until you hear an edit field where you can type text. Type something like "ABF Audio Book"
4) Click browse. If you are following our example, you can type in
c:\abf\player.exe
and press Enter.
5) Press Enter on the OK button. From now on, every ABF book should automatically open in the ABF player.

I wish you the best of luck using these two programs. The player and the converter will be updated in the coming days.

Peter.
http://mosedal.net/abf
