[Setup]
OutputDir=.
OutputBaseFilename=BMILogConduit-1r1
AppName=BMILog Conduit
AppVerName=BMILog Conduit 1r1
AppPublisher=Schau.Com
AppPublisherURL=http://www.schau.com/bmilog/index.html
AppSupportURL=http://www.schau.com/bmilog/index.html
AppUpdatesURL=http://www.schau.com/bmilog/index.html
AppVersion=1r1
DefaultDirName={pf}\Schau.Com\BMILog
DefaultGroupName=BMILog
DisableProgramGroupPage=yes

[Files]
Source: "1r1\CHANGELOG.txt"; DestDir: "{app}"
Source: "1r1\LICENSE.txt"; DestDir: "{app}"
Source: "1r1\README.txt"; DestDir: "{app}"; Flags: isreadme
Source: "1r1\BMILog\release\BMILog10.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "1r1\PalmConduitInstall\release\CondInst.dll"; DestDir: "{app}"
Source: "1r1\PalmConduitInstall\release\PCI.exe"; DestDir: "{app}"
Source: "C:\cdk403\common\bin\condmgr.dll"; DestDir: "{app}"
Source: "C:\cdk403\common\bin\HSAPI.dll"; DestDir: "{app}"
; NOTE: Don't use "Flags: ignoreversion" on any shared system files

[Icons]
Name: "{group}\Changelog"; Filename: "{app}\CHANGELOG.txt"
Name: "{group}\License"; Filename: "{app}\LICENSE.txt"
Name: "{group}\Read Me"; Filename: "{app}\README.txt"

[Run]
Filename: "{app}\pci.exe"; Parameters: "/o ""{app}"" ""BgSc"" ""{app}"" ""BMILog10.dll"" ""BMILog"" ""BgSc_BMILogData.dat"" ""BgSc_BMILogData"" ""BMILog (PCI)"" ""2"" ""Installed by PCI and InnoSetup"""; Flags: runminimized

[UninstallRun]
Filename: "{app}\pci.exe"; Parameters: "/u ""{app}"" ""BgSc"""; Flags: runminimized
