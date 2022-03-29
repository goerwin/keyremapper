import Foundation

class ServiceDelegateXPC : NSObject, NSXPCListenerDelegate {
    func listener(_ listener: NSXPCListener, shouldAcceptNewConnection newConnection: NSXPCConnection) -> Bool {
      newConnection.interruptionHandler = {
        GlobalSwift.kill()
      }
      
      newConnection.invalidationHandler = {
        GlobalSwift.kill()
      }
      
      newConnection.remoteObjectInterface = NSXPCInterface(with: AppProviderXPCProtocol.self)
      newConnection.exportedInterface = NSXPCInterface(with: ServiceProviderXPCProtocol.self)
      newConnection.exportedObject = ServiceProviderXPC()
      newConnection.resume()
      
      GlobalSwift.connection = newConnection
        return true
    }
}
