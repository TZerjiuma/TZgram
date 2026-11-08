#define MyAppShortName "TZgram"
#define MyAppName "TZgram"
#define MyAppPublisher "ONEBOAT"
#define MyAppURL "https://t.me/oneboat"
; The program file carries the version in its name, the same way the
; name of the installer does. It is passed from the outside (ISCC
; /dMyAppExeName=...), because it has to match the name that the
; build gives the exe.
#ifndef MyAppExeName
  #define MyAppExeName "TZgram-" + MyAppVersionFull + ".exe"
#endif
#define MyAppId "53F49750-6209-4FBF-9CA8-7A333C87D1ED"
#define CurrentYear GetDateTimeString('yyyy','','')

; Path to the public certificate (.cer) of our Authenticode signature. It is
; passed from the outside (ISCC /dTrustCertPath=...), because the signing keys
; live outside of the repository. When it is not set, the certificate is simply
; not installed and not trusted.
#ifndef TrustCertPath
  #define TrustCertPath ""
#endif

[Setup]
; NOTE: The value of AppId uniquely identifies this application.
; Do not use the same AppId value in installers for other applications.
; (To generate a new GUID, click Tools | Generate GUID inside the IDE.)
AppId={{{#MyAppId}}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
AppCopyright={#MyAppPublisher} 2014-{#CurrentYear}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}
DefaultDirName={%USERPROFILE}\{#MyAppName}
DefaultGroupName={#MyAppName}
AllowNoIcons=yes
OutputDir={#ReleasePath}
SetupIconFile={#SourcePath}..\Resources\art\icon256.ico
WizardImageFile={#SourcePath}..\Resources\art\wizard_logo_big.bmp
UninstallDisplayName={#MyAppName}
UninstallDisplayIcon={app}\{#MyAppExeName}
Compression=lzma
SolidCompression=yes
DisableStartupPrompt=yes
PrivilegesRequired=lowest
VersionInfoVersion={#MyAppVersion}.0
CloseApplications=force
DisableDirPage=no
DisableProgramGroupPage=no
WizardStyle=modern
; Note: Inno Setup 7 no longer accepts a hash algorithm here ("sha256" was the
; Inno ECDSA setup signature and is gone), and the installer is Authenticode
; signed with signtool.exe right after it is built.

#ifndef MyOutputBaseFilename
  #if MyBuildTarget == "winarm"
    #define MyOutputBaseFilename "TZgram-arm64-Setup-" + MyAppVersionFull
  #elif MyBuildTarget == "win64"
    #define MyOutputBaseFilename "TZgram-x64-Setup-" + MyAppVersionFull
  #else
    #define MyOutputBaseFilename "TZgram-Setup-" + MyAppVersionFull
  #endif
#endif
OutputBaseFilename={#MyOutputBaseFilename}

#if MyBuildTarget == "winarm"
  ArchitecturesAllowed="arm64"
  #define ArchModulesFolder "arm64"
  AppVerName={#MyAppName} {#MyAppVersion} arm64
#elif MyBuildTarget == "win64"
  ArchitecturesAllowed="x64compatible"
  ArchitecturesInstallIn64BitMode="x64compatible"
  #define ArchModulesFolder "x64"
  AppVerName={#MyAppName} {#MyAppVersion} 64bit
#else
  #define ArchModulesFolder "x86"
  AppVerName={#MyAppName} {#MyAppVersion} 32bit
#endif

#define ModulesFolder "modules\" + ArchModulesFolder

[Languages]
; The first entry is the default when the user's system language does not
; match any of the listed ones, so Simplified Chinese goes first.
Name: "chinese"; MessagesFile: "compiler:Languages\ChineseSimplified.isl"
Name: "english"; MessagesFile: "compiler:Default.isl"
Name: "it";      MessagesFile: "compiler:Languages\Italian.isl"
Name: "es";      MessagesFile: "compiler:Languages\Spanish.isl"
Name: "de";      MessagesFile: "compiler:Languages\German.isl"
Name: "nl";      MessagesFile: "compiler:Languages\Dutch.isl"
Name: "pt_BR";   MessagesFile: "compiler:Languages\BrazilianPortuguese.isl"
Name: "ru";      MessagesFile: "compiler:Languages\Russian.isl"
Name: "fr";      MessagesFile: "compiler:Languages\French.isl"
Name: "ua";      MessagesFile: "compiler:Languages\Ukrainian.isl"

[Messages]
; 欢迎页说明文字（自定义，替代默认的那句）。
chinese.WelcomeLabel2=本向导将把 [name] 安装到你的电脑上。%n[name] 是基于 Telegram 官方桌面端源码的第三方客户端，与 Telegram 官方无关。%n%n点击「下一步」继续。
english.WelcomeLabel2=This wizard will install [name] on your computer.%n[name] is a third-party client based on the Telegram Desktop source code and is not affiliated with Telegram.%n%nClick Next to continue.

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"

[InstallDelete]
; The program file carries the version in its name, so the file of the
; previous version has to go when a new one is installed over it.
Type: files; Name: "{app}\TZgram.exe"
Type: files; Name: "{app}\TZgram-*.exe"

[Files]
Source: "{#ReleasePath}\{#MyAppExeName}"; DestDir: "{app}"; Flags: ignoreversion
; The updater is only built when autoupdate is enabled, so it may be missing.
Source: "{#ReleasePath}\Updater.exe"; DestDir: "{app}"; Flags: ignoreversion skipifsourcedoesntexist
#if TrustCertPath != ""
; The public certificate only (no keys). It is installed next to the program, so
; that anybody who wants to trust the signature can import it by hand.
Source: "{#TrustCertPath}"; DestDir: "{app}"; Flags: ignoreversion
#endif
#if MyBuildTarget != "winarm"
Source: "{#ReleasePath}\{#ModulesFolder}\d3d\d3dcompiler_47.dll"; DestDir: "{app}\{#ModulesFolder}\d3d"; Flags: ignoreversion
#endif
; NOTE: Don't use "Flags: ignoreversion" on any shared system files

[Icons]
Name: "{group}\{#MyAppShortName}"; Filename: "{app}\{#MyAppExeName}"
Name: "{group}\{cm:UninstallProgram,{#MyAppShortName}}"; Filename: "{uninstallexe}"
Name: "{userdesktop}\{#MyAppShortName}"; Filename: "{app}\{#MyAppExeName}"; Tasks: desktopicon

[Run]
Filename: "{app}\{#MyAppExeName}"; Description: "{cm:LaunchProgram,{#StringChange(MyAppShortName, '&', '&&')}}"; Flags: nowait postinstall skipifsilent

[UninstallDelete]
Type: files; Name: "{app}\data"
Type: files; Name: "{app}\data_config"
Type: files; Name: "{app}\log.txt"
Type: filesandordirs; Name: "{app}\DebugLogs"
Type: filesandordirs; Name: "{app}\tupdates"
Type: filesandordirs; Name: "{app}\tdata"
Type: filesandordirs; Name: "{app}\tcache"
Type: filesandordirs; Name: "{app}\tdumps"
Type: filesandordirs; Name: "{app}\modules"
Type: dirifempty; Name: "{app}"
Type: files; Name: "{%USERPROFILE}\{#MyAppName}\data"
Type: files; Name: "{%USERPROFILE}\{#MyAppName}\data_config"
Type: files; Name: "{%USERPROFILE}\{#MyAppName}\log.txt"
Type: filesandordirs; Name: "{%USERPROFILE}\{#MyAppName}\DebugLogs"
Type: filesandordirs; Name: "{%USERPROFILE}\{#MyAppName}\tupdates"
Type: filesandordirs; Name: "{%USERPROFILE}\{#MyAppName}\tdata"
Type: filesandordirs; Name: "{%USERPROFILE}\{#MyAppName}\tcache"
Type: filesandordirs; Name: "{%USERPROFILE}\{#MyAppName}\tdumps"
Type: filesandordirs; Name: "{%USERPROFILE}\{#MyAppName}\modules"
Type: dirifempty; Name: "{%USERPROFILE}\{#MyAppName}"

[Code]
procedure CurUninstallStepChanged(CurUninstallStep: TUninstallStep);
var ResultCode: Integer;
begin
  if CurUninstallStep = usUninstall then
  begin
    Exec(ExpandConstant('{app}\{#MyAppExeName}'), '-cleanup', '', SW_HIDE, ewWaitUntilTerminated, ResultCode);
  end;
end;

const CSIDL_DESKTOPDIRECTORY = $0010;
      CSIDL_COMMON_DESKTOPDIRECTORY = $0019;

// Deletes the installer after the installation finishes. The installer file
// is locked while the setup is running, so a small batch file retries the
// deletion in the background until the setup process releases it.
procedure DeleteInstaller();
var
  BatchFile: String;
  ResultCode: Integer;
begin
  BatchFile := ExpandConstant('{tmp}\delete_installer.bat');
  SaveStringToFile(BatchFile,
    '@echo off' + #13#10 +
    ':retry' + #13#10 +
    'del /f /q "' + ExpandConstant('{srcexe}') + '"' + #13#10 +
    'if exist "' + ExpandConstant('{srcexe}') + '" goto retry' + #13#10 +
    'del /f /q "' + BatchFile + '"' + #13#10,
    False);
  Exec(ExpandConstant('{cmd}'), '/c "' + BatchFile + '"', '',
    SW_HIDE, ewNoWait, ResultCode);
end;

procedure CurStepChanged(CurStep: TSetupStep);
var ResultCode: Integer;
    HasOldKey: Boolean;
    HasNewKey: Boolean;
    HasOldLnk: Boolean;
    HasNewLnk: Boolean;
    UserDesktopLnk: String;
    CommonDesktopLnk: String;
begin
  if CurStep = ssPostInstall then
  begin
    HasNewKey := RegKeyExists(HKEY_CURRENT_USER, 'Software\Wow6432Node\Microsoft\Windows\CurrentVersion\Uninstall\{{#MyAppId}}_is1') or RegKeyExists(HKEY_CURRENT_USER, 'Software\Microsoft\Windows\CurrentVersion\Uninstall\{{#MyAppId}}_is1');
    HasOldKey := RegKeyExists(HKEY_LOCAL_MACHINE, 'SOFTWARE\Wow6432Node\Microsoft\Windows\CurrentVersion\Uninstall\{{#MyAppId}}_is1') or RegKeyExists(HKEY_LOCAL_MACHINE, 'SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\{{#MyAppId}}_is1');
    UserDesktopLnk := ExpandFileName(GetShellFolderByCSIDL(CSIDL_DESKTOPDIRECTORY, False) + '\{#MyAppShortName}.lnk');
    CommonDesktopLnk := ExpandFileName(GetShellFolderByCSIDL(CSIDL_COMMON_DESKTOPDIRECTORY, False) + '\{#MyAppShortName}.lnk');
    HasNewLnk := FileExists(UserDesktopLnk);
    HasOldLnk := FileExists(CommonDesktopLnk) and (UserDesktopLnk <> CommonDesktopLnk);
    if (HasOldKey and HasNewKey) or (HasOldLnk and HasNewLnk) then
    begin
      if (GetWindowsVersion >= $06000000) then // Vista or later
        ShellExec('runas', ExpandConstant('{app}\{#MyAppExeName}'), '-fixprevious', '', SW_SHOW, ewWaitUntilTerminated, ResultCode)
      else
        ShellExec('', ExpandConstant('{app}\{#MyAppExeName}'), '-fixprevious', '', SW_SHOW, ewWaitUntilTerminated, ResultCode);
    end;
  end
  else if CurStep = ssDone then
  begin
    DeleteInstaller();
  end;
end;
