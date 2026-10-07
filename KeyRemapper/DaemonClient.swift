import Foundation
import ServiceManagement

// Talks to the root daemon that does the remapping. It's bundled in the app and registered with
// SMAppService. Calls that wait for an answer time out, so a stuck daemon can't freeze the app
final class DaemonClient {
  enum Registration {
    case ready
    case needsApproval
    case failed(String)
  }

  enum DaemonError: LocalizedError {
    case unreachable
    case wrongVersion(String)
    case noAccessibility
    // The daemon already reported the error (eg. invalid config)
    case reported
    case startFailed(Int)

    var errorDescription: String? {
      switch self {
      case .unreachable: "The Daemon doesn't respond"
      case .wrongVersion(let version):
        "App \(Constants.VERSION) and Daemon \(version) versions don't match, restart the Daemon"
      case .noAccessibility: "Enable Accessibility for \(Constants.BUNDLE_NAME)"
      case .reported: nil
      case .startFailed(let code): "Couldn't start the Daemon. Error \(code)"
      }
    }
  }

  let service = SMAppService.daemon(plistName: "\(Constants.MACH_SERVICE_NAME).plist")

  // Called when the daemon quits or crashes, after that it isn't remapping anymore
  var onDisconnect: (() -> Void)?
  // The daemon stops remapping when it reports an error (eg. a rule that fails)
  var onError: ((String) -> Void)?

  private var connection: NSXPCConnection?
  private lazy var appProvider = AppProviderXPC { [weak self] err in self?.onError?(err) }

  func register() -> Registration {
    if service.status != .enabled && service.status != .requiresApproval {
      do {
        try service.register()
      } catch {
        // register also throws when the daemon is registered but still needs approval
        if service.status != .requiresApproval {
          return .failed("Couldn't register the Daemon: \(error.localizedDescription)")
        }
      }
    }

    return service.status == .requiresApproval ? .needsApproval : .ready
  }

  func unregister() throws {
    if service.status == .notRegistered || service.status == .notFound { return }
    try service.unregister()
  }

  // config and symbols are JSON, with the config imports already resolved
  func start(config: String, symbols: String, profileIdx: Int) throws {
    if let version = version(), version != Constants.VERSION { restart() }

    guard let version = version() else { throw DaemonError.unreachable }
    if version != Constants.VERSION { throw DaemonError.wrongVersion(version) }

    let result = call { proxy, reply in
      proxy.start(config: config, symbols: symbols, profileIdx: profileIdx, withReply: reply)
    }

    switch result {
    case .ok?: return
    case nil: throw DaemonError.unreachable
    case .noAccessibility?: throw DaemonError.noAccessibility
    case .reportedError?: throw DaemonError.reported
    case let result?: throw DaemonError.startFailed(result.rawValue)
    }
  }

  func stop() { proxy?.stop() }
  func kill() { proxy?.kill() }
  func startLogging() { proxy?.startLogging() }
  func stopLogging() { proxy?.stopLogging() }

  private func version() -> String? {
    call(timeout: 2) { proxy, reply in proxy.getVersion(withReply: reply) }
  }

  // After an app update, the daemon from the previous version can still be running.
  // Once it quits, launchd starts the new one on the next connection
  private func restart() {
    // Kept open until the end so the kill message isn't dropped
    let oldConnection = connection
    defer { oldConnection?.invalidate() }
    kill()

    for _ in 0..<20 {
      connection = nil
      if version() == Constants.VERSION { return }
      Thread.sleep(forTimeInterval: 0.1)
    }
  }

  private var proxy: ServiceProviderXPCProtocol? {
    getConnection().remoteObjectProxyWithErrorHandler { _ in } as? ServiceProviderXPCProtocol
  }

  private func call<T>(
    timeout: TimeInterval = 5,
    _ body: (ServiceProviderXPCProtocol, @escaping (T) -> Void) -> Void
  ) -> T? {
    let semaphore = DispatchSemaphore(value: 0)
    var result: T?

    guard
      let proxy = getConnection().remoteObjectProxyWithErrorHandler({ _ in semaphore.signal() })
        as? ServiceProviderXPCProtocol
    else { return nil }

    body(proxy) { value in
      result = value
      semaphore.signal()
    }

    if semaphore.wait(timeout: .now() + timeout) == .timedOut {
      resetConnection()
      return nil
    }
    return result
  }

  private func getConnection() -> NSXPCConnection {
    if let connection { return connection }

    let connection = NSXPCConnection(
      machServiceName: Constants.MACH_SERVICE_NAME, options: [.privileged])
    connection.remoteObjectInterface = NSXPCInterface(with: ServiceProviderXPCProtocol.self)
    connection.exportedInterface = NSXPCInterface(with: AppProviderXPCProtocol.self)
    connection.exportedObject = appProvider

    // Connections replaced on purpose (eg. in restart) don't count as disconnections
    let onClose = { [weak self, weak connection] in
      DispatchQueue.main.async {
        guard let self, let connection, self.connection === connection else { return }
        self.connection = nil
        self.onDisconnect?()
      }
    }
    connection.interruptionHandler = onClose
    connection.invalidationHandler = onClose
    connection.resume()

    self.connection = connection
    return connection
  }

  private func resetConnection() {
    let connection = self.connection
    self.connection = nil
    connection?.invalidate()
  }
}
