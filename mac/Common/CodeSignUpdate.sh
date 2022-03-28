appType=$(/usr/libexec/PlistBuddy -c 'Print AppType' "${INFOPLIST_FILE}")

function replacePlaceholders {
  sed -i.bu 's/__QUOTE__/\"/g' "$1"
  rm -rf "$1.bu"
}

function getCertificate {
  certificate="certificate 1[field.1.2.840.113635.100.6.2.1]"
  echo "identifier __QUOTE__"$1"__QUOTE__ and anchor apple generic and certificate leaf[subject.CN] = __QUOTE__"$EXPANDED_CODE_SIGN_IDENTITY_NAME"__QUOTE__ and "$certificate" /* exists */"
}

if [ $appType == "Client" ]; then
  daemonBundleIdentifier=$(/usr/libexec/PlistBuddy -c 'Print DaemonBundleIdentifier' "${INFOPLIST_FILE}")
  certificate=$(getCertificate $daemonBundleIdentifier)

  /usr/libexec/PlistBuddy -c 'Delete SMPrivilegedExecutables' "${INFOPLIST_FILE}"
  /usr/libexec/PlistBuddy -c 'Add SMPrivilegedExecutables dict' "${INFOPLIST_FILE}"
  /usr/libexec/PlistBuddy -c 'Add SMPrivilegedExecutables:'"${daemonBundleIdentifier}"' string '"$certificate"'' "${INFOPLIST_FILE}"
  replacePlaceholders "${INFOPLIST_FILE}"
fi

if [ $appType == "Daemon" ]; then
  clientBundleIdentifier=$(/usr/libexec/PlistBuddy -c 'Print ClientBundleIdentifier' "${INFOPLIST_FILE}")
  certificate=$(getCertificate $clientBundleIdentifier)

  # ClientBundleIdentifier
  /usr/libexec/PlistBuddy -c 'Delete SMAuthorizedClients' "${INFOPLIST_FILE}"
  /usr/libexec/PlistBuddy -c 'Add SMAuthorizedClients array' "${INFOPLIST_FILE}"
  /usr/libexec/PlistBuddy -c 'Add SMAuthorizedClients: string '"$certificate"'' "${INFOPLIST_FILE}"
  replacePlaceholders "${INFOPLIST_FILE}"
fi
