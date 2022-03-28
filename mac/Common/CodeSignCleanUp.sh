appType=$(/usr/libexec/PlistBuddy -c 'Print AppType' "${INFOPLIST_FILE}")

if [ $appType == "Client" ]; then
  /usr/libexec/PlistBuddy -c 'Delete SMPrivilegedExecutables' "${INFOPLIST_FILE}"
  /usr/libexec/PlistBuddy -c 'Add SMPrivilegedExecutables dict' "${INFOPLIST_FILE}"
  /usr/libexec/PlistBuddy -c 'Add SMPrivilegedExecutables:'"__replacedByScripts__"' string '"$certificate"'' "${INFOPLIST_FILE}"
fi

if [ $appType == "Daemon" ]; then
  /usr/libexec/PlistBuddy -c 'Delete SMAuthorizedClients' "${INFOPLIST_FILE}"
  /usr/libexec/PlistBuddy -c 'Add SMAuthorizedClients array' "${INFOPLIST_FILE}"
  /usr/libexec/PlistBuddy -c 'Add SMAuthorizedClients: string '"__replacedByScripts__"'' "${INFOPLIST_FILE}"
fi
