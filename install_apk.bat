@echo off
echo Installing VCamdroid apk to android device

dist\adb\adb.exe install -r dist\apk\app-release.apk