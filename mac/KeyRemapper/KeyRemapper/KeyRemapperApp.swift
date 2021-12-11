//
//  KeyRemapperApp.swift
//  KeyRemapper
//
//  Created by Erwin Gaitan Ospino on 10/12/21.
//

import SwiftUI

@main
struct KeyRemapperApp: App {
    var body: some Scene {
        WindowGroup {
          ContentView().frame(minWidth: 500, idealWidth: 500, maxWidth: /*@START_MENU_TOKEN@*/.infinity/*@END_MENU_TOKEN@*/, minHeight: 500, idealHeight: 500, maxHeight: .infinity, alignment: /*@START_MENU_TOKEN@*/.center/*@END_MENU_TOKEN@*/)
        }
    }
}

struct KeyRemapperApp_Previews: PreviewProvider {
  static var previews: some View {
    /*@START_MENU_TOKEN@*/Text("Hello, World!")/*@END_MENU_TOKEN@*/
  }
}
