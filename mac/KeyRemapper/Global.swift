import AppKit
import SecurityFoundation
import ServiceManagement
import SystemConfiguration

struct Global {
  static let state: State = State()

  //https://ja.stackoverflow.com/questions/73580/authorizationenvironment%E3%82%92%E7%94%9F%E6%88%90%E3%81%99%E3%82%8Bobjective-c%E3%81%AE%E3%82%B3%E3%83%BC%E3%83%89%E3%82%92swift%E3%81%A7%E6%9B%B8%E3%81%8F%E6%96%B9%E6%B3%95%E3%82%92%E3%81%8A%E3%81%97%E3%81%88%E3%81%A6%E3%81%8F%E3%81%A0%E3%81%95%E3%81%84
  static func getPrivilegedHelperAuth() -> AuthorizationRef? {
    var authItem = AuthorizationItem(
      name: kSMRightBlessPrivilegedHelper, valueLength: 0,
      value: UnsafeMutableRawPointer(bitPattern: 0), flags: 0)
    var rights = AuthorizationRights(count: 1, items: &authItem)
    var authRef: AuthorizationRef?
    AuthorizationCreate(
      &rights, nil, [.interactionAllowed, .extendRights, .preAuthorize],
      &authRef)

    return authRef
  }

  static func showCloseAlert(_ title: String, _ description: String) {
    let alert = NSAlert()
    alert.messageText = title
    alert.informativeText = description
    alert.alertStyle = NSAlert.Style.warning
    alert.addButton(withTitle: "OK")
    alert.runModal()
  }

  static func blessHelper(label: String, authRef: AuthorizationRef) -> Bool {
    var error: Unmanaged<CFError>?
    let blessStatus = SMJobBless(
      kSMDomainSystemLaunchd, label as CFString, authRef, &error)

    if !blessStatus {
      NSLog(
        "[SMJBS]: Helper bless failed with error \(error!.takeUnretainedValue())"
      )
    }

    return blessStatus
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
