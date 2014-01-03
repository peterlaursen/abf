/* $Id$

This file is public domain - we can therefore use it for our project without any license quirks.
We do, however, include the original file, including comments.
This file has been taken from the following web address:
http://www.linuxmisc.com/9-unix-programmer/9218bf8724abe157.htm

*/
/*
 * scandir() for win32
 * this tool should make life easier for people writing for both unix
 and wintel
 * written by Tom Torfs, 2002/10/31
 * donated to the public domain; use this code for anything you like
 as long as
 * it is understood there are absolutely *NO* warranties of any kind,
 even implied
 */
#ifndef SCANDIR_H
#define SCANDIR_H
#ifdef WIN32
#include <stdio.h>
#define MAXNAMLEN FILENAME_MAX
/* directory entry structure */
 struct dirent {
         char d_name[MAXNAMLEN+1];      /* name of directory entry (0
 terminated) */
     unsigned long d_ino;               /* file serial number -- will be 0 for
 win32 */
     short d_namlen;            /* length of string in d_name */
     short d_reclen;            /* length of this record */
 };
/* the scandir() function */
 int scandir(
     const char *dirname,
     struct dirent ***namelist,
     int (*select)(const struct dirent *),
     int (*compar)(
          const struct dirent **,
          const struct dirent **
     )
 );
/* compare function for scandir() for alphabetic sort
   (case-insensitive on Win32) */
 int alphasort(
     const struct dirent **d1,
     const struct dirent **d2
 );
/* case-insensitive strcmp() */
 int istrcmp(
         const char *s1,
         const char *s2
 );
#endif
#endif
