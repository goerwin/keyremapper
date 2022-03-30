projectName="KeyRemapper"
archs=("x86" "x64")
tmpDir=$(mktemp -d)

for arch in "${archs[@]}"; do
  archFolder="$projectName""_$arch"
  mkdir -p $tmpDir/$archFolder/keyRemapperWin

  cp "$arch/$projectName/Release/$projectName.exe" $tmpDir/$archFolder
  cp "$arch/$projectName/Release/interception.dll" $tmpDir/$archFolder
  cp "$arch/$projectName/Release/config.json" $tmpDir/$archFolder/keyRemapperWin
  cp "$arch/$projectName/Release/symbols.json" $tmpDir/$archFolder/keyRemapperWin
done

# Since zip always includes the parent dir, in this case
# the temp dir, I have to cd into it and zip the folder I want
# and move it. Parenthesis are required so it doesn't change the
# cd in the main shell
(cd $tmpDir && zip -q -r win.zip .)

rm -rf win.zip
mv $tmpDir/win.zip .

rm -rf $tmpDir

echo "win.zip was created!"
