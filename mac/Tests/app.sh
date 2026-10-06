#!/usr/bin/env bash
set -euo pipefail

#
# Smoke test of the installed app: builds and installs the signed app, then drives its menu with
# UI scripting and checks that the daemon keeps running. The terminal app needs Accessibility.
# Usage: mac/Tests/app.sh [--no-build]
#

app=/Applications/KeyRemapper.app
build=/tmp/keyremapper-test-app
process='application process "KeyRemapper"'
menu="menu 1 of menu bar item 1 of menu bar 2 of $process"
failures=0

ui() { osascript -e "tell application \"System Events\" to $1"; }
menuItems() { ui "get name of every menu item of $menu" | sed 's/, /\n/g'; }
menuHas() { menuItems | grep -qx "$1"; }
# Profile items start with a checkmark when active
clickMenu() {
  ui "click (first menu item of $menu whose name is \"$1\" or name is \"✔  $1\")" >/dev/null || true
  sleep 1
}
daemonPid() { pgrep -f 'MacOS/co.goerwin.KeyRemapperDaemon' || true; }
daemonIs() { [[ "$(daemonPid)" == "$1" ]]; }
daemonRuns() { [[ -n "$(daemonPid)" ]]; }
daemonStopped() { [[ -z "$(daemonPid)" ]]; }
hasWindow() { ui "exists window \"$1\" of $process" | grep -q true; }
hasNoWindow() { ! hasWindow "$1"; }
hasNoAlerts() { [[ -z "$(ui "get name of every window of $process whose subrole is \"AXDialog\"")" ]]; }
daemonNotInDock() { ! lsappinfo list | grep -A4 '"co.goerwin.KeyRemapperDaemon"' | grep -q Foreground; }

check() {
  local name=$1
  shift
  if "$@"; then
    echo "ok: $name"
  else
    echo "FAIL: $name"
    failures=$((failures + 1))
  fi
}

waitFor() {
  for _ in $(seq 1 20); do
    "$@" && return 0
    sleep 0.5
  done
  return 1
}

if [[ "${1:-}" != "--no-build" ]]; then
  (cd "$(dirname "$0")/.." && xcodebuild -quiet -project KeyRemapper.xcodeproj -target KeyRemapper \
    -configuration Release build SYMROOT="$build")
fi

frontApp=$(lsappinfo info -only bundleid "$(lsappinfo front)" | cut -d'"' -f4)

# The daemon quits with the app, then launchd starts the new one for the new app
pkill -x KeyRemapper || true
waitFor daemonStopped
rm -rf "$app"
ditto "$build/Release/KeyRemapper.app" "$app"
open "$app"

check "starts the daemon" waitFor daemonRuns
pid=$(daemonPid)
check "starts remapping" waitFor menuHas Pause
check "shows no alerts" hasNoAlerts
check "keeps the daemon out of the Dock" daemonNotInDock

clickMenu Pause
check "pauses" menuHas Resume
open -a Finder && sleep 0.5 && open -b "$frontApp" && sleep 0.5
check "survives switching apps while paused" daemonIs "$pid"
clickMenu Resume
check "resumes" menuHas Pause

# Profiles are the menu items between the 2nd and 3rd separators
profiles=$(menuItems | awk '/^missing value$/ { n++; next } n == 2')
active=$(echo "$profiles" | grep '^✔  ' | sed 's/^✔  //')
while read -r profile; do
  name=${profile#✔  }
  clickMenu "$name"
  check "switches to profile \"$name\"" menuHas "✔  $name"
done <<< "$profiles"
clickMenu "$active"
check "survives switching profiles" daemonIs "$pid"

clickMenu Logger
check "opens the Logger" hasWindow Logger
ui "click button 1 of window \"Logger\" of $process" >/dev/null
sleep 1
check "closes the Logger" hasNoWindow Logger
check "shows no alerts at the end" hasNoAlerts
check "keeps the same daemon running" daemonIs "$pid"

echo
if ((failures)); then
  echo "$failures FAILED"
  exit 1
fi
echo "SUCCESS!"
