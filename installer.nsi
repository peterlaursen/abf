/*
In order to discover what installation package we are to use, we'll try to package our ABF products using NSIS.
;
; This is done for a couple reasons:
; * To learn this installer
; * To make it easier to use with Winamp
; * Because it has a unique way of doing things
; * Because it can be run on FreeBSD.
; 
; Product Name: ABF Products
; Copyright (C) 2008, 2009 Peter Laursen.
; Contents: ABF Converter, ABF Player, Experimental File Converter, Winamp Plugin (if selected), ReadMe file, Required Libraries
; Version used to generate this installer: NSIS 2.45

*/
; Include the File functions header
!include "FileFunc.nsh"
; Tell the user that this is a test-only installer.
Function .onInit
MessageBox MB_OK "This is a test installer. It will, in time, be able to install the ABF products."
FunctionEnd
; Set text on the title bar
Caption "ABF Products 0.30-Alpha1 Test Installer."
; Set the Installer name - not quite sure what this does...
Name "ABF Products"
; Installer filename
OutFile "installer.exe"
; Set the privileges for Windows Vista and 7
RequestExecutionLevel admin
; Ask the compiler to compress using LZMA
SetCompressor /solid lzma
InstallDir "$PROGRAMFILES\abf-nsis\"
Page license
Page directory
Page Components
Page instfiles
LicenseForceSelection radiobuttons "I have read and understood the license" "I have read but decline to accept generous license terms"
LicenseData "license.txt"
Section "Main Components (Required)"
SectionIn RO
SetOutPath $INSTDIR
File "d:\audiere-1.9.4-win32\bin\audiere.dll"
File "converter.exe"
File "player.exe"
	File "libdaisy.dll"
File "libabf.dll"
File "libspeexdsp.dll"
File "Readme.txt"
SectionEnd
; Try to install the Winamp Plugin if selected
Section "Winamp Plugin"
; We'll try to access some registry values.
  ReadRegStr $0 HKLM \
     "Software\Microsoft\Windows\CurrentVersion\Uninstall\Winamp" \ 
     "UninstallString"
${GetParent} $0 $0
StrCpy $INSTDIR "$0\plugins"
SetOutPath $PROGRAMFILES\Winamp\Plugins
File "in_abf.dll"
File "libspeexdsp.dll"
File "libabf.dll"
SectionEnd
