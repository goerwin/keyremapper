import AppKit
import SystemConfiguration

struct Global {
  static let state: State = State()

  // Versions up to 4.x installed the daemon with SMJobBless. It uses the same label, so it has
  // to be removed (as admin, once) before registering the daemon bundled in the app
  static func removeLegacyDaemon() -> Bool {
    let label = Constants.MACH_SERVICE_NAME
    let paths = [
      "/Library/LaunchDaemons/\(label).plist", "/Library/PrivilegedHelperTools/\(label)",
    ].filter(fileExists)

    if paths.isEmpty { return true }

    let command = "launchctl bootout system/\(label); rm -f \(paths.joined(separator: " "))"
    var error: NSDictionary?
    NSAppleScript(source: "do shell script \"\(command)\" with administrator privileges")?
      .executeAndReturnError(&error)

    return error == nil
  }

  static func runProcess(_ path: String, args: [String]) {
    let process = Process()
    process.executableURL = URL(fileURLWithPath: path)
    process.arguments = args
    try? process.run()
    process.waitUntilExit()
  }

  static func showCloseAlert(_ title: String, _ description: String) {
    let alert = NSAlert()
    alert.messageText = title
    alert.informativeText = description
    alert.alertStyle = NSAlert.Style.warning
    alert.addButton(withTitle: "OK")
    alert.runModal()
  }

  static func fileExists(_ filePath: String) -> Bool {
    let fileManager = FileManager.default
    if fileManager.fileExists(atPath: filePath) {
      return true
    }

    return false
  }

  static func resizeImage(image: NSImage, w: Int, h: Int) -> NSImage? {
    let frame = NSRect(x: 0, y: 0, width: w, height: h)
    let newSize = NSSize(width: w, height: h)

    guard
      let representation = image.bestRepresentation(
        for: frame, context: nil, hints: nil)
    else {
      return nil
    }
    let image = NSImage(
      size: newSize, flipped: false,
      drawingHandler: { (_) -> Bool in
        return representation.draw(in: frame)
      })

    return image
  }

  static func getCurLoggedInUserFromRoot() -> String? {
    var uid: uid_t = 0
    var gid: gid_t = 0

    guard let name = SCDynamicStoreCopyConsoleUser(nil, &uid, &gid) else {
      return nil
    }

    return name as String
  }

  static func getRootPath() -> String? {
    // Since we are running from root, the current user is root so
    // I have to use this to get the actual logged in user
    guard let currentLoggedInUser = getCurLoggedInUserFromRoot() else {
      return nil
    }
    return "/Users/\(currentLoggedInUser)/keyRemapperMac"
  }

  static func getJsonConfig() -> [String: AnyObject]? {
    guard let configPath = getConfigPath() else { return nil }

    do {
      let data = try Data(
        contentsOf: URL(fileURLWithPath: configPath), options: .mappedIfSafe)
      let jsonResult = try JSONSerialization.jsonObject(
        with: data, options: .mutableLeaves)

      return jsonResult as? [String: AnyObject]
    } catch {}

    return nil
  }

  static func updateJsonConfigActiveProfileIdx(idx: Int) {
    guard let configPath = getConfigPath() else { return }
    guard var jsonConfig = getJsonConfig() else { return }

    do {
      jsonConfig["activeProfileIdx"] = idx as AnyObject
      let data = try JSONSerialization.data(
        withJSONObject: jsonConfig, options: .prettyPrinted)
      FileManager.default.createFile(
        atPath: configPath, contents: data, attributes: nil)
    } catch {}
  }

  static func getJsonConfigActiveProfileIdx() -> Int? {
    guard let jsonConfig = getJsonConfig() else { return nil }
    return jsonConfig["activeProfileIdx"] as? Int
  }

  static func getConfigPath() -> String? {
    guard let rootPath = getRootPath() else { return nil }
    return "\(rootPath)/config.json"
  }

  static func getResourceSymbolsPath() -> String? {
    guard let resourcePath = Bundle.main.resourcePath else { return nil }
    return "\(resourcePath)/symbols.json"
  }

  static func copyFile(srcPath filePath: String, to destPath: String) throws {
    let data = try Data(contentsOf: URL(fileURLWithPath: filePath))
    FileManager.default.createFile(
      atPath: destPath, contents: data, attributes: nil)
  }
}
