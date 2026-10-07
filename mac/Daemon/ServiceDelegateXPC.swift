import Foundation
import Security

class ServiceDelegateXPC: NSObject, NSXPCListenerDelegate {
  func listener(
    _ listener: NSXPCListener,
    shouldAcceptNewConnection newConnection: NSXPCConnection
  )
    -> Bool
  {
    // Only the signed KeyRemapper app can drive this root daemon. The pid check rejects
    // others upfront, setCodeSigningRequirement (audit token based) guards every message
    guard
      let requirement = Self.clientRequirement,
      Self.isClientValid(pid: newConnection.processIdentifier, requirement: requirement)
    else {
      NSLog("Rejected the connection of pid \(newConnection.processIdentifier)")
      // It was launched for this connection (eg. from a client that already quit), so nothing
      // else would stop it
      if GlobalSwift.connection == nil { DispatchQueue.main.async { GlobalSwift.kill() } }
      return false
    }
    newConnection.setCodeSigningRequirement(requirement)

    newConnection.interruptionHandler = {
      DispatchQueue.main.async { GlobalSwift.kill() }
    }

    newConnection.invalidationHandler = {
      DispatchQueue.main.async { GlobalSwift.kill() }
    }

    newConnection.remoteObjectInterface = NSXPCInterface(
      with: AppProviderXPCProtocol.self)
    newConnection.exportedInterface = NSXPCInterface(
      with: ServiceProviderXPCProtocol.self)
    newConnection.exportedObject = ServiceProviderXPC()
    newConnection.resume()

    GlobalSwift.connection = newConnection
    return true
  }

  // The app must be signed by the same team as this daemon
  static let clientRequirement: String? = {
    var code: SecCode?
    var staticCode: SecStaticCode?
    var info: CFDictionary?

    guard
      let clientId = Bundle.main.infoDictionary?["ClientBundleIdentifier"] as? String,
      SecCodeCopySelf([], &code) == errSecSuccess, let code,
      SecCodeCopyStaticCode(code, [], &staticCode) == errSecSuccess, let staticCode,
      SecCodeCopySigningInformation(
        staticCode, SecCSFlags(rawValue: kSecCSSigningInformation), &info) == errSecSuccess,
      let teamId = (info as? [String: Any])?[kSecCodeInfoTeamIdentifier as String] as? String
    else { return nil }

    return
      "anchor apple generic and identifier \"\(clientId)\" and certificate leaf[subject.OU] = \"\(teamId)\""
  }()

  static func isClientValid(pid: pid_t, requirement: String) -> Bool {
    var code: SecCode?
    var secRequirement: SecRequirement?
    let attributes = [kSecGuestAttributePid: pid] as CFDictionary

    guard SecCodeCopyGuestWithAttributes(nil, attributes, [], &code) == errSecSuccess,
      SecRequirementCreateWithString(requirement as CFString, [], &secRequirement)
        == errSecSuccess,
      let code, let secRequirement
    else { return false }

    return SecCodeCheckValidity(code, [], secRequirement) == errSecSuccess
  }
}
