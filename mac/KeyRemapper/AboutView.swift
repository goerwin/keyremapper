import SwiftUI

struct AboutView: View {
  var body: some View {
    VStack(spacing: 10) {
      Image(nsImage: NSImage(named: "AppIcon")!)

      Text("\(Constants.BUNDLE_NAME)").font(.system(size: 20, weight: .bold)).padding(.bottom, -10)
      Text("\(Constants.VERSION)")  .foregroundStyle(.gray)

      Text("Daemon Service Name").font(.system(size: 10)).foregroundStyle(.gray).padding(.bottom, -10)
      Text("\(Constants.MACH_SERVICE_NAME)")

      Text("\(Constants.OWNER_NAME)").font(.system(size: 10)).foregroundStyle(.blue)
        .onTapGesture {
          NSWorkspace.shared.open(URL(string: "\(Constants.OWNER_WEBSITE)")!)
        }
        .onHover { hovering in
          if hovering {
            NSCursor.pointingHand.push()
          } else {
            NSCursor.pop()
          }
        }
    }
    .padding(20)
    .frame(minWidth: 350, minHeight: 300)
  }
}
