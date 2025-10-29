[Setup]
OutputDir=.
OutputBaseFilename=TramsforPalmOS-1.0
AppName=Trams for Palm OS
AppVerName=Trams for Palm OS v1.0
AppPublisher=Schau.Com
AppPublisherURL=http://www.schau.com/
AppSupportURL=http://www.schau.com/
AppUpdatesURL=http://www.schau.com/
AppVersion=1.0
DefaultDirName={pf}\Schau.Com\Trams for Palm OS\v1.0
DefaultGroupName=Trams for Palm OS
DisableProgramGroupPage=yes
UsePreviousAppDir=no

[Files]
Source: "dependencies\CHANGELOG.txt"; DestDir: "{app}"; Flags: ignoreversion
Source: "dependencies\CondInst.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "dependencies\CondMgr.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "dependencies\Instaide.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "dependencies\README.txt"; DestDir: "{app}"; Flags: ignoreversion isreadme
Source: "prc\src\Trams.prc"; DestDir: "{app}"; Flags: ignoreversion
Source: "TramsConduit\release\TramsConduit.dll"; DestDir: "{app}"; Flags: ignoreversion; AfterInstall: GenTramsConduit(ExpandConstant('{app}'))
Source: "TramsCvt\TramsCvt\bin\Release\TramsCvt.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "TramsInst\bin\Release\TramsInst.exe"; DestDir: "{app}"; Flags: ignoreversion

[Icons]
Name: "{group}\ChangeLog"; Filename: "{app}\CHANGELOG.txt"
Name: "{group}\Install Prc"; Filename: "{app}\TramsInst.exe"; WorkingDir: "{app}"
Name: "{group}\Read Me"; Filename: "{app}\README.txt"
Name: "{group}\TramsCvt"; Filename: "{app}\TramsCvt.exe"; WorkingDir: "{app}"
Name: "{group}\Uninstall"; Filename: "{app}\unins000.exe"; WorkingDir: "{app}"

[Run]
Filename: "{app}\CondInst.exe"; Parameters: "/i ""{app}\TramsConduit.txt"""; WorkingDir: "{app}"; StatusMsg: "Registering Conduit ..."
Filename: "{app}\TramsInst.exe"; Parameters: "/oneshot"; WorkingDir: "{app}"; StatusMsg: "Queueing Trams for Palm OS to be delivered on next HotSync ..."

[UninstallRun]
Filename: "{app}\CondInst.exe"; Parameters: "/u ""{app}\TramsConduit.txt"""; WorkingDir: "{app}"; StatusMsg: "Removing Conduit ..."

[UninstallDelete]
Type: files; Name: "{app}\TramsConduit.txt"

[Code]
procedure GenTramsConduit(dir: String);
var
	file: String;
begin
	file:=dir+'\TramsConduit.txt';

	SaveStringToFile(file, 'CreatorID Trms'+#13#10, False);
	SaveStringToFile(file, 'Directory Trams'+#13#10, True);
	SaveStringToFile(file, 'Description Trams Conduit'+#13#10, True);
	SaveStringToFile(file, 'Dll '+dir+'\TramsConduit.dll'+#13#10, True);
	SaveStringToFile(file, 'Name Trams'+#13#10, True);
end;
