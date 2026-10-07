import CoreServices
import Foundation

// The config folder (~/keyRemapperMac). Configs can import other files from it, so any JSON
// file saved in it counts as a config change
final class ConfigStore {
  struct Config {
    // With the imports resolved
    let json: String
    // nil for the ones without a name
    let profileNames: [String?]
    let activeProfileIdx: Int?
  }

  let folderPath = FileManager.default.homeDirectoryForCurrentUser
    .appendingPathComponent("keyRemapperMac").path
  var configPath: String { "\(folderPath)/config.json" }

  var onChange: (() -> Void)?

  private var stream: FSEventStreamRef?

  deinit { stopWatching() }

  var exists: Bool { FileManager.default.fileExists(atPath: configPath) }

  // The imports and comments are only understood by the C++ code, the JSON it
  // resolves to is plain
  func load() throws -> Config {
    let json = try ConfigFile.resolve(configPath)
    let object = (try? JSONSerialization.jsonObject(with: Data(json.utf8))) as? [String: Any]
    let profiles = object?["profiles"] as? [Any] ?? []

    return Config(
      json: json, profileNames: profiles.map { ($0 as? [String: Any])?["name"] as? String },
      activeProfileIdx: object?["activeProfileIdx"] as? Int)
  }

  func createIfMissing() throws {
    if exists { return }
    guard let defaultConfig = Bundle.main.path(forResource: "config", ofType: "json") else {
      return
    }

    try FileManager.default.createDirectory(
      atPath: folderPath, withIntermediateDirectories: true)
    try FileManager.default.copyItem(atPath: defaultConfig, toPath: configPath)
    startWatching()
  }

  // Changes are grouped (latency), so an editor saving several files triggers onChange once
  func startWatching() {
    if stream != nil || !FileManager.default.fileExists(atPath: folderPath) { return }

    var context = FSEventStreamContext(
      version: 0, info: Unmanaged.passUnretained(self).toOpaque(), retain: nil, release: nil,
      copyDescription: nil)

    let callback: FSEventStreamCallback = { _, info, count, paths, _, _ in
      guard let info else { return }
      let store = Unmanaged<ConfigStore>.fromOpaque(info).takeUnretainedValue()
      let paths = unsafeBitCast(paths, to: NSArray.self) as? [String] ?? []
      if paths.prefix(count).contains(where: { $0.hasSuffix(".json") }) { store.onChange?() }
    }

    stream = FSEventStreamCreate(
      nil, callback, &context, [folderPath] as CFArray,
      FSEventStreamEventId(kFSEventStreamEventIdSinceNow), 0.3,
      FSEventStreamCreateFlags(
        kFSEventStreamCreateFlagFileEvents | kFSEventStreamCreateFlagUseCFTypes))

    guard let stream else { return }
    FSEventStreamSetDispatchQueue(stream, .main)
    FSEventStreamStart(stream)
  }

  func stopWatching() {
    guard let stream else { return }
    FSEventStreamStop(stream)
    FSEventStreamInvalidate(stream)
    FSEventStreamRelease(stream)
    self.stream = nil
  }
}
