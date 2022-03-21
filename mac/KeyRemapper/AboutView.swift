//
//  AboutView.swift
//  KeyRemapper
//
//  Created by Erwin Gaitan Ospino on 20/03/22.
//

import SwiftUI

struct AboutView: View {
  var body: some View {
    VStack(spacing: 10) {
                Image(nsImage: NSImage(named: "AppIcon")!)
                
                Text("\(bundleName)").font(.system(size: 20, weight: .bold))
                
                Text("Version: \(version)")

//                Text(Bundle.main.copyright)
//                    .font(.system(size: 10, weight: .thin))
//                    .multilineTextAlignment(.center)
            }
            .padding(20)
            .frame(minWidth: 350, minHeight: 300)
      }
}

struct AboutView_Previews: PreviewProvider {
    static var previews: some View {
        AboutView()
    }
}
