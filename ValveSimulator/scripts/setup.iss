; ============================================================================
; ValveSimulator — Inno Setup 6 安装脚本 (Release)
; ============================================================================
; 使用方法:
;   1. 运行 build_release.bat 编译 Release 版 + windeployqt
;   2. 用 Inno Setup 6 编译此脚本生成安装包
; ============================================================================

#define MyAppName       "虚拟阀门模拟器"
#define MyAppVersion    "1.0.0"
#define MyAppPublisher  "ValveSim"
#define MyAppExeName    "ValveSimulator.exe"
; NOTE: Paths are relative to this .iss file location (scripts/)
#define MySourceDir     "..\build_rel"
#define MyIconFile      "..\resources\app_icon.ico"

[Setup]
AppId={{B8F4A3D2-7E1C-4A9F-B5D8-2C6E1F3A8D04}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
AppPublisher={#MyAppPublisher}
DefaultDirName={autopf}\{#MyAppName}
DefaultGroupName={#MyAppName}
OutputBaseFilename=ValveSimulator_Setup_v{#MyAppVersion}
OutputDir=..\installer
SetupIconFile={#MyIconFile}
Compression=lzma2/ultra64
SolidCompression=yes
PrivilegesRequired=admin
WizardStyle=modern
ArchitecturesInstallIn64BitMode=x64compatible
ArchitecturesAllowed=x64compatible

[Languages]
Name: "chinese"; MessagesFile: "compiler:Languages\ChineseSimplified.isl"

[Tasks]
Name: "desktopicon"; Description: "创建桌面快捷方式"; GroupDescription: "附加图标:"; Flags: checkedonce

[Files]
; 主程序
Source: "{#MySourceDir}\{#MyAppExeName}"; DestDir: "{app}"; Flags: ignoreversion

; Qt Release DLL (无 d 后缀)
Source: "{#MySourceDir}\Qt6Core.dll";          DestDir: "{app}"; Flags: ignoreversion
Source: "{#MySourceDir}\Qt6Gui.dll";           DestDir: "{app}"; Flags: ignoreversion
Source: "{#MySourceDir}\Qt6Widgets.dll";       DestDir: "{app}"; Flags: ignoreversion
Source: "{#MySourceDir}\Qt6Network.dll";       DestDir: "{app}"; Flags: ignoreversion
Source: "{#MySourceDir}\Qt6Svg.dll";           DestDir: "{app}"; Flags: ignoreversion

; 图形运行时
Source: "{#MySourceDir}\d3dcompiler_47.dll";   DestDir: "{app}"; Flags: ignoreversion
Source: "{#MySourceDir}\dxcompiler.dll";       DestDir: "{app}"; Flags: ignoreversion
Source: "{#MySourceDir}\dxil.dll";             DestDir: "{app}"; Flags: ignoreversion
Source: "{#MySourceDir}\opengl32sw.dll";       DestDir: "{app}"; Flags: ignoreversion

; VC++ 可再发行组件（安装时自动运行）
Source: "{#MySourceDir}\vc_redist.x64.exe";    DestDir: "{tmp}"; Flags: deleteafterinstall

; Qt 插件目录
Source: "{#MySourceDir}\platforms\*";          DestDir: "{app}\platforms";          Flags: ignoreversion recursesubdirs
Source: "{#MySourceDir}\styles\*";             DestDir: "{app}\styles";             Flags: ignoreversion recursesubdirs
Source: "{#MySourceDir}\imageformats\*";       DestDir: "{app}\imageformats";       Flags: ignoreversion recursesubdirs
Source: "{#MySourceDir}\iconengines\*";        DestDir: "{app}\iconengines";        Flags: ignoreversion recursesubdirs
Source: "{#MySourceDir}\generic\*";            DestDir: "{app}\generic";            Flags: ignoreversion recursesubdirs
Source: "{#MySourceDir}\networkinformation\*"; DestDir: "{app}\networkinformation"; Flags: ignoreversion recursesubdirs
Source: "{#MySourceDir}\tls\*";                DestDir: "{app}\tls";                Flags: ignoreversion recursesubdirs

[Icons]
Name: "{group}\{#MyAppName}";                 Filename: "{app}\{#MyAppExeName}"
Name: "{group}\卸载 {#MyAppName}";             Filename: "{uninstallexe}"
Name: "{autodesktop}\{#MyAppName}";            Filename: "{app}\{#MyAppExeName}"; Tasks: desktopicon

[Run]
; 先静默安装 VC++ 运行时（如已安装则跳过）
Filename: "{tmp}\vc_redist.x64.exe"; Parameters: "/quiet /norestart"; StatusMsg: "正在安装 VC++ 运行时..."; Flags: waituntilterminated
; 安装完成后自动运行程序
Filename: "{app}\{#MyAppExeName}"; Description: "启动 {#MyAppName}"; Flags: nowait postinstall skipifsilent

[UninstallDelete]
Type: dirifempty; Name: "{app}\platforms"
Type: dirifempty; Name: "{app}\styles"
Type: dirifempty; Name: "{app}\imageformats"
Type: dirifempty; Name: "{app}\iconengines"
Type: dirifempty; Name: "{app}\generic"
Type: dirifempty; Name: "{app}\networkinformation"
Type: dirifempty; Name: "{app}\tls"
