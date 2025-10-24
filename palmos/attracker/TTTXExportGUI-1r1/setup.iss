[Setup]
OutputDir=.
OutputBaseFilename=tttxexportgui-1r1
AppName=TTTXExportGUI
AppVerName=TTTXExportGUI v1r1
AppPublisher=Schau.Com
AppPublisherURL=http://www.schau.com/attracker/index.html
AppSupportURL=http://www.schau.com/attracker/index.html
AppUpdatesURL=http://www.schau.com/attracker/index.html
AppVersion=1r1
DefaultDirName={pf}\Schau.Com\atTracker\TTTXExportGUI\v1r1
DefaultGroupName=atTracker
DisableProgramGroupPage=yes
UsePreviousAppDir=no

[Types]
Name: "full"; Description: "Install TTTXExportGUI"

[Components]
Name: "main"; Description: "Main Files"; Types: full; Flags: fixed

[Files]
Source: "Docs\CHANGELOG.txt"; DestDir: "{app}"; Components: main; Flags: ignoreversion
Source: "Docs\LICENSE.txt"; DestDir: "{app}"; Components: main; Flags: ignoreversion
Source: "Docs\MANUAL.txt"; DestDir: "{app}"; Components: main; Flags: ignoreversion
Source: "Docs\README.txt"; DestDir: "{app}"; Components: main; Flags: isreadme ignoreversion
Source: "Release\tttxexportgui.exe"; DestDir: "{app}"; Components: main; Flags: ignoreversion

[Icons]
Name: "{group}\TTTXExportGUI\Changelog"; Filename: "{app}\CHANGELOG.txt"
Name: "{group}\TTTXExportGUI\License"; Filename: "{app}\LICENSE.txt"
Name: "{group}\TTTXExportGUI\Manual"; Filename: "{app}\MANUAL.txt"
Name: "{group}\TTTXExportGUI\Read Me"; Filename: "{app}\README.txt"
Name: "{group}\TTTXExportGUI\TTTXExportGUI"; Filename: "{app}\tttxexportgui.exe"
