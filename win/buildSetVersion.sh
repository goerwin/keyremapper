version="$1.0" # => v1.0.0.0
version=$(echo "$version" | sed -r 's/v//g') # => 1.0.0.0
versionWithCommas=$(echo "$version" | sed -r 's/\./,/g') # => v1,0,0,0

sed -i -e 's/FILEVERSION 1,0,0,1/FILEVERSION '$versionWithCommas'/' src/resources.rc
sed -i -e 's/PRODUCTVERSION 1,0,0,1/PRODUCTVERSION '$versionWithCommas'/' src/resources.rc
sed -i -e 's/"FileVersion", "1.0.0.1"/"FileVersion", "'$version'"/' src/resources.rc
sed -i -e 's/"ProductVersion", "1.0.0.1"/"ProductVersion", "'$version'"/' src/resources.rc
