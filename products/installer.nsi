/*
In order to discover what installation package we are to use, we'll try to package our ABF products using NSIS.
This is done for a couple reasons:
 * To learn this installer
 * To make it easier to use with Winamp
 * Because it has a unique way of doing things
 * Because it can be run on FreeBSD.
Product Name: ABF 2.0 Products
Copyright (C) 2008, 2009, 2010, 2011, 2012 Peter Laursen.
Contents: ABF Converter, ABF Player, Experimental File Converter (Graphical), Winamp Plugin (if selected), ReadMe file Required Libraries (LIBABF, LIBDaisy20, LibSpeex, LibSpeexDSP)
Version used to generate this installer: NSIS SVN Revision 6228

*/
!Include "envvars.nsh"
!Include "winmessages.nsh"
; Tell the user that this is a test-only installer.
Function .onInit
MessageBox MB_OK "This is an alpha product. This means that the product has been tested, but you must expect some errors in the working code. These products are distributed for testing only and should not be used for mainline production yet."
FunctionEnd
; Set the Installer name - This is shown on the title bar
Name "ABF Products 2.0"
; Installer filename
OutFile "abf-installer.exe"
; Set the privileges for Windows Vista and 7
RequestExecutionLevel admin
; Ask the compiler to compress using LZMA
SetCompressor /solid lzma
InstallDir "$PROGRAMFILES\abf\"
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
WriteUninstaller "$ABFInstallDir\ABF-Uninstaller.exe"
FunctionEnd
; This function performs some after-installation setup
Function AfterFileCopying
${EnvVarUpdate} $0 "PATH" "P" HKCU $ABFInstallDir
; Create the uninstaller
Call CreateUninstaller
; After this, create some start menu icons.
CreateDirectory "$SMPROGRAMS\ABF 2.0 Products"
SetOutPath "$SMPROGRAMS\ABF 2.0 Products"
CreateShortCut "Documentation.lnk" "$ABFInstallDir\README.txt"
CreateShortCut "Launch Player.lnk" "$ABFInstallDir\abfplayer.exe"
CreateShortCut "Uninstall ABF Products.lnk" "$ABFInstallDir\ABFUninstaller.exe"
CreateShortCut "ABF Website.lnk" "http://tdlsoftware.net/abf"
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
File "..\bin\*.dll"
File "..\bin\abfconverter.exe"
File "abfplayer.exe"
;File "..\libdaisy20\libdaisy20.dll"
;File "..\libabf\libabf.dll"
File "Readme.txt"
Call AfterFileCopying
SectionEnd
Section /O "Experimental Converter"
SectionIn 2
StrCpy $ABFInstallDir $INSTDIR
SetOutPath $INSTDIR
;File "..\gui\converter\guiconverter.exe"
File "readme.txt"
File "..\bin\opusenc-win.exe"
File "abfplayer.exe"
File "..\bin\*.dll"
Call AfterFileCopying
CreateShortCut "$SMPROGRAMS\ABF Products\Experimental Graphical Converter.lnk" "$INSTDIR\guiconverter.exe"
SectionEnd
Section /O "Player Only"
SectionIn 3
StrCpy $ABFInstallDir $INSTDIR
SetOutPath $INSTDIR
File "..\bin\libabf.dll"
File "abfplayer.exe"
Call AfterFileCopying
SectionEnd
/*
The below function needs to be shared between the installer and uninstaller, since they cannot call each other's functions.

To accomplish this, we wrap the WinampPath function in a macro definition, insert the macro twice and we should have a desired result.
*/
!Macro SharedWinampPath un
Function ${un}WinampPath
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
!macroend
/* Define another macro: We need to close Winamp if the user chooses to install the Winamp plugin.
*/
!Macro SharedWinampClose un
Function ${un}CloseWinamp
Push $4
; The below loop executes until Winamp has been closed.
loop:
FindWindow $4 "Winamp v1.x"
IntCmp $4 0 done
MessageBox MB_ICONINFORMATION|MB_OK "Winamp is running. It needs to be closed before you can continue with the Winamp plugin installation."
SendMessage $4 16 0 0
Sleep 100
goto loop
done:
pop $4
FunctionEnd
!MacroEnd
!InsertMacro SharedWinampClose ""
!InsertMacro SharedWinampClose "un."
; Insert the SharedWinampPath macro twice
!InsertMacro SharedWinampPath ""
!InsertMacro SharedWinampPath "un."
; Try to install the Winamp Plugin if selected
Section /O "Winamp Plugin"
call CloseWinamp
Sleep 200
; We'll try to access some registry values.
CreateDirectory $INSTDIR
SectionIn 4
Call WinampPath
StrCpy $0 "$0\plugins"
SetOutPath $0
;File "..\winamp\in_abf.dll"
;File "..\..\speex-1.2rc1\bin\libspeex.dll"
; This section does not need to add or remove anything from the path, therefore, it needs only to create the uninstaller
Call CreateUninstaller
SectionEnd
Section "Uninstall"
Delete $INSTDIR\*.*
RmDir $INSTDIR
Delete "$SMPROGRAMS\ABF Products\*.*"
RmDir "$SMPROGRAMS\ABF Products"
Call un.WinampPath
Call Un.CloseWinamp
StrCpy $0 "$0\plugins"
Delete "$0\in_abf.dll"
pop $0
MessageBox MB_YESNO|MB_ICONQUESTION "Would you like to remove the database containing audio book positions? If you are to re-install, please answer 'No' here." /SD IDYES IDNO path
Delete "$PROFILE\.abfplayer.db"
path:
Call un.RestorePath
 SendMessage ${HWND_BROADCAST} ${WM_WININICHANGE} 0 "STR:Environment" /TIMEOUT=5000
SectionEnd
; A function only called from the uninstaller
; The path is restored as it was at installation time. This is "faulty" but it's the best I can do currently.
Function un.RestorePath
${un.EnvVarUpdate} $0 "PATH" "R" HKCU $ABFInstallDir
DeleteRegKey HKCR ".abf"
DeleteRegKey HKCR "ABFAudioBook"

System::Call "shell32::SHChangeNotify(i,i,i,i) (${SHCNE_ASSOCCHANGED}, ${SHCNF_FLUSH}, 0, 0)"
FunctionEnd
