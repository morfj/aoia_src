SetCompressor /SOLID lzma

Name "Item Assistant Installer"

OutFile "aoia_setup.exe"

InstallDir "c:\aoia"

RequestExecutionLevel admin

Page directory
Page instfiles

Section
	# Install for all users
	SetShellVarContext all

	SetOutPath $INSTDIR

	WriteUninstaller "$INSTDIR\aoia_uninstall.exe"

	File ..\Release\ItemAssistant.exe
	File ..\Release\ItemAssistant*.dll
	File ..\Release\README.txt
	File ..\Release\sqlite3.dll
	File ..\Release\*.manifest
	File ..\Release\msvc*.dll
	
#	CreateShortCut "$SMPROGRAMS\ItemAssistant.lnk" "$INSTDIR\ItemAssistant.exe"
SectionEnd

#Section "Start Automatically"
#	WriteRegStr SHELL_CONTEXT "Software\Microsoft\Windows\CurrentVersion\Run" "ItemAssistant" "$INSTDIR\ItemAssistant.exe"
#SectionEnd

#Section "un.Start Automatically"
#	DeleteRegValue SHELL_CONTEXT "Software\Microsoft\Windows\CurrentVersion\Run" "ItemAssistant"
#SectionEnd

Section "uninstall"
	# Uninnstall for all users
	SetShellVarContext all

	# Delete installed files
	Delete "$INSTDIR\ItemAssistant*.dll"
	Delete "$INSTDIR\ItemAssistant.exe"
	Delete "$INSTDIR\README.txt"
	Delete "$INSTDIR\sqlite3.dll"
	Delete "$INSTDIR\*.manifest"
	Delete "$INSTDIR\msvc*.dll"

	# Delete generated files & folders
	Delete "$INSTDIR\aoitems.db"
	Delete "$INSTDIR\ItemAssistant.db"
	Delete "$INSTDIR\ItemAssistant.conf"
	RMDir /r "$INSTDIR\binfiles"

	# Delete installation folder it if is not empty.
	# Do _NOT_ use /r option for this folder incase something else has been installed. Witout /r only an empty folder will be deleted.
	RMDir "$INSTDIR"
	
	Delete "$INSTDIR\aoia_uninstall.exe"
#	Delete "$SMPROGRAMS\ItemAssistant.lnk"
SectionEnd
