import Foundation

class ServiceDelegateXPC : NSObject, NSXPCListenerDelegate {
    func listener(_ listener: NSXPCListener, shouldAcceptNewConnection newConnection: NSXPCConnection) -> Bool {
        newConnection.exportedInterface = NSXPCInterface(with: ServiceProviderXPCProtocol.self)

      newConnection.interruptionHandler = {
        
      }
      
      newConnection.invalidationHandler = {
        
      }
      
        let exportedObject = ServiceProviderXPC()
        newConnection.exportedObject = exportedObject
        newConnection.resume()
        return true
    }
}
