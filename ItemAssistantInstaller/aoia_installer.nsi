!include "FileFunc.nsh"

SetCompressor /SOLID lzma

Name "Item Assistant Installer"

!ifndef BUILD_NUMBER
!define BUILD_NUMBER 0.0.0.0
!endif

# Add-Remove-Programs registry key
!define ARP_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\ItemAssistant"

OutFile "ItemAssistant-installer-${BUILD_NUMBER}.exe"

InstallDir "c:\aoia"

RequestExecutionLevel admin

Page directory
Page instfiles

Section
	# Install for all users
	SetShellVarContext all

	SetOutPath $INSTDIR

	WriteUninstaller "$INSTDIR\ItemAssistant-uninstaller.exe"

	File ..\Release\ItemAssistant.exe
	File ..\Release\ItemAssistant*.dll
	File ..\Release\README.txt
	File ..\Release\sqlite3.dll
	File ..\Release\*.manifest
	File ..\Release\msvc*.dll
	
#	CreateShortCut "$SMPROGRAMS\ItemAssistant.lnk" "$INSTDIR\ItemAssistant.exe"


	# Register uninstaller in "add/remove programs".
	WriteRegStr HKLM 	"${ARP_KEY}" "DisplayName"			"Anarchy Online Item Assistant"
	WriteRegStr HKLM 	"${ARP_KEY}" "UninstallString"		"$\"$INSTDIR\ItemAssistant-uninstaller.exe$\""
	WriteRegStr HKLM 	"${ARP_KEY}" "QuietUninstallString"	"$\"$INSTDIR\ItemAssistant-uninstaller.exe$\" /S"
	WriteRegStr HKLM 	"${ARP_KEY}" "HelpLink"				"http://ia-help.frellu.net"
	WriteRegStr HKLM 	"${ARP_KEY}" "DisplayVersion" 		"${BUILD_NUMBER}"
	WriteRegStr HKLM 	"${ARP_KEY}" "Publisher"			"Kode Regn"
	WriteRegDWORD HKLM 	"${ARP_KEY}" "NoModify" 			1
	WriteRegDWORD HKLM	"${ARP_KEY}" "NoRepair"				1

	# Calculate and record installed size
	${GetSize} "$INSTDIR" "/S=0K" $0 $1 $2
	IntFmt $0 "0x%08X" $0
	WriteRegDWORD HKLM "${ARP_KEY}" "EstimatedSize" "$0"
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

	Delete "$INSTDIR\ItemAssistant-uninstaller.exe"

	# Delete installation folder it if is not empty.
	# Do _NOT_ use /r option for this folder incase something else has been installed. Witout /r only an empty folder will be deleted.
	RMDir "$INSTDIR"
	
#	Delete "$SMPROGRAMS\ItemAssistant.lnk"

	# Remove from "add/remove programs".
	DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\ItemAssistant"
SectionEnd
