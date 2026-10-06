import AppKit

struct Global {
  static let state: State = State()

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

  static func getResourceSymbolsPath() -> String? {
    guard let resourcePath = Bundle.main.resourcePath else { return nil }
    return "\(resourcePath)/symbols.json"
  }
}
