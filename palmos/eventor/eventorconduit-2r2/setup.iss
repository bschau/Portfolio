[Setup]
OutputDir=.
OutputBaseFilename=EventorConduit-2r2
AppName=Eventor Conduit
AppVerName=Eventor Conduit 2r2
AppPublisher=Schau.Com
AppPublisherURL=http://www.schau.com/eventor/index.html
AppSupportURL=http://www.schau.com/eventor/index.html
AppUpdatesURL=http://www.schau.com/eventor/index.html
AppVersion=2r2
DefaultDirName={pf}\Schau.Com\Eventor\v2r2
DefaultGroupName=Eventor
DisableProgramGroupPage=yes

[Files]
Source: "2r2\CHANGELOG.txt"; DestDir: "{app}"
Source: "2r2\LICENSE.txt"; DestDir: "{app}"
Source: "2r2\README.txt"; DestDir: "{app}"; Flags: isreadme
Source: "2r2\eventor\release\Eventor22.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "2r2\palmconduitinstall\release\CondInst.dll"; DestDir: "{app}"
Source: "2r2\palmconduitinstall\release\PCI.exe"; DestDir: "{app}"
Source: "C:\cdk403\common\bin\condmgr.dll"; DestDir: "{app}"
Source: "C:\cdk403\common\bin\HSAPI.dll"; DestDir: "{app}"
; NOTE: Don't use "Flags: ignoreversion" on any shared system files

[Icons]
Name: "{group}\Changelog"; Filename: "{app}\CHANGELOG.txt"
Name: "{group}\License"; Filename: "{app}\LICENSE.txt"
Name: "{group}\Read Me"; Filename: "{app}\README.txt"

[Run]
Filename: "{app}\pci.exe"; Parameters: "/o ""{app}"" ""PERV"" ""{app}"" ""Eventor22.dll"" ""Eventor"" ""EventorData.dat"" ""PERV_EventorData"" ""Eventor (PCI)"" ""2"" ""Installed by PCI and InnoSetup"""; Flags: runminimized

[UninstallRun]
Filename: "{app}\pci.exe"; Parameters: "/u ""{app}"" ""PERV"""; Flags: runminimized
