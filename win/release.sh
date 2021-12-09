projectName="KeyRemapper"

filesx86=(
  "win/x86/$projectName/Release/$projectName.exe"
  "win/x86/$projectName/Release/interception.dll"
  "win/x86/$projectName/Release/*.json"
)

filesx64=(
  "win/x64/$projectName/Release/$projectName.exe"
  "win/x64/$projectName/Release/interception.dll"
  "win/x64/$projectName/Release/*.json"
)

tmpDir=$(mktemp -d)

folder86="$projectName""_x86"
folder64="$projectName""_x64"
mkdir $tmpDir/$folder86
mkdir $tmpDir/$folder64

for entry in "${filesx86[@]}"; do
  cp $entry $tmpDir/$folder86
done

for entry in "${filesx64[@]}"; do
  cp $entry $tmpDir/$folder64
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
