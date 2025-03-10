#!/usr/bin/env zsh

#
# Replaces {qt} with " in the file passed $1
# Using this because PlistBuddy doesn't like/strips quotes in the string
#
function replaceQuotePlaceholdersInFile() {
  sed -i'' -e 's/{qt}/\"/g' "$1"
}

#
# gets the certificate string for the given bundle identifier $1
#
function getCertificateStr {
  printf "identifier {qt}"$1"{qt} and anchor apple generic and certificate leaf[subject.CN] = {qt}"$APP_CERTIFICATE"{qt}"
}


source .env 2>/dev/null || true

if [ -z $SRCROOT ]; then
  printf "Error: no SRCROOT defined"
  exit 1
fi
if [ -z $APP_CERTIFICATE ]; then
  printf "Error: no APP_CERTIFICATE defined"
  exit 1
fi

sourceInfoPlistPath="$SRCROOT/$TARGET_NAME/Info.plist"
buildPlistPath="$BUILD_DIR/$TARGET_NAME/Info.build.plist"

appType=$(/usr/libexec/PlistBuddy -c "Print AppType" "$sourceInfoPlistPath")

cp "$sourceInfoPlistPath" "$buildPlistPath"

if [ $appType = "Client" ]; then
  daemonBundleIdentifier=$(/usr/libexec/PlistBuddy -c "Print DaemonBundleIdentifier" "$buildPlistPath")
  certificate=$(getCertificateStr $daemonBundleIdentifier)

  /usr/libexec/PlistBuddy -c "Delete SMPrivilegedExecutables" "$buildPlistPath"
  /usr/libexec/PlistBuddy -c "Add SMPrivilegedExecutables dict" "$buildPlistPath"
  /usr/libexec/PlistBuddy -c "Add SMPrivilegedExecutables:${daemonBundleIdentifier} string $certificate" "${buildPlistPath}"
fi

if [ $appType = "Daemon" ]; then
  clientBundleIdentifier=$(/usr/libexec/PlistBuddy -c "Print ClientBundleIdentifier" "$buildPlistPath")
  certificate=$(getCertificateStr $clientBundleIdentifier)

  /usr/libexec/PlistBuddy -c "Delete SMAuthorizedClients" "$buildPlistPath"
  /usr/libexec/PlistBuddy -c "Add SMAuthorizedClients array" "$buildPlistPath"
  /usr/libexec/PlistBuddy -c "Add SMAuthorizedClients: string $certificate" "$buildPlistPath"
fi

replaceQuotePlaceholdersInFile "$buildPlistPath"

