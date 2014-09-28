; splot.nsi
;--------------------------------

; Includes
;!include "FileFunc.nsh"
;--------------------------------

!searchparse /file "..\configure.ac" `AC_INIT([` PROGRAM `], [SPLOT_VERSION], [http://github/esohns/splot],[splot])`

; Languages
LoadLanguageFile "${NSISDIR}\Contrib\Language files\English.nlf"

; The name of the installer
Name ${PROGRAM}

; The file to write
!searchparse /file "..\configure.ac" `AC_SUBST([SPLOT_VERSION], [` VER_MAJOR `.` VER_MINOR `])`
VIAddVersionKey /LANG=${LANG_ENGLISH} "ProductName" ${PROGRAM}
VIAddVersionKey /LANG=${LANG_ENGLISH} "Comments" "a top-down arcade shooter"
;VIAddVersionKey /LANG=${LANG_ENGLISH} "CompanyName" ""
!define /date NOW "%Y"
VIAddVersionKey /LANG=${LANG_ENGLISH} "LegalCopyright" "© ${NOW}"
VIAddVersionKey /LANG=${LANG_ENGLISH} "FileDescription" "${PROGRAM} installer"
VIAddVersionKey /LANG=${LANG_ENGLISH} "FileVersion" "${VER_MAJOR}.${VER_MINOR}"
VIAddVersionKey /LANG=${LANG_ENGLISH} "ProductVersion" "${VER_MAJOR}.${VER_MINOR}"
;VIAddVersionKey /LANG=${LANG_ENGLISH} "InternalName" ""
;VIAddVersionKey /LANG=${LANG_ENGLISH} "LegalTrademarks" ""
;VIAddVersionKey /LANG=${LANG_ENGLISH} "OriginalFilename" ""
;VIAddVersionKey /LANG=${LANG_ENGLISH} "PrivateBuild" ""
;VIAddVersionKey /LANG=${LANG_ENGLISH} "SpecialBuild" ""
VIProductVersion "${VER_MAJOR}.${VER_MINOR}.0.0"

OutFile "${PROGRAM}-${VER_MAJOR}.${VER_MINOR}.exe"

; The default installation directory
;InstallDir $DESKTOP\${PROGRAM}
InstallDir $PROGRAMFILES\${PROGRAM}

; Registry key to check for directory (so if you install again, it will 
; overwrite the old one automatically)
InstallDirRegKey HKLM "Software\${PROGRAM}" "Install_Dir"

; Request application privileges for Windows Vista
RequestExecutionLevel admin

; License
LicenseData "..\COPYING"

; Options
AutoCloseWindow true
Icon "..\data\png\hero.ico"
XPStyle on

;--------------------------------

; Pages

Page license
Page components
Page directory
Page instfiles

UninstPage uninstConfirm
UninstPage instfiles

;--------------------------------

; the stuff to install
Section "${PROGRAM} (required)"

SectionIn RO
  
; set output path to the installation directory
SetOutPath $INSTDIR

; put files there (3rd party)
!if ${release} == "Debug"
File "C:\Windows\System32\msvcp120d.dll" ; *TODO*: remove this ASAP
File "C:\Windows\System32\msvcr120d.dll" ; *TODO*: remove this ASAP
File "..\..\ACE_wrappers\lib\ACEd.dll"
!else
File "C:\Windows\System32\msvcp120.dll" ; *TODO*: remove this ASAP
File "C:\Windows\System32\msvcr120.dll" ; *TODO*: remove this ASAP
File "..\..\ACE_wrappers\lib\ACE.dll"
!endif

File "..\..\SDL-1.2.15\VisualC\SDL\${release}\SDL.dll"

;File "..\..\SDL_mixer-1.2.12\lib\x86\libFLAC-8.dll"
File "..\..\SDL_mixer-1.2.12\lib\x86\libmikmod-2.dll"
;File "..\..\SDL_mixer-1.2.12\lib\x86\libogg-0.dll"
;File "..\..\SDL_mixer-1.2.12\lib\x86\libvorbis-0.dll"
;File "..\..\SDL_mixer-1.2.12\lib\x86\libvorbisfile-3.dll"
File "..\..\SDL_mixer-1.2.12\lib\x86\smpeg.dll"
File "..\..\SDL_mixer-1.2.12\lib\x86\SDL_mixer.dll"

File "..\..\SDL_image-1.2.12\lib\x86\zlib1.dll"
File "..\..\SDL_image-1.2.12\lib\x86\libpng15-15.dll"
File "..\..\SDL_image-1.2.12\lib\x86\SDL_image.dll"

!if ${release} == "Debug"
File "..\..\ftgl-2.1.3~rc5\msvc\Build\ftgl_D.dll"
!else
File "..\..\ftgl-2.1.3~rc5\msvc\Build\ftgl.dll"
!endif

File "..\prj\msvc\${release}\splot.exe"

; Data
; set output path to the installation directory
SetOutPath $INSTDIR\fnt

File "..\data\fnt\gothub__.ttf"

; set output path to the installation directory
SetOutPath $INSTDIR\png

File "..\data\png\darken.png"
File "..\data\png\elect.png"
File "..\data\png\electric.png"
File "..\data\png\enemy00.png"
File "..\data\png\enemy01.png"
File "..\data\png\enemy01-extra.png"
File "..\data\png\enemy02.png"
File "..\data\png\enemy03.png"
File "..\data\png\enemy03-extra.png"
File "..\data\png\enemy04.png"
File "..\data\png\enemy05.png"
File "..\data\png\enemy06.png"
File "..\data\png\enemyAmmo00.png"
File "..\data\png\enemyAmmo01.png"
File "..\data\png\enemyAmmo02.png"
File "..\data\png\enemyAmmo03.png"
File "..\data\png\enemyAmmo04.png"
File "..\data\png\enemyAmmoExplo00.png"
File "..\data\png\enemyAmmoExplo01.png"
File "..\data\png\enemyAmmoExplo02.png"
File "..\data\png\enemyAmmoExplo03.png"
File "..\data\png\enemyAmmoExplo04.png"
File "..\data\png\enemyExplo.png"
File "..\data\png\explo.png"
File "..\data\png\glitter.png"
File "..\data\png\gndBaseSea.png"
File "..\data\png\gndMetalBase00.png"
File "..\data\png\gndMetalBase01.png"
File "..\data\png\gndMetalBase02.png"
File "..\data\png\gndMetalBlip.png"
File "..\data\png\hero.png"
File "..\data\png\heroAmmo00.png"
File "..\data\png\heroAmmo01.png"
File "..\data\png\heroAmmo02.png"
File "..\data\png\heroAmmoExplo00.png"
File "..\data\png\heroAmmoExplo01.png"
File "..\data\png\heroAmmoExplo02.png"
File "..\data\png\heroAmmoFlash00.png"
File "..\data\png\heroAmmoFlash01.png"
File "..\data\png\heroAmmoFlash02.png"
File "..\data\png\heroExplo00.png"
File "..\data\png\heroExplo01.png"
File "..\data\png\heroExplo02.png"
File "..\data\png\heroShields.png"
File "..\data\png\heroSuper.png"
File "..\data\png\icon32.png"
File "..\data\png\life.png"
File "..\data\png\menu_back.png"
File "..\data\png\menu_updown.png"
File "..\data\png\reflect.png"
File "..\data\png\powerUpAmmo.png"
File "..\data\png\powerUpShield.png"
File "..\data\png\powerUpTex.png"
File "..\data\png\statBar.png"
File "..\data\png\shields.png"
File "..\data\png\stat-top.png"
File "..\data\png\superBomb.png"
File "..\data\png\useFocus.png"
File "..\data\png\useItem00.png"

; set output path to the installation directory
SetOutPath $INSTDIR\wav

File "..\data\wav\boom.wav"
File "..\data\wav\exploBig.wav"
File "..\data\wav\exploPop.wav"
File "..\data\wav\exploStd.wav"
File "..\data\wav\GAMEMENU.MID"
File "..\data\wav\GAMEMUSI.MID"
File "..\data\wav\license.txt"
File "..\data\wav\life_add.wav"
File "..\data\wav\life_lose.wav"
File "..\data\wav\music_game.wav"
File "..\data\wav\music_menu.wav"
File "..\data\wav\power.wav"

; set output path to the application data directory
;SetOutPath $APPDATA\${PROGRAM}
;File "..\..\engine\data\default_player.profile"

; Write the installation path into the registry
WriteRegStr HKLM SOFTWARE\${PROGRAM} "Install_Dir" "$INSTDIR"

; Write the uninstall keys for Windows
WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PROGRAM}" "DisplayName" "${PROGRAM}"
WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PROGRAM}" "UninstallString" '"$INSTDIR\uninstall.exe"'
WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PROGRAM}" "NoModify" 1
WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PROGRAM}" "NoRepair" 1
WriteUninstaller "uninstall.exe"

SectionEnd

;--------------------------------

; Optional section (can be disabled by the user)
Section "Start Menu Shortcuts"

CreateDirectory "$SMPROGRAMS\${PROGRAM}"
CreateShortCut "$SMPROGRAMS\${PROGRAM}\Uninstall.lnk" "$INSTDIR\uninstall.exe" "" "$INSTDIR\uninstall.exe" 0
CreateShortCut "$SMPROGRAMS\${PROGRAM}\${PROGRAM}.lnk" "$INSTDIR\${PROGRAM}.exe" "" "$INSTDIR\${PROGRAM}.exe" 0

SectionEnd

;--------------------------------

; Uninstaller

Section "Uninstall"

; Auto-Close
SetAutoClose true

; Remove registry keys
DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PROGRAM}"
DeleteRegKey HKLM SOFTWARE\${PROGRAM}

; Remove files AND uninstaller (yes this works !!!)
;Delete "$INSTDIR\*.*"
; Ask before removing player profiles ? *TODO*
RMDir /r "$APPDATA\${PROGRAM}"

; Remove shortcuts, if any
Delete "$SMPROGRAMS\${PROGRAM}"

; Remove directories used
RMDir /r "$PROGRAMFILES\${PROGRAM}"
RMDir "$SMPROGRAMS\${PROGRAM}"

SectionEnd
