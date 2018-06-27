/*
$Id$
Copyright (C) 2009, 2010, 2011, 2012, 2013, 2014 Peter Laursen.

In order to discover what installation package we are to use, we'll try to package our ABF products using NSIS.
This is done for a couple reasons:
 * To learn this installer
 * To make it easier to use with Winamp
 * Because it has a unique way of doing things
 * Because it can be run on FreeBSD.
Product Name: ABF Products
Copyright (C) 2008-2018 Peter Laursen.
Contents: ABF Converter, ABF Player, ReadMe file, License,  Required Libraries (ABFDecoder,, ABFEncoder, LIBDaisy20)
Version used to generate this installer: NSIS Version 3.03
*/
!Include "winmessages.nsh"
; Set the Installer name - This is shown on the title bar
Name "ABF Products"
; Installer filename
OutFile "abf0.50-installer.exe"
; Set the privileges for Windows Vista and 7
RequestExecutionLevel admin
; Ask the compiler to compress using LZMA
SetCompressor /solid lzma
InstallDir "$PROGRAMFILES64\abf\"
Page license
Page directory
Page Components
Page instfiles
UninstPage uninstConfirm
UninstPage InstFiles
LicenseForceSelection radiobuttons "I have read and understood the license" "I have read but decline to accept generous license terms"
LicenseData "license.txt"
InstType "Ordinary Components"
InstType "Player Only"
Var ABFInstallDir
Function CreateUninstaller
WriteUninstaller "$ABFInstallDir\ABF-Uninstaller.exe"
FunctionEnd
; This function performs some after-installation setup
Function AfterFileCopying
; Create the uninstaller
Call CreateUninstaller
; After this, create some start menu icons.
CreateDirectory "$SMPROGRAMS\ABF Products"
SetOutPath "$SMPROGRAMS\ABF Products"
CreateShortCut "$SMPROGRAMS\ABF Products\Documentation.lnk" "$INSTDIR\README.txt"
CreateShortCut "$SMPROGRAMS\ABF Products\Launch Player.lnk" "$INSTDIR\abfplayer.exe"
CreateShortCut "$SMPROGRAMS\ABF Products\Uninstall ABF Products.lnk" "$INSTDIR\ABFUninstaller.exe"
CreateShortCut "$SMPROGRAMS\ABF Products\ABF Website.lnk" "http://tdlsoftware.net/abf"
; Try to set a file association
StrCpy $5 "ABFAudioBook"
WriteRegStr HKCR ".abf" "" $5
WriteRegStr HKCR "ABFAudioBook" "" "ABF Audio Book"
WriteRegStr HKCR "ABFAudioBook\shell\open\command" "" '"$ABFInstallDir\abfplayer.exe" "%1"'
!define SHCNF_FLUSH        0x1000
!define SHCNE_ASSOCCHANGED 0x08000000
System::Call "shell32::SHChangeNotify(i,i,i,i) (${SHCNE_ASSOCCHANGED}, ${SHCNF_FLUSH}, 0, 0)"
FunctionEnd
Section "Standard Working Components"
SectionIn 1 2
StrCpy $ABFInstallDir $INSTDIR
SetOutPath $INSTDIR
File ".\*.dll"
File ".\abfconverter.exe"
File ".\abfplayer.exe"
File "Readme.txt"
file "License.txt"
Call AfterFileCopying
SectionEnd
Section /O "Player Only"
SectionIn 2
StrCpy $ABFInstallDir $INSTDIR
SetOutPath $INSTDIR
File ".\abfdecoder.dll"
File ".\abfplayer.exe"
Call AfterFileCopying
SectionEnd
Section "Uninstall"
Delete $INSTDIR\*.*
RmDir $INSTDIR
Delete "$SMPROGRAMS\ABF Products\*.*"
RmDir "$SMPROGRAMS\ABF Products"
SectionEnd
