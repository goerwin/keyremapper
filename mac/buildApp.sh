#!/usr/bin/env bash

# Input $1 should be of type (v1.0.0)

# Update version in project.pbxproj
version=$(echo "$1" | sed -r 's/v//g') # => 1.0.0
sed -i '' -e 's/MARKETING_VERSION \= [^\;]*\;/MARKETING_VERSION = '$version';/' KeyRemapper.xcodeproj/project.pbxproj

# Remove previous builds
rm -rf build
rm -rf mac

# Build
xcodebuild archive -target KeyRemapper -configuration Release

# Zip
appPath=$(readlink "build/Release/KeyRemapper.app")
mkdir mac
cp -r "$appPath" mac
zip -r mac.zip mac

# Cleanup
rm -rf mac
rm -rf build

echo "mac.zip created!"
