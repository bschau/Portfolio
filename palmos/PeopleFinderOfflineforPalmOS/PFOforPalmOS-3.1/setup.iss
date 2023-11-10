[Setup]
OutputDir=.
OutputBaseFilename=PFOforPalmOS-3.1
AppName=PeopleFinder Offline for Palm OS
AppVerName=PeopleFinder Offline for Palm OS v3.1
AppPublisher=Schau.Com
AppPublisherURL=http://www.schau.com/
AppSupportURL=http://www.schau.com/
AppUpdatesURL=http://www.schau.com/
AppVersion=3.1
DefaultDirName={pf}\Schau.Com\PeopleFinder Offline for Palm OS\v3.1
DefaultGroupName=PeopleFinder Offline for Palm OS
DisableProgramGroupPage=yes
UsePreviousAppDir=no

[Files]
Source: "pfoc\pfoc\bin\Release\pfoc.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "dependencies\ICSharpCode.SharpZipLib.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "dependencies\Instaide.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "prc\src\PFO.prc"; DestDir: "{app}"; Flags: ignoreversion
Source: "pfoi\pfoi\bin\Release\pfoi.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "dependencies\README.txt"; DestDir: "{app}"; Flags: ignoreversion isreadme
Source: "dependencies\CHANGELOG.txt"; DestDir: "{app}"; Flags: ignoreversion

[Icons]
Name: "{group}\Converter"; Filename: "{app}\pfoc.exe"; WorkingDir: "{app}"
Name: "{group}\PFO Installer"; Filename: "{app}\pfoi.exe"; WorkingDir: "{app}"
Name: "{group}\Read Me"; Filename: "{app}\README.txt"
Name: "{group}\ChangeLog"; Filename: "{app}\CHANGELOG.txt"
Name: "{group}\Uninstall PFO"; Filename: "{app}\unins000.exe"; WorkingDir: "{app}"

[Run]
Filename: "{app}\pfoi.exe"; Parameters: "/oneshot"; WorkingDir: "{app}"; StatusMsg: "Queueing PFO for HotSync ..."

[UninstallDelete]
Type: dirifempty; Name: "{app}\HP\PeopleFinder Offline for Palm OS\v3.1"
Type: dirifempty; Name: "{app}\HP\PeopleFinder Offline for Palm OS"
Type: dirifempty; Name: "{app}\HP"

