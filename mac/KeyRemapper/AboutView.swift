import SwiftUI

struct AboutView: View {
  var body: some View {
    VStack(spacing: 10) {
                Image(nsImage: NSImage(named: "AppIcon")!)
                
                Text("\(BUNDLE_NAME)").font(.system(size: 20, weight: .bold))
                
                Text("Version: \(VERSION)")
            }
            .padding(20)
            .frame(minWidth: 350, minHeight: 300)
      }
}
