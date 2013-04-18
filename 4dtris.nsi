Name "4dtris"
OutFile "4dtris-1.0.0.exe"
InstallDir $PROGRAMFILES\4dtris
RequestExecutionLevel user

Page directory
Page instfiles

Section ""
  SetOutPath $INSTDIR
  File 4dtris.exe
  File LiberationSans-BoldItalic.ttf
  File SDL.dll
  File SDL_ttf.dll
  File libcharset1.dll
  File libfreetype-6.dll
  File libiconv-2.dll
  File libintl-8.dll
  File zlib1.dll
  File AUTHORS
  File NEWS
  File COPYING
  File ChangeLog
  File README

  CreateDirectory "$SMPROGRAMS\4dtris"
  CreateShortCut "$SMPROGRAMS\4dtris\4dtris.lnk" "$INSTDIR\4dtris.exe" "" "$INSTDIR\4dtris.exe" 0
SectionEnd
