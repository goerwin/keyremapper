import SecurityFoundation
import ServiceManagement

struct Util {
    static func getPrivilegedHelperAuth() -> AuthorizationRef? {
      var authItem = AuthorizationItem(name: kSMRightBlessPrivilegedHelper, valueLength: 0, value:UnsafeMutableRawPointer(bitPattern: 0), flags: 0)
      var rights = AuthorizationRights(count: 1, items: &authItem)
      
      var authRef: AuthorizationRef?
      AuthorizationCreate(&rights, nil, [.interactionAllowed, .extendRights, .preAuthorize], &authRef)
    
      return authRef
    }
    
    @discardableResult
    static func blessHelper(label: String, authRef: AuthorizationRef) -> Bool {
        var error: Unmanaged<CFError>?
        let blessStatus = SMJobBless(kSMDomainSystemLaunchd, label as CFString, authRef, &error)
        
        if !blessStatus {
            NSLog("[SMJBS]: Helper bless failed with error \(error!.takeUnretainedValue())")
        }
        
        return blessStatus
    }
}
