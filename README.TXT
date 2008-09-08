DAISYINFO
Version 0.2
Copyright (C) 2008 Peter Laursen.

README

This is an extremely simple program that gives you all the meta information about a Daisy book that is available, such as author, title, publisher and a lot of other information.

To run the program:

Windows: Press Windows Key+R and type "cmd" in the Run combo box.
Navigate to the program's folder, usually "C:\Program Files\ABF\daisyinfo"
cd "\program files\abf\daisyinfo"

Type daisyinfo <path> where <path> is the location of a daisy book, for example
daisyinfo "c:\audiobooks\daisy01"

Mac OS X:

Open Terminal from Spotlight.
I believe the program may be in the path. Simply type "daisyinfo <path>" where <path> is the location of a daisy book.

FreeBSD/Linux:

Log in using standard procedures. I assume you have a text shell. If the program is in $PATH, simply type "daisyinfo <path>" where <path> is to be replaced with the location of a daisy book, say
daisyinfo /home/programmer3/daisybook1

REDIRECTING OUTPUT TO A FILE:

Since this program displays a lot of information, it may be very useful to have this information stored in a file. For once, there is a well-defined standard. Invoke the program like this:
daisyinfo <path> > myfile.txt
and all the output is stored in the file "myfile.txt". It can then be viewed in a normal text editor.

KNOWN PROBLEMS:

This version fixes the only known problem with DaisyInfo-0.1, namely the displaying of Danish æ, ø and å.

REPORTING OTHER PROBLEMS:

To report other problems, please don't hesitate to write me an email and I'll do my best to try and solve the problem.

Peter Laursen
