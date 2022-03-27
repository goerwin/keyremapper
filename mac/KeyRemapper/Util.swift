import SecurityFoundation
import ServiceManagement
import SystemConfiguration
import AppKit

struct Util {
    static func getPrivilegedHelperAuth() -> AuthorizationRef? {
      var authItem = AuthorizationItem(name: kSMRightBlessPrivilegedHelper, valueLength: 0, value:UnsafeMutableRawPointer(bitPattern: 0), flags: 0)
      var rights = AuthorizationRights(count: 1, items: &authItem)
      var authRef: AuthorizationRef?
      AuthorizationCreate(&rights, nil, [.interactionAllowed, .extendRights, .preAuthorize], &authRef)
    
      return authRef
    }
    
    static func blessHelper(label: String, authRef: AuthorizationRef) -> Bool {
      
        var error: Unmanaged<CFError>?
        let blessStatus = SMJobBless(kSMDomainSystemLaunchd, label as CFString, authRef, &error)
        
        if !blessStatus {
            NSLog("[SMJBS]: Helper bless failed with error \(error!.takeUnretainedValue())")
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

    guard let representation = image.bestRepresentation(for: frame, context: nil, hints: nil) else {
        return nil
    }
    let image = NSImage(size: newSize, flipped: false, drawingHandler: { (_) -> Bool in
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

    return name as String;
  }
  
  static var rootPathInfo: [String]? = {
    // Since we are running from root, the current user is root so
    // I have to use this to get the actual logged in user
    guard let currentLoggedInUser = getCurLoggedInUserFromRoot() else { return nil }
    let folderName = "keyRemapperMac"

    return [
      "/Users/\(currentLoggedInUser)/\(folderName)",
      "~/\(folderName)"
    ]
  }()

  static func getRootPathInfo() -> [String]? {
    // Since we are running from root, the current user is root so
    // I have to use this to get the actual logged in user
    guard let currentLoggedInUser = getCurLoggedInUserFromRoot() else { return nil }
    let folderName = "keyRemapperMac"

    return [
      "/Users/\(currentLoggedInUser)/\(folderName)",
      "~/\(folderName)"
    ]
  }

  static func getConfigName(configPath: String) -> String? {
    do {
        let data = try Data(contentsOf: URL(fileURLWithPath: configPath), options: .mappedIfSafe)
      let jsonResult = try JSONSerialization.jsonObject(with: data, options: .mutableLeaves)
      if let jsonResult = jsonResult as? Dictionary<String, AnyObject>, let name = jsonResult["name"] as? String {
          return name
      }
    } catch {}

    return nil
  }
  
  static func getConfigPathInfo(configIdx: Int) -> [String]? {
    guard let rootPathInfo = rootPathInfo else {
      return nil
    }

    let idxStr = configIdx == 0 ? "" : String(configIdx + 1)

    return [
      "\(rootPathInfo[0])/config\(idxStr).json",
      "\(rootPathInfo[1])/config\(idxStr).json"
    ]
  }
}
