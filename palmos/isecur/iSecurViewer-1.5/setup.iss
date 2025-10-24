[Setup]
OutputDir=.
OutputBaseFilename=iSecurViewer-1.5
AppName=iSecurViewer
AppVerName=iSecurViewer v1.5
AppPublisher=Schau.Com
AppPublisherURL=http://www.schau.com/
AppSupportURL=http://www.schau.com/
AppUpdatesURL=http://www.schau.com/
AppVersion=1.5
DefaultDirName={pf}\Schau.Com\iSecurViewer\v1.5
DefaultGroupName=iSecur
DisableProgramGroupPage=yes
UsePreviousAppDir=no

[Files]
Source: "iSecurViewer\bin\Release\iSecurViewer.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "dependencies\README.txt"; DestDir: "{app}"; Flags: ignoreversion isreadme
Source: "dependencies\CHANGELOG.txt"; DestDir: "{app}"; Flags: ignoreversion
Source: "dependencies\Instaide.dll"; DestDir: "{app}"; Flags: ignoreversion

[Icons]
Name: "{group}\iSecurViewer"; Filename: "{app}\iSecurViewer.exe"; WorkingDir: "{app}"
Name: "{group}\iSecurViewer - Read Me"; Filename: "{app}\README.txt"
Name: "{group}\iSecurViewer - ChangeLog"; Filename: "{app}\CHANGELOG.txt"
Name: "{group}\Uninstall iSecurViewer"; Filename: "{app}\unins000.exe"; WorkingDir: "{app}"

[UninstallDelete]
Type: dirifempty; Name: "{app}\Schau.Com\iSecurViewer\v1.5"
Type: dirifempty; Name: "{app}\Schau.Com\iSecurViewer"
Type: dirifempty; Name: "{app}\Schau.Com"

