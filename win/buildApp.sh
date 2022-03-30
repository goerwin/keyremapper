#!/usr/bin/env bash

# Input $1 should be of type (v1.0.0)

# Update version in src/resources.rc
version="$1.0"                                           # => v1.0.0.0
version=$(echo "$version" | sed -r 's/v//g')             # => 1.0.0.0
versionWithCommas=$(echo "$version" | sed -r 's/\./,/g') # => v1,0,0,0

sed -i -e 's/FILEVERSION 1,0,0,1/FILEVERSION '$versionWithCommas'/' src/resources.rc
sed -i -e 's/PRODUCTVERSION 1,0,0,1/PRODUCTVERSION '$versionWithCommas'/' src/resources.rc
sed -i -e 's/"FileVersion", "1.0.0.1"/"FileVersion", "'$version'"/' src/resources.rc
sed -i -e 's/"ProductVersion", "1.0.0.1"/"ProductVersion", "'$version'"/' src/resources.rc

# Build
projectName="KeyRemapper"
archs=("x86" "x64")
tmpDir=$(mktemp -d)

for arch in "${archs[@]}"; do
  archFolder="$projectName""_$arch"
  mkdir -p $tmpDir/$archFolder/keyRemapperWin

  # Remove previous builds
  rm -rf "$arch"

  msbuild /target:Rebuild /p:Configuration=Release /p:Platform="$arch" .\KeyRemapper.vcxproj
  msbuild /target:Rebuild /p:Configuration=Release /p:Platform="$arch" .\KeyRemapper.vcxproj

  cp "$arch/$projectName/Release/$projectName.exe" $tmpDir/$archFolder
  cp "$arch/$projectName/Release/interception.dll" $tmpDir/$archFolder
  cp "$arch/$projectName/Release/config.json" $tmpDir/$archFolder/keyRemapperWin
  cp "$arch/$projectName/Release/symbols.json" $tmpDir/$archFolder/keyRemapperWin

  # Cleanup
  rm -rf "$arch"
done

# Since zip always includes the parent dir, in this case
# the temp dir, I have to cd into it and zip the folder I want
# and move it. Parenthesis are required so it doesn't change the
# cd in the main shell
(cd $tmpDir && zip -q -r win.zip .)

rm -rf win.zip
mv $tmpDir/win.zip .

# Cleanup
rm -rf $tmpDir

echo "win.zip was created!"
