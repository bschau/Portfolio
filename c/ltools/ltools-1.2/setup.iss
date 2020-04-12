[Setup]
OutputDir=.
OutputBaseFilename=ltools-1.2
AppName=ltools
AppVerName=ltools-1.2
AppPublisher=Schau.Com
AppPublisherURL=http://www.schau.com/
AppSupportURL=http://www.schau.com/
AppUpdatesURL=http://www.schau.com/
AppVersion=1.2
DefaultDirName={pf}\Schau.Com\ltools
DefaultGroupName=ltools
DisableProgramGroupPage=yes
UsePreviousAppDir=no

[Files]
Source: "lcdoc\lcdoc.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "lhd\lhd.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "lmake\lmake.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "lme\lme.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "docs\README.txt"; DestDir: "{app}"; Flags: ignoreversion isreadme
Source: "docs\CHANGELOG.txt"; DestDir: "{app}"; Flags: ignoreversion
Source: "docs\LICENSE.txt"; DestDir: "{app}"; Flags: ignoreversion

[Icons]
Name: "{group}\Read Me"; Filename: "{app}\README.txt"
Name: "{group}\ChangeLog"; Filename: "{app}\CHANGELOG.txt"
Name: "{group}\License"; Filename: "{app}\LICENSE.txt"
Name: "{group}\Uninstall ltools"; Filename: "{app}\unins000.exe"; WorkingDir: "{app}"

[UninstallDelete]
Type: dirifempty; Name: "{app}\Schau.Com\ltools"
Type: dirifempty; Name: "{app}\Schau.Com"

