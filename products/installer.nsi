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
!Include "winmessages.nsh"
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
UninstPage uninstConfirm
UninstPage InstFiles
LicenseForceSelection radiobuttons "I have read and understood the license" "I have read but decline to accept generous license terms"
LicenseData "license.txt"
InstType "Ordinary Components"
InstType "Experimental Converter"
InstType "Player Only"
InstType "Winamp Plugin Only"
Var ABFInstallDir
Function CreateUninstaller
WriteUninstaller "$ABFInstallDir\ABFUninstaller.exe"
FunctionEnd

Section "Main Components (Required)"
SectionIn 1 2
StrCpy $ABFInstallDir $INSTDIR
SetOutPath $INSTDIR
File "d:\audiere-1.9.4-win32\bin\audiere.dll"
File "converter.exe"
File "player.exe"
	File "libdaisy.dll"
File "libabf.dll"
File "libspeexdsp.dll"
File "Readme.txt"
ReadRegStr $1 HKCU "Environment" "Path"
StrCpy $1 "$1;$ABFInstallDir"
WriteRegStr HKCU "Environment" "Path" $1
Call CreateUninstaller

 SendMessage ${HWND_BROADCAST} ${WM_WININICHANGE} 0 "STR:Environment" /TIMEOUT=5000

SectionEnd
Section /O "Experimental Converter"
SectionIn 2
StrCpy $INSTDIR $ABFInstallDir 
SetOutPath $INSTDIR
File "audiere.dll"
File "..\fileconverter\fileconverter.exe"
File "libspeexdsp.dll"
File "readme.txt"
File "converter.exe"
File "player.exe"
File "libdaisy.dll"
Call CreateUninstaller
ReadRegStr $1 HKCU "Environment" "Path"
WriteRegStr HKCU "Environment" "Path" "$1;$ABFInstallDir"
SendMessage ${HWND_BROADCAST} ${WM_WININICHANGE} 0 "STR:Environment" /TIMEOUT=5000
SectionEnd
Section /O "Player Only"
SectionIn 3
StrCpy $ABFInstallDir $INSTDIR
SetOutPath $INSTDIR
File "libabf.dll"
File "player.exe"
File "audiere.dll"
Call CreateUninstaller
ReadRegStr $1 HKCU "Environment" "Path"
WriteRegStr HKCU "Environment" "Path" "$1;$ABFInstallDir"
SendMessage ${HWND_BROADCAST} ${WM_WININICHANGE} 0 "STR:Environment" /TIMEOUT=5000
SectionEnd
Function WinampPath
  Push $0
  Push $1
  Push $2
  ReadRegStr $0 HKLM \
     "Software\Microsoft\Windows\CurrentVersion\Uninstall\Winamp" \ 
     "UninstallString"
  StrCmp $0 "" fin

    StrCpy $1 $0 1 0 ; get firstchar
    StrCmp $1 '"' "" getparent 
      ; if first char is ", let's remove "'s first.
      StrCpy $0 $0 "" 1
      StrCpy $1 0
      rqloop:
        StrCpy $2 $0 1 $1
        StrCmp $2 '"' rqdone
        StrCmp $2 "" rqdone
        IntOp $1 $1 + 1
        Goto rqloop
      rqdone:
      StrCpy $0 $0 $1
    getparent:
    ; the uninstall string goes to an EXE, let's get the directory.
    StrCpy $1 -1
    gploop:
      StrCpy $2 $0 1 $1
      StrCmp $2 "" gpexit
      StrCmp $2 "\" gpexit
      IntOp $1 $1 - 1
      Goto gploop
    gpexit:
    StrCpy $0 $0 $1

    StrCmp $0 "" fin
    IfFileExists $0\winamp.exe fin
      StrCpy $0 ""
  fin:
  Pop $2
  Pop $1
FunctionEnd
; Try to install the Winamp Plugin if selected
Section /O "Winamp Plugin"
; We'll try to access some registry values.
CreateDirectory $INSTDIR
SectionIn 4
Call WinampPath
StrCpy $0 "$0\plugins"
SetOutPath $0
File "in_abf.dll"
File "libabf.dll"
Call CreateUninstaller
SectionEnd
Function un.WinampPath
  Push $0
  Push $1
  Push $2
  ReadRegStr $0 HKLM \
     "Software\Microsoft\Windows\CurrentVersion\Uninstall\Winamp" \ 
     "UninstallString"
  StrCmp $0 "" fin

    StrCpy $1 $0 1 0 ; get firstchar
    StrCmp $1 '"' "" getparent 
      ; if first char is ", let's remove "'s first.
      StrCpy $0 $0 "" 1
      StrCpy $1 0
      rqloop:
        StrCpy $2 $0 1 $1
        StrCmp $2 '"' rqdone
        StrCmp $2 "" rqdone
        IntOp $1 $1 + 1
        Goto rqloop
      rqdone:
      StrCpy $0 $0 $1
    getparent:
    ; the uninstall string goes to an EXE, let's get the directory.
    StrCpy $1 -1
    gploop:
      StrCpy $2 $0 1 $1
      StrCmp $2 "" gpexit
      StrCmp $2 "\" gpexit
      IntOp $1 $1 - 1
      Goto gploop
    gpexit:
    StrCpy $0 $0 $1

    StrCmp $0 "" fin
    IfFileExists $0\winamp.exe fin
      StrCpy $0 ""
  fin:
  Pop $2
  Pop $1
FunctionEnd
Section "Uninstall"
Delete $INSTDIR\*.*
RmDir $INSTDIR
Call un.WinampPath
StrCpy $0 "$0\plugins"
Delete "$0\in_abf.dll"
Delete "$0\libabf.dll"
pop $0
Delete "$PROFILE\.abfplayer.db"
DeleteRegValue HKCU "Environment" "Path"
SectionEnd
