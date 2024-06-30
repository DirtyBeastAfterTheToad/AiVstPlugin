; Script generated by the Inno Setup Script Wizard.
; SEE THE DOCUMENTATION FOR DETAILS ON CREATING INNO SETUP SCRIPT FILES!

[Setup]
; NOTE: The value of AppId uniquely identifies this application. Do not use the same AppId value in installers for other applications.
; (To generate a new GUID, click Tools | Generate GUID inside the IDE.)
AppId={{05682ADB-8C00-4955-B436-98889BDACCA2}
AppName=AiSamplerVST
AppVersion=0.1
;AppVerName=AiSamplerVST 0.1
AppPublisher=DBAT
DefaultDirName={autopf}\AiSamplerVST
DefaultGroupName=AiSamplerVST
DisableProgramGroupPage=yes
; Uncomment the following line to run in non administrative install mode (install for current user only.)
;PrivilegesRequired=lowest
PrivilegesRequiredOverridesAllowed=dialog
OutputBaseFilename=AiVSTSetup
Compression=lzma
SolidCompression=yes
WizardStyle=modern

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Files]
Source: "C:\Users\ryanl\JucerProjects\NewProject\Builds\VisualStudio2022\x64\Debug\VST3\AiPlugin.vst3\Contents\x86_64-win\AiPlugin.vst3"; DestDir: "{app}"; Flags: ignoreversion
Source: "C:\Users\ryanl\JucerProjects\NewProject\Source\python_embed"; DestDir: "{app}\python"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "C:\Users\ryanl\JucerProjects\NewProject\Source\setup.py"; DestDir: "{app}"; Flags: ignoreversion
; NOTE: Don't use "Flags: ignoreversion" on any shared system files

[Run]
; Unzip Python and install dependencies
Filename: "{app}\python\python.exe"; Parameters: "{app}\setup.py install"; WorkingDir: "{app}"; Flags: runhidden