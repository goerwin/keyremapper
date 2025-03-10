#!/usr/bin/env bash
set -ex

#
# Installs the Apple certificate to sign the app and builds the macOS App using xcodebuild.
# The input for this script ($1) should be the version to be built. For example, v1.0.0
#
# This is meant to be run on GitHub Actions, but can be run locally with precautions, because
# the certificates can be removed or conflict with the current ones in the system. So AVOID IT.
#
# I'm not using Provisioning profiles since I don't have a paid Apple Developer account
# and this is not meant to be distributed in the App Store.
#

# Arguments (eg: v4.4.0)
# In the github action flow, this will passed a tag name (eg: v4.4.0)
versionStr=$1

#
# Function to check if a variable is set
#
throw_if_empty() {
  local var_value
  eval "var_value=\${$1}"

  if [[ -z "$var_value" ]]; then
    echo "Error: Environment variable $1 not defined" >&2
    exit 1
  fi
}

# Load environment variables
if [ -f .env ]; then
  source .env
fi

# Check required variables
throw_if_empty "versionStr"
throw_if_empty "BUILD_CERTIFICATE_BASE64"
throw_if_empty "P12_PASSWORD"
throw_if_empty "KEYCHAIN_PASSWORD"

###
#######################################################################
# INSTALL CERTIFICATE
#######################################################################
###

# Guide on how to install Apple certificate on macOS runners
# https://docs.github.com/en/actions/use-cases-and-examples/deploying/installing-an-apple-certificate-on-macos-runners-for-xcode-development
#

# If RUNNER_TEMP is not set, create a temporary directory
# This env var is set by GitHub Actions, so we need to create it
# manually when running locally.
if [[ -z "$RUNNER_TEMP" ]]; then
  RUNNER_TEMP="$(mktemp -d)"
fi

# create variables
CERTIFICATE_PATH=$RUNNER_TEMP/build_certificate.p12
KEYCHAIN_PATH=$RUNNER_TEMP/app-signing.keychain-db
# PP_PATH=$RUNNER_TEMP/build_pp.mobileprovision

# import certificate from secrets
echo -n "$BUILD_CERTIFICATE_BASE64" | base64 --decode -o $CERTIFICATE_PATH
# echo -n "$BUILD_PROVISION_PROFILE_BASE64" | base64 --decode -o $PP_PATH

# create temporary keychain
security create-keychain -p "$KEYCHAIN_PASSWORD" $KEYCHAIN_PATH
security set-keychain-settings -lut 21600 $KEYCHAIN_PATH
security unlock-keychain -p "$KEYCHAIN_PASSWORD" $KEYCHAIN_PATH

# import certificate to keychain
security import $CERTIFICATE_PATH -P "$P12_PASSWORD" -A -t cert -f pkcs12 -k $KEYCHAIN_PATH
security list-keychain -d user -s $KEYCHAIN_PATH

# apply provisioning profile
# mkdir -p ~/Library/MobileDevice/Provisioning\ Profiles
# cp $PP_PATH ~/Library/MobileDevice/Provisioning\ Profiles

###
#######################################################################
# BUILD APP
#######################################################################
###

# Update version in project.pbxproj
version=$(echo "$versionStr" | sed -r 's/v//g') # => 1.0.0
sed -i '' -e 's/MARKETING_VERSION \= [^\;]*\;/MARKETING_VERSION = '$version';/' KeyRemapper.xcodeproj/project.pbxproj

# Remove previous builds/dist
rm -rf build
rm -rf dist

# Build
xcodebuild archive -target KeyRemapper -configuration Release

# Zip
appPath=$(readlink "build/Release/KeyRemapper.app")

mkdir -p "dist/mac"
cp -r "$appPath" "dist/mac"
(
  cd "dist"
  zip -r mac.zip .
)

echo "mac.zip created! at $(pwd)"

###
#######################################################################
# CLEAN UP
#######################################################################
###

# Removing build folder
rm -rf build

# Removing keychain and certificate files at $RUNNER_TEMP
security delete-keychain $RUNNER_TEMP/app-signing.keychain-db
rm -rf "$RUNNER_TEMP"
