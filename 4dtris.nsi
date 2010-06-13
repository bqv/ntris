Name "4dtris"
OutFile "4dtris-0.3.1.exe"
InstallDir $PROGRAMFILES\4dtris
RequestExecutionLevel user

Page directory
Page instfiles

Section ""
  SetOutPath $INSTDIR
  File 4dtris.exe

  CreateDirectory "$SMPROGRAMS\4dtris"
  CreateShortCut "$SMPROGRAMS\4dtris\4dtris.lnk" "$INSTDIR\4dtris.exe" "" "$INSTDIR\4dtris.exe" 0
SectionEnd
