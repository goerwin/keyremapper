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
      let clients = Bundle.main.infoDictionary?["SMAuthorizedClients"] as? [String],
      let requirement = clients.first,
      Self.isClientValid(pid: newConnection.processIdentifier, requirement: requirement)
    else { return false }
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
