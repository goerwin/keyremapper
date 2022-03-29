projectName="KeyRemapper"

archs=("x86" "x64")

tmpDir=$(mktemp -d)

for arch in "${archs[@]}"; do
  archFolder="$projectName""_$arch"
  mkdir -p $tmpDir/$archFolder/winKeyRemapperWin
  # mkdir "win/$arch/$projectName/Release/keyRemapperWin"

  cp "win/$arch/$projectName/Release/$projectName.exe" $tmpDir/$archFolder
  cp "win/$arch/$projectName/Release/interception.dll" $tmpDir/$archFolder
  cp "win/$arch/$projectName/Release/config.json" $tmpDir/$archFolder/winKeyRemapperWin
  cp "win/$arch/$projectName/Release/symbols.json" $tmpDir/$archFolder/winKeyRemapperWin
done

# Since zip always includes the parent dir, in this case
# the temp dir, I have to cd into it and zip the folder I want
# and move it. Parenthesis are required so it doesn't change the
# cd in the main shell
(cd $tmpDir && zip -q -r $projectName.zip .)

outputWinDir="win/output"
rm -rf $outputWinDir
mkdir -p $outputWinDir
mv $tmpDir/$projectName.zip $outputWinDir

rm -rf $tmpDir

echo "$outputWinDir/$projectName.zip was created!"
