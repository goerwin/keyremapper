projectName="KeyRemapper"

filesx86=(
  "x86/$projectName/Release/$projectName.exe"
  "x86/$projectName/Release/interception.dll"
  "x86/$projectName/Release/symbols.json"
  "x86/$projectName/Release/mode1.json"
)

filesx64=(
  "x64/$projectName/Release/$projectName.exe"
  "x64/$projectName/Release/interception.dll"
  "x64/$projectName/Release/symbols.json"
  "x64/$projectName/Release/mode1.json"
)

tmpDir=$(mktemp -d)

mkdir $tmpDir/$projectName
mkdir $tmpDir/$projectName/x86
mkdir $tmpDir/$projectName/x64

for entry in "${filesx86[@]}"; do
  cp $entry $tmpDir/$projectName/x86
done

for entry in "${filesx64[@]}"; do
  cp $entry $tmpDir/$projectName/x64
done

# Since zip always includes the parent dir, in this case
# the temp dir, I have to cd into it and zip the folder I want
# and move it. Parenthesis are required so it doesn't change the
# cd in the main shell
(cd $tmpDir && zip -q -r $projectName.zip .)
mv $tmpDir/$projectName.zip .

# zip -q $projectName.zip $tmpDir/$projectName/x86
rm -rf $tmpDir

echo "Release $projectName.zip was created!"
