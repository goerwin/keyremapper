#pragma once

#include <vector>
#include <string>

namespace KeyEvent
{
  using json = nlohmann::json;
  using String = std::string;

  struct Key
  {
    unsigned short code;
    unsigned short state;

    Key() {}

    Key(unsigned short cCode, unsigned short cState = 4)
    {
      code = cCode;
      state = cState;
    }
  };

  struct KeyDown : Key
  {
    KeyDown(unsigned short cCode, unsigned short cState = 0) : Key(cCode, cState) {}
  };

  struct KeyUp : Key
  {
    KeyUp(unsigned short cCode, unsigned short cState = 1) : Key(cCode, cState) {}
  };

  typedef std::vector<Key> Keys;

  class Class
  {
    static short SC_H;
    static short SC_J;
    static short SC_K;
    static short SC_L;
    static short SC_Up;
    static short SC_Down;
    static short SC_Right;
    static short SC_Left;
    static short SC_Home;
    static short SC_End;
    static short SC_Ctrl;
    static short SC_Alt;

    bool isCtrlAsAlt;
    bool isAltAsCtrl;
    json keyDownStatus = {};
    json jsonSchema;

    void print(String string, String string2);
    String getStateSymbol(unsigned short state);
    Keys concatKeyVectors(Keys keys, Keys keys2, Keys keys3, Keys keys4);
    Keys keyDownCtrlAsAlt();
    Keys keyDownCtrlAsCtrl();
    Keys keyUpCtrl();
    Keys keyDownAltAsCtrl();
    Keys keyDownAltAsAlt();
    Keys keyUpAlt();
    short getVimArrowKeyCode(short keyCode);
    short getVimHomeEndKeyCode(short keyCode);
    bool isKeyDown(Key key);
    bool isKeyMatches(json ruleKey, std::string key);
    bool isConditionMatches(json conditions);
    json getFireKeysFromRule(json rule, std::string key);
    json getFireKeys();
    Keys concatKeys(Keys keys, Key key);
    Keys getKeysFromDescription(json keysDescription, short currentKeyCode);
    void printKeys(Keys keys);

  public:
    Class(json jsonSchema);
    Keys getKeyEvents(Keys keys);
  };
}; // namespace KeyEvent
