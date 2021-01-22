# Builds a .zip file for loading with BMBF
$NDKPath = Get-Content $PSScriptRoot/ndkpath.txt

$buildScript = "$NDKPath/build/ndk-build"
if (-not ($PSVersionTable.PSEdition -eq "Core")) {
    $buildScript += ".cmd"
}

& $buildScript NDK_PROJECT_PATH=$PSScriptRoot APP_BUILD_SCRIPT=$PSScriptRoot/Android.mk NDK_APPLICATION_MK=$PSScriptRoot/Application.mk

Compress-Archive -Path "./libs/arm64-v8a/libCrashMod_0_1_0.so","./libs/arm64-v8a/libcustom-types.so", "./libs/arm64-v8a/libquestui.so", "./libs/arm64-v8a/libcodegen_0_6_1.so", "./libs/arm64-v8a/libbeatsaber-hook_1_0_10.so", "./bmbfmod.json" -DestinationPath "./CrashMod_v0.1.0.zip" -Update
