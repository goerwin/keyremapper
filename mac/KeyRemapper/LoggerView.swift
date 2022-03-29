import SwiftUI

struct LoggerView: View {
  @ObservedObject var state: State

  var body: some View {
    VStack(alignment: .leading, spacing: 10) {
      Text("• Most recent events appear at top").bold().italic()
      Text(
        "• Keys: inputCode:inputState -> remappedCode:remappedState -> parsedKeyEvent -> remappedKeyEvent -> keyEventsSent"
      ).bold().italic()
      Divider()
      Text(state.loggerStr)
    }
    .padding(20)
    .frame(
      minWidth: 530, maxWidth: .infinity, minHeight: 530, maxHeight: .infinity,
      alignment: .topLeading)
  }

}
