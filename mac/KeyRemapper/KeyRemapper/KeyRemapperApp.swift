import SwiftUI

@main
struct KeyRemapperApp: App {
  init() {
    DispatchQueue.global(qos: .userInitiated).async {
      var background: Background?

      background = Background()
    }
  }
  
    var body: some Scene {
        WindowGroup {
          ContentView().frame(minWidth: 200, idealWidth: 200, maxWidth: 200, minHeight: 500, idealHeight: 500, maxHeight: .infinity, alignment: .center)
        }
    }
}

struct KeyRemapperApp_Previews: PreviewProvider {
  static var previews: some View {
    /*@START_MENU_TOKEN@*/Text("Hello, World!")/*@END_MENU_TOKEN@*/
  }
}
