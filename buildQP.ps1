# Builds a .qmod file for loading with QP
& $PSScriptRoot/build.ps1

Compress-Archive -Path "./libs/arm64-v8a/libCrashMod.so", ".\extern\libbeatsaber-hook_2_2_4.so", ".\mod.json" -DestinationPath "./CrashMod.zip" -Update
Remove-Item "./CrashMod.qmod"
Rename-Item "./CrashMod.zip" "./CrashMod.qmod"