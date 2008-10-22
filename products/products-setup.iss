; Script generated by the Inno Setup Script Wizard.
; SEE THE DOCUMENTATION FOR DETAILS ON CREATING INNO SETUP SCRIPT FILES!

[Setup]
; NOTE: The value of AppId uniquely identifies this application.
; Do not use the same AppId value in installers for other applications.
; (To generate a new GUID, click Tools | Generate GUID inside the IDE.)
AppId={{2234610C-0AAA-4079-92AA-67EDF171E4B5}
AppName=ABF Products
AppVerName=ABF Products 0.2
AppPublisher=ABF Project
AppPublisherURL=http://mosedal.net/abf
AppSupportURL=http://mosedal.net/abf
AppUpdatesURL=http://mosedal.net/abf
DefaultDirName={pf}\ABF\Products
DefaultGroupName=ABF\ABF Products
OutputDir=D:\svn\abf\code\trunk\products
OutputBaseFilename=Products-0.2-win32-setup
LicenseFile=d:\svn\abf\code\trunk\products\license.txt
InfoBeforeFile=d:\svn\abf\code\trunk\products\readme.txt
Compression=lzma
SolidCompression=yes
ChangesAssociations=yes
[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Files]
Source: "D:\svn\abf\code\trunk\products\audiere.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "D:\svn\abf\code\trunk\products\converter.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "D:\svn\abf\code\trunk\products\libdaisy.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "D:\svn\abf\code\trunk\products\libspeexdsp.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "D:\svn\abf\code\trunk\products\player.exe"; DestDir: "{app}"; Flags: ignoreversion
        Source: "d:\svn\abf\code\trunk\products\readme.txt"; DestDir: "{app}"; flags: isreadme
; NOTE: Don't use "Flags: ignoreversion" on any shared system files

[Icons]
Name: "{group}\{cm:ProgramOnTheWeb,Products}"; Filename: "http://mosedal.net/abf"
Name: "{group}\{cm:UninstallProgram,Products}"; Filename: "{uninstallexe}"
        Name: "{group}\View README.TXT"; Filename: "{app}\readme.txt";
                       [Registry]
Root: HKCR; Subkey: ".abf"; ValueType: string; ValueName: ""; ValueData: "ABFAudioBook"; Flags: uninsdeletevalue
 Root: HKCR; Subkey: "ABFAudioBook"; ValueType: string; ValueName: ""; ValueData: "ABF Audio Book"; Flags: uninsdeletekey
Root: HKCR; Subkey: "ABFAudioBook\shell\open\command"; ValueType: string;ValueName: ""; ValueData: """{app}\PLAYER.EXE"" ""%1"""
                                                        Root: HKCU; Subkey: "Environment"; ValueType: string; ValueName: "Path"; ValueData: """{app}"""; Flags: uninsdeletekeyifempty
                                                        
