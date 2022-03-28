#!/usr/bin/env zsh

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

appType=$(/usr/libexec/PlistBuddy -c 'Print AppType' "${sourceInfoPlistPath}")

cp "$sourceInfoPlistPath" "$buildPlistPath"

function replacePlaceholders {
  sed -i.bu 's/__QUOTE__/\"/g' "$1"
  rm -rf "$1.bu"
}

function getCertificate {
  certificate=$(printf $APP_CERTIFICATE | sed -e "s/\"/__QUOTE__/g")
  printf "identifier __QUOTE__"$1"__QUOTE__ and anchor apple generic and "$certificate""
}

if [ $appType = "Client" ]; then
  daemonBundleIdentifier=$(/usr/libexec/PlistBuddy -c 'Print DaemonBundleIdentifier' "${buildPlistPath}")
  certificate=$(getCertificate $daemonBundleIdentifier)

  /usr/libexec/PlistBuddy -c 'Delete SMPrivilegedExecutables' "${buildPlistPath}"
  /usr/libexec/PlistBuddy -c 'Add SMPrivilegedExecutables dict' "${buildPlistPath}"
  /usr/libexec/PlistBuddy -c 'Add SMPrivilegedExecutables:'"${daemonBundleIdentifier}"' string '"$certificate"'' "${buildPlistPath}"
  replacePlaceholders "${buildPlistPath}"
fi

if [ $appType = "Daemon" ]; then
  clientBundleIdentifier=$(/usr/libexec/PlistBuddy -c 'Print ClientBundleIdentifier' "${buildPlistPath}")
  certificate=$(getCertificate $clientBundleIdentifier)

  /usr/libexec/PlistBuddy -c 'Delete SMAuthorizedClients' "${buildPlistPath}"
  /usr/libexec/PlistBuddy -c 'Add SMAuthorizedClients array' "${buildPlistPath}"
  /usr/libexec/PlistBuddy -c 'Add SMAuthorizedClients: string '"$certificate"'' "${buildPlistPath}"
  replacePlaceholders "${buildPlistPath}"
fi
