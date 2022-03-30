# can only be done with PowerShell (msbuild only available there)
# msbuild /target:Rebuild /p:Configuration=Release /p:Platform=x86 .\KeyRemapper.vcxproj
# msbuild /target:Rebuild /p:Configuration=Release /p:Platform=x64 .\KeyRemapper.vcxproj

# cp "x64/KeyRemapper/Release/$projectName.exe" $tmpDir/$outputFolderName

$archs = "x86","x64"
$projectName = "KeyRemapper"
$pwd = ("" + (Get-Location))
$distPath = ($pwd + "\dist")
$zipAbsPath = ($distPath + "\win.zip")

Remove-Item -Force -Recurse -ErrorAction SilentlyContinue $distPath
New-Item -Path $pwd -Type Directory -Name "dist"

for ($i = 0; $i -le ($archs.length - 1); $i += 1) {
  $arch = $archs[$i]
  $outputFolderName = ($projectName + "_" + $arch)
  $outputAbsPath = ($distPath + "\" + $outputFolderName)
  $outputConfigFolderAbsPath = ($outputAbsPath + "\keyRemapperWin")

  Remove-Item -Force -Recurse -ErrorAction SilentlyContinue $arch
  New-Item -Path $distPath -Type Directory -Name $outputFolderName
  New-Item -Path $outputAbsPath -Type Directory -Name "keyRemapperWin"

  # Note: To test locally, use this instead
  # $msbuild = "C:\Program Files\Microsoft Visual Studio\2022\Community\Msbuild\Current\Bin\MSBuild.exe"
  # . $msbuild /target:Rebuild /p:Configuration=Release /p:Platform=$arch .\KeyRemapper.vcxproj

  msbuild /target:Rebuild /p:Configuration=Release /p:Platform=$arch .\KeyRemapper.vcxproj

  $releaseFolderPath = ($arch + "/" + $projectName + "/Release")
  cp ($releaseFolderPath + "/" + $projectName + ".exe") -Destination $outputAbsPath
  cp ($releaseFolderPath + "/interception.dll") -Destination $outputAbsPath
  cp ($releaseFolderPath + "/config.json") -Destination $outputConfigFolderAbsPath
  cp ($releaseFolderPath + "/symbols.json") -Destination $outputConfigFolderAbsPath

  # Cleanup
  Remove-Item -Force -Recurse -ErrorAction SilentlyContinue $arch
}

Remove-Item -Force -Recurse -ErrorAction SilentlyContinue $zipAbsPath
Compress-Archive -Path ($distPath + "\*") -Destination $zipAbsPath
