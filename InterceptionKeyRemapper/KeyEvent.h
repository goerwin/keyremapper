#pragma once

#include <windows.h>
#include <vector>

enum ScanCodes {
	SC_C = 0x2E,
	SC_D = 0x20,
	SC_E = 0x12,
	SC_H = 0x23,
	SC_I = 0x17,
	SC_J = 0x24,
	SC_K = 0x25,
	SC_L = 0x26,
	SC_M = 0x32,
	SC_P = 0x19,
	SC_Q = 0x10,
	SC_S = 0x1F,
	SC_T = 0x14,
	SC_U = 0x16,
	SC_V = 0x2F,
	SC_W = 0x11,
	SC_X = 0x2D,
	SC_Y = 0x15,
	SC_Z = 0x2C,
	SC_1 = 0x02,
	SC_2 = 0x03,
	SC_3 = 0x04,
	SC_4 = 0x05,
	SC_5 = 0x06,
	SC_6 = 0x07,
	SC_7 = 0x08,
	SC_8 = 0x09,
	SC_9 = 0x0A,
	SC_0 = 0x0B,
	SC_MUTE = 0xA0,
	SC_VOLUMEDOWN = 0xAE,
	SC_VOLUMEUP = 0xB0,
	SC_ESC = 0x01,
	SC_CAPSLOCK = 0x3A,
	SC_LEFT = 0x4B,
	SC_RIGHT = 0x4D,
	SC_UP = 0x48,
	SC_DOWN = 0x50,
	SC_SPACE = 0x39,
	SC_LWIN = 0x5B,
	SC_LALT = 0x38,
	SC_RALT = 0xB8,
	SC_LCTRL = 0x1D,
	SC_RCTRL = 0x9D,
	SC_LSHIFT = 0x2A,
	SC_RSHIFT = 0x36,
	SC_LBSLASH = 0x56,
	SC_RETURN = 0x1C,
	SC_SUPR = 0x53,
	SC_BACK = 0x0E,
	SC_TAB = 0x0F,
	SC_HOME = 0x47,
	SC_END = 0x4F,
	SC_PRIOR = 0x49,
	SC_NEXT = 0x51,
	SC_SEMI = 0x27,
	SC_NP0 = 0x52,
	SC_MINUS = 0x0C,
	SC_GRAVE = 0x29,
	SC_F1 = 0x3B,
	SC_F2 = 0x3C,
	SC_F3 = 0x3D,
	SC_F4 = 0x3E,
	SC_F5 = 0x3F,
	SC_F6 = 0x40,
	SC_F7 = 0x41,
	SC_F8 = 0x42,
	SC_F9 = 0x43,
	SC_F10 = 0x44,
	SC_F11 = 0x57,
	SC_F12 = 0x58,
	SC_MOUSELEFT = 0xF1,
	SC_MOUSERIGHT = 0xF2,
	SC_BRIGHTNESSDOWN = 0xF3,
	SC_BRIGHTNESSUP = 0xF4,
	SC_GENERAL = 0xFF,
	SC_NULL = 0xF0,
	SC_MODE1 = 0xF5,
	SC_MODE2 = 0xF6,
	SC_MODE3 = 0xF7,
	SC_MODE4 = 0xF8,
};

struct Key {
	unsigned short code;
	unsigned short state;

	Key() {}

	Key(unsigned short cCode, unsigned short cState = 4) {
		code = cCode;
		state = cState;
	}
};

typedef std::vector<Key> Keys;
typedef std::string String;

struct KeyDown : Key {
	KeyDown(unsigned short cCode, unsigned short cState = 0) : Key(cCode, cState) {}
};

struct KeyUp : Key {
	KeyUp(unsigned short cCode, unsigned short cState = 1) : Key(cCode, cState) {}
};

namespace KeyEvent {
	Keys getKeyEvents(Keys keys);
	void setActiveProcessName(std::string _activeProcessName);
	void setCustomHotKeysFromFile(String customHotKeysFilePath, String coreHotKeysFilepath = "./hotKeys/_core.md");
	String getScanCodeSymbol(unsigned short code);
	String getStateSymbol(unsigned short state);
	String getKeySymbols(Keys keys);
	bool isKeyDown(Key key);
	Keys getParsedKeysForEsc();
	void initialize(String coreHotKeysFilepath = "./hotKeys/_core.md");
}