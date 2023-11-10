[Setup]
OutputDir=.
OutputBaseFilename=Diploma
AppName=Diploma
AppVerName=Diploma v1r1
AppPublisher=Schau.Com
AppPublisherURL=http://www.schau.com/s/dictate/index.html
AppSupportURL=http://www.schau.com/s/dictate/index.html
AppUpdatesURL=http://www.schau.com/s/dictate/index.html
AppVersion=1r1
DefaultDirName={pf}\Schau.Com\Dictate\Diploma\v1r1
DefaultGroupName=Diploma
DisableProgramGroupPage=yes
UsePreviousAppDir=no

[Types]
Name: "full"; Description: "Install Diploma"

[Components]
Name: "main"; Description: "Main Files"; Types: full; Flags: fixed

[Files]
Source: "Docs\CHANGELOG.txt"; DestDir: "{app}"; Components: main; Flags: ignoreversion
Source: "Docs\DIPLOMAS.txt"; DestDir: "{app}"; Components: main; Flags: ignoreversion
Source: "Docs\LICENSE.txt"; DestDir: "{app}"; Components: main; Flags: ignoreversion
Source: "Docs\README.txt"; DestDir: "{app}"; Components: main; Flags: isreadme ignoreversion
Source: "Files\Standard Diploma.htm"; DestDir: "{app}"; Components: main; Flags: ignoreversion
Source: "Files\diploma.gif"; DestDir: "{app}"; Components: main; Flags: ignoreversion;
Source: "Diploma\Release\Diploma.exe"; DestDir: "{app}"; Components: main; Flags: ignoreversion

[Icons]
Name: "{group}\Changelog"; Filename: "{app}\CHANGELOG.txt"
Name: "{group}\License"; Filename: "{app}\LICENSE.txt"
Name: "{group}\Read Me"; Filename: "{app}\README.txt"
Name: "{group}\How to create your own Diploma"; Filename: "{app}\DIPLOMAS.txt"
Name: "{group}\Diploma"; Filename: "{app}\Diploma.exe"

[Run]
Filename: "{app}\Diploma.exe"; Description: "Launch Diploma"; Flags: postinstall nowait skipifsilent unchecked

[Registry]
Root: HKLM; Subkey: "SOFTWARE\Schau.Com\Dictate\Diploma"; ValueType: string; ValueName: "InstPath"; ValueData: "{app}"; Flags: uninsdeletekey
