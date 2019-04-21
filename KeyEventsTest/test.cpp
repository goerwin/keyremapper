#pragma once

#ifndef UNICODE
#define UNICODE
#endif

#include "pch.h"
#include <vector>
#include <windows.h>
#include "../InterceptionKeyRemapper/interception.h"
#include "../InterceptionKeyRemapper/KeyEvent.cpp"
#include "../InterceptionKeyRemapper/erwinUtils.cpp"

String filepath = "../testCases.md";

class KeyEventTest : public ::testing::Test {
 protected:
	// You can do set-up work for each test here.
	KeyEventTest() {}

	// You can do clean-up work t doesn't throw exceptions here.
	~KeyEventTest() override {}

	// If the constructor and destructor arot enough for setting up
	// and cleaning up each test, you can define the following methods:

	// Code here will be called immediately after the constructor (right before each test).
	void SetUp() override {
		setGlobalDefaultValues();
	}

	// Code here will be called immediately after each test (right before the destructor).
	void TearDown() override {}

	static std::vector<Key> removeNullKeyEvents(std::vector<Key> keyEvents) {
		std::vector<Key> newKeyEvents;
		int size = keyEvents.size();

		for (int i = 0; i < size; i++) {
			if (keyEvents[i].code != SC_NULL) {
				newKeyEvents.insert(newKeyEvents.end(), keyEvents[i]);
			}
		}

		return newKeyEvents;
	}

	static bool compareKeyEvents(std::vector<Key> keyEvents, std::vector<Key> keyEvents2) {
		int size1 = keyEvents.size();
		int size2 = keyEvents2.size();

		if (size1 != size2) {
			return false;
		}

		try {
			for (int i = 0; i < size2; i++) {
				if (
					keyEvents[i].code != keyEvents2[i].code ||
					keyEvents[i].state != keyEvents2[i].state
					) {
					return false;
				}
			}

			return true;
		} catch (const std::exception&) {
			return false;
		}
	}

	static std::vector<std::pair<String, String>> KeyEventTest::outputSection;

	static bool validateKeyMapsAndOutputThem(
		String title,
		std::pair<Keys, Keys> keys,
		String comment = ""
	) {
		auto inputKeys = keys.first;
		auto convertedInputKeys = removeNullKeyEvents(getKeyEvents(inputKeys));
		auto expectedKeys = removeNullKeyEvents(keys.second);
		auto result = compareKeyEvents(convertedInputKeys, expectedKeys);

		if (result) {
			String symbolMappedKeys = getKeySymbols(inputKeys);
			symbolMappedKeys = symbolMappedKeys
				.append(" = ")
				.append(getKeySymbols(convertedInputKeys))
				.append(comment != "" ? String(" // ").append(comment) : "")
				.append("\n");

			auto outputSectionSize = outputSection.size();
			bool isSectionFound = false;
			for (int i = 0; i < outputSectionSize; i++) {
				if (outputSection[i].first == title) {
					isSectionFound = true;
					outputSection[i].second = outputSection[i].second.append(symbolMappedKeys);
				}
			}
			if (!isSectionFound) {
				outputSection.insert(outputSection.end(), { title, symbolMappedKeys });
			}

			outputSectionSize = outputSection.size();
			String sections = "";
			for (int i = 0; i < outputSectionSize; i++) {
				sections = sections.append("### ")
					.append(outputSection[i].first)
					.append("\n```\n")
					.append(outputSection[i].second)
					.append("```\n")
					.append(i == outputSectionSize - 1 ? "" : "\n");
			}
			sections = String("# Test Cases\n\n").append(sections);
			ErwinUtils::writeToFile(filepath, sections, false, false);
		}

		return result;
	}
};

std::vector<std::pair<String, String>> KeyEventTest::outputSection;

// Mouse events

TEST_F(KeyEventTest, MOUSE_LEFT_CLICK) {
	EXPECT_TRUE(validateKeyMapsAndOutputThem("Mouse", {
		{
			KeyDown(SC_LWIN),
			KeyDown(SC_C),
			KeyUp(SC_C),
			KeyUp(SC_LWIN)
		},
		{
			KeyDown(SC_MOUSELEFT),
			KeyUp(SC_MOUSELEFT)
		}
	}));
}

TEST_F(KeyEventTest, MOUSE_RIGHT_CLICK) {
	EXPECT_TRUE(validateKeyMapsAndOutputThem("Mouse", {
		{
			KeyDown(SC_LWIN),
			KeyDown(SC_CAPSLOCK),
			KeyDown(SC_C),
			KeyUp(SC_C),
			KeyUp(SC_CAPSLOCK),
			KeyUp(SC_LWIN)
		},
		{
			KeyDown(SC_MOUSERIGHT),
			KeyUp(SC_MOUSERIGHT)
		}
	}));
}

TEST_F(KeyEventTest, MOUSE_LEFT_CLICK_HOLD) {
	EXPECT_TRUE(validateKeyMapsAndOutputThem("Mouse", {
		{
			KeyDown(SC_LWIN),
			KeyDown(SC_C),
			KeyDown(SC_C),
			KeyDown(SC_C),
			KeyDown(SC_C),
			KeyUp(SC_C),
			KeyUp(SC_LWIN)
		},
		{
			KeyDown(SC_MOUSELEFT),
			KeyUp(SC_MOUSELEFT)
		}
	}));
}

TEST_F(KeyEventTest, MOUSE_RIGHT_CLICK_HOLD) {
	EXPECT_TRUE(validateKeyMapsAndOutputThem("Mouse", {
		{
			KeyDown(SC_LWIN),
			KeyDown(SC_CAPSLOCK),
			KeyDown(SC_C),
			KeyDown(SC_C),
			KeyDown(SC_C),
			KeyDown(SC_C),
			KeyUp(SC_C),
			KeyDown(SC_CAPSLOCK),
			KeyUp(SC_LWIN)
		},
		{
			KeyDown(SC_MOUSERIGHT),
			KeyUp(SC_MOUSERIGHT)
		}
	}));
}

TEST_F(KeyEventTest, MOUSE_LSHIFT_LEFT_CLICK) {
	EXPECT_TRUE(validateKeyMapsAndOutputThem("Mouse", {
		{
			KeyDown(SC_LWIN),
			KeyDown(SC_LSHIFT),
			KeyDown(SC_C),
			KeyUp(SC_C),
			KeyUp(SC_LSHIFT),
			KeyUp(SC_LWIN)
		},
		{
			KeyDown(SC_LSHIFT),
			KeyDown(SC_MOUSELEFT),
			KeyUp(SC_MOUSELEFT),
			KeyUp(SC_LSHIFT)
		}
	}));
}

TEST_F(KeyEventTest, MOUSE_LALT_LEFT_CLICK) {
	EXPECT_TRUE(validateKeyMapsAndOutputThem("Mouse", {
		{
			KeyDown(SC_LWIN),
			KeyDown(SC_LALT),
			KeyDown(SC_C),
			KeyUp(SC_C),
			KeyUp(SC_LALT),
			KeyUp(SC_LWIN)
		},
		{
			KeyDown(SC_LCTRL),
			KeyDown(SC_MOUSELEFT),
			KeyUp(SC_MOUSELEFT),
			KeyUp(SC_LCTRL)
		}
	}));
	EXPECT_TRUE(validateKeyMapsAndOutputThem("Mouse", {
		{
			KeyDown(SC_LALT),
			KeyDown(SC_LWIN),
			KeyDown(SC_C),
			KeyUp(SC_C),
			KeyUp(SC_LALT),
			KeyUp(SC_LWIN)
		},
		{
			KeyDown(SC_LCTRL),
			KeyDown(SC_MOUSELEFT),
			KeyUp(SC_MOUSELEFT),
			KeyUp(SC_LCTRL)
		}
	}));
}

TEST_F(KeyEventTest, KEY) {
	EXPECT_TRUE(validateKeyMapsAndOutputThem("Key", {
		{ KeyDown(SC_H), KeyUp(SC_H) },
		{ KeyDown(SC_H), KeyUp(SC_H) }
	}));
}

TEST_F(KeyEventTest, KEY_DOWN) {
	EXPECT_TRUE(validateKeyMapsAndOutputThem("Key", {
		{ KeyDown(SC_H), KeyDown(SC_H), KeyDown(SC_H) },
		{ KeyDown(SC_H), KeyDown(SC_H), KeyDown(SC_H) }
	}));
}

TEST_F(KeyEventTest, KEY_DOWN_THEN_UP) {
	EXPECT_TRUE(validateKeyMapsAndOutputThem("Key", {
		{ KeyDown(SC_H), KeyDown(SC_H), KeyUp(SC_H) },
		{ KeyDown(SC_H), KeyDown(SC_H), KeyUp(SC_H) }
	}));
}

TEST_F(KeyEventTest, KEY_MULTIPLE_KEYS_2KEYS_IN_ORDER) {
	EXPECT_TRUE(validateKeyMapsAndOutputThem("Key", {
		{ KeyDown(SC_H), KeyDown(SC_J), KeyUp(SC_J), KeyUp(SC_H) },
		{ KeyDown(SC_H), KeyDown(SC_J), KeyUp(SC_J), KeyUp(SC_H) }
	}));
}

TEST_F(KeyEventTest, KEY_MULTIPLE_KEYS_2KEYS_NO_ORDER) {
	EXPECT_TRUE(validateKeyMapsAndOutputThem("Key", {
		{ KeyDown(SC_H), KeyDown(SC_J), KeyUp(SC_H), KeyUp(SC_J) },
		{ KeyDown(SC_H), KeyDown(SC_J), KeyUp(SC_H), KeyUp(SC_J) }
	}));
}

TEST_F(KeyEventTest, KEY_MULTIPLE_KEYS_3KEYS_IN_ORDER) {
	EXPECT_TRUE(validateKeyMapsAndOutputThem("Key", {
		{
			KeyDown(SC_H),
			KeyDown(SC_J),
			KeyDown(SC_K),
			KeyUp(SC_K),
			KeyUp(SC_J),
			KeyUp(SC_H)
		},
		{
			KeyDown(SC_H),
			KeyDown(SC_J),
			KeyDown(SC_K),
			KeyUp(SC_K),
			KeyUp(SC_J),
			KeyUp(SC_H)
		}
	}));
}

TEST_F(KeyEventTest, KEY_MULTIPLE_KEYS_3KEYS_NO_ORDER) {
	EXPECT_TRUE(validateKeyMapsAndOutputThem("Key", {
		{
			KeyDown(SC_H),
			KeyDown(SC_J),
			KeyDown(SC_K),
			KeyUp(SC_K),
			KeyUp(SC_H),
			KeyUp(SC_J)

		},
		{
			KeyDown(SC_H),
			KeyDown(SC_J),
			KeyDown(SC_K),
			KeyUp(SC_K),
			KeyUp(SC_H),
			KeyUp(SC_J)
		}
	}));
}

TEST_F(KeyEventTest, VIM_MODE_ARROWKEYS) {
	std::vector<ScanCodes> keys = { SC_H, SC_L, SC_J, SC_K };
	std::vector<ScanCodes> arrowKeys = { SC_LEFT, SC_RIGHT, SC_DOWN, SC_UP };
	int size = keys.size();

	for (int i = 0; i < size; i++) {
		EXPECT_TRUE(validateKeyMapsAndOutputThem("Vim Mode", {
			{
				KeyDown(SC_CAPSLOCK),
				KeyDown(keys[i]),
				KeyUp(keys[i]),
				KeyUp(SC_CAPSLOCK)
			},
			{
				Key(arrowKeys[i])
			}
		}));
	}
}

TEST_F(KeyEventTest, VIM_MODE_ARROWKEYS_REPEAT) {
	std::vector<ScanCodes> keys = { SC_H, SC_L, SC_J, SC_K };
	std::vector<ScanCodes> arrowKeys = { SC_LEFT, SC_RIGHT, SC_DOWN, SC_UP };
	int size = keys.size();

	for (int i = 0; i < size; i++) {
		EXPECT_TRUE(validateKeyMapsAndOutputThem("Vim Mode", {
			{
				KeyDown(SC_CAPSLOCK),
				KeyDown(keys[i]),
				KeyUp(keys[i]),
				KeyDown(keys[i]),
				KeyUp(keys[i]),
				KeyUp(SC_CAPSLOCK)
			},
			{
				Key(arrowKeys[i]),
				Key(arrowKeys[i])
			}
		}));
	}
}

TEST_F(KeyEventTest, VIM_MODE_ARROWKEYS_IN_APP_SWITCHER) {
	std::vector<ScanCodes> keys = { SC_H, SC_L, SC_J, SC_K };
	std::vector<ScanCodes> arrowKeys = { SC_LEFT, SC_RIGHT, SC_DOWN, SC_UP };
	int size = keys.size();

	// Get in AppSwitcher
	getKeyEvents({
		KeyDown(SC_LALT),
		KeyDown(SC_TAB),
		KeyUp(SC_TAB)
	});

	for (int i = 0; i < size; i++) {
		EXPECT_TRUE(validateKeyMapsAndOutputThem("Vim Mode", {
			{
				KeyDown(SC_CAPSLOCK),
				KeyDown(keys[i]),
				KeyUp(keys[i]),
				KeyUp(SC_CAPSLOCK)
			},
			{
				Key(arrowKeys[i])
			}
		}, "AppSwitcher"));
	}
}

TEST_F(KeyEventTest, VIM_MODE_ARROWKEYS_IN_APP_SWITCHER_REPEAT) {
	std::vector<ScanCodes> keys = { SC_H, SC_L, SC_J, SC_K };
	std::vector<ScanCodes> arrowKeys = { SC_LEFT, SC_RIGHT, SC_DOWN, SC_UP };
	int size = keys.size();

	// Get in AppSwitcher (getKeyEvents modifies state)
	getKeyEvents({
		KeyDown(SC_LALT),
		KeyDown(SC_TAB),
		KeyUp(SC_TAB)
	});

	for (int i = 0; i < size; i++) {
		EXPECT_TRUE(validateKeyMapsAndOutputThem("Vim Mode", {
			{
				KeyDown(SC_CAPSLOCK),
				KeyDown(keys[i]),
				KeyUp(keys[i]),
				KeyDown(keys[i]),
				KeyUp(keys[i]),
				KeyUp(SC_CAPSLOCK)
			},
			{
				Key(arrowKeys[i]),
				Key(arrowKeys[i])
			}
		}, "AppSwitcher"));
	}
}

TEST_F(KeyEventTest, VIM_MODE_LSHIFT_ARROWKEYS) {
	std::vector<ScanCodes> keys = { SC_H, SC_L, SC_J, SC_K };
	std::vector<ScanCodes> vimShiftKeys = { SC_S, SC_LSHIFT };
	std::vector<ScanCodes> arrowKeys = { SC_LEFT, SC_RIGHT, SC_DOWN, SC_UP };
	int size = keys.size();
	int vimShiftsize = vimShiftKeys.size();

	for (int i = 0; i < size; i++) {
		for (int j = 0; j < vimShiftsize; j++) {
			EXPECT_TRUE(validateKeyMapsAndOutputThem("Vim Mode", {
				{
					KeyDown(SC_CAPSLOCK),
					KeyDown(vimShiftKeys[j]),
					KeyDown(keys[i]),
					KeyUp(keys[i]),
					KeyUp(vimShiftKeys[j]),
					KeyUp(SC_CAPSLOCK)
				},
				{
					KeyDown(SC_LSHIFT),
					Key(arrowKeys[i]),
					KeyUp(SC_LSHIFT)
				}
			}));
		}
	}
}

TEST_F(KeyEventTest, VIM_MODE_LSHIFT_ARROWKEYS_REPEAT) {
	std::vector<ScanCodes> keys = { SC_H, SC_L, SC_J, SC_K };
	std::vector<ScanCodes> vimShiftKeys = { SC_S, SC_LSHIFT };
	std::vector<ScanCodes> arrowKeys = { SC_LEFT, SC_RIGHT, SC_DOWN, SC_UP };
	int size = keys.size();
	int vimShiftsize = vimShiftKeys.size();

	for (int i = 0; i < size; i++) {
		for (int j = 0; j < vimShiftsize; j++) {
			EXPECT_TRUE(validateKeyMapsAndOutputThem("Vim Mode", {
				{
					KeyDown(SC_CAPSLOCK),
					KeyDown(vimShiftKeys[j]),
					KeyDown(keys[i]),
					KeyUp(keys[i]),
					KeyDown(keys[i]),
					KeyUp(keys[i]),
					KeyUp(vimShiftKeys[j]),
					KeyUp(SC_CAPSLOCK)
				},
				{
					KeyDown(SC_LSHIFT),
					Key(arrowKeys[i]),
					Key(arrowKeys[i]),
					KeyUp(SC_LSHIFT)
				}
			}));
		}
	}
}

TEST_F(KeyEventTest, VIM_MODE_LWIN_ARROWKEYS) {
	std::vector<ScanCodes> keys = { SC_H, SC_L, SC_J, SC_K };
	std::vector<ScanCodes> arrowKeys = { SC_LEFT, SC_RIGHT, SC_DOWN, SC_UP };
	int size = keys.size();

	for (int i = 0; i < size; i++) {
		EXPECT_TRUE(validateKeyMapsAndOutputThem("Vim Mode", {
			{
				KeyDown(SC_CAPSLOCK),
				KeyDown(SC_LWIN),
				KeyDown(keys[i]),
				KeyUp(keys[i]),
				KeyUp(SC_LWIN),
				KeyUp(SC_CAPSLOCK)
			},
			{
				KeyDown(SC_LCTRL),
				Key(arrowKeys[i]),
				KeyUp(SC_LCTRL)
			}
		}));
	}
}

TEST_F(KeyEventTest, VIM_MODE_LWIN_ARROWKEYS_REPEAT) {
	std::vector<ScanCodes> keys = { SC_H, SC_L, SC_J, SC_K };
	std::vector<ScanCodes> arrowKeys = { SC_LEFT, SC_RIGHT, SC_DOWN, SC_UP };
	int size = keys.size();

	for (int i = 0; i < size; i++) {
		EXPECT_TRUE(validateKeyMapsAndOutputThem("Vim Mode", {
			{
				KeyDown(SC_CAPSLOCK),
				KeyDown(SC_LWIN),
				KeyDown(keys[i]),
				KeyUp(keys[i]),
				KeyDown(keys[i]),
				KeyUp(keys[i]),
				KeyUp(SC_LWIN),
				KeyUp(SC_CAPSLOCK)
			},
			{
				KeyDown(SC_LCTRL),
				Key(arrowKeys[i]),
				KeyUp(SC_LCTRL),
				KeyDown(SC_LCTRL),
				Key(arrowKeys[i]),
				KeyUp(SC_LCTRL)
			}
		}));
	}
}

TEST_F(KeyEventTest, VIM_MODE_LWIN_SHIFT_ARROWKEYS) {
	std::vector<ScanCodes> keys = { SC_H, SC_L, SC_J, SC_K };
	std::vector<ScanCodes> vimShiftKeys = { SC_S, SC_LSHIFT };
	std::vector<ScanCodes> arrowKeys = { SC_LEFT, SC_RIGHT, SC_DOWN, SC_UP };
	int size = keys.size();
	int vimShiftsize = vimShiftKeys.size();

	for (int i = 0; i < size; i++) {
		for (int j = 0; j < vimShiftsize; j++) {
			EXPECT_TRUE(validateKeyMapsAndOutputThem("Vim Mode", {
				{
					KeyDown(SC_CAPSLOCK),
					KeyDown(vimShiftKeys[j]),
					KeyDown(SC_LWIN),
					KeyDown(keys[i]),
					KeyUp(keys[i]),
					KeyUp(SC_LWIN),
					KeyUp(vimShiftKeys[j]),
					KeyUp(SC_CAPSLOCK)
				},
				{
					KeyDown(SC_LSHIFT),
					KeyDown(SC_LCTRL),
					Key(arrowKeys[i]),
					KeyUp(SC_LCTRL),
					KeyUp(SC_LSHIFT)
				}
			}));
		}
	}
}

TEST_F(KeyEventTest, VIM_MODE_LWIN_SHIFT_ARROWKEYS_REPEAT) {
	std::vector<ScanCodes> keys = { SC_H, SC_L, SC_J, SC_K };
	std::vector<ScanCodes> vimShiftKeys = { SC_S, SC_LSHIFT };
	std::vector<ScanCodes> arrowKeys = { SC_LEFT, SC_RIGHT, SC_DOWN, SC_UP };
	int size = keys.size();
	int vimShiftsize = vimShiftKeys.size();

	for (int i = 0; i < size; i++) {
		for (int j = 0; j < vimShiftsize; j++) {
			EXPECT_TRUE(validateKeyMapsAndOutputThem("Vim Mode", {
				{
					KeyDown(SC_CAPSLOCK),
					KeyDown(vimShiftKeys[j]),
					KeyDown(SC_LWIN),
					KeyDown(keys[i]),
					KeyUp(keys[i]),
					KeyDown(keys[i]),
					KeyUp(keys[i]),
					KeyUp(SC_LWIN),
					KeyUp(vimShiftKeys[j]),
					KeyUp(SC_CAPSLOCK)
				},
				{
					KeyDown(SC_LSHIFT),
					KeyDown(SC_LCTRL),
					Key(arrowKeys[i]),
					KeyUp(SC_LCTRL),
					KeyDown(SC_LCTRL),
					Key(arrowKeys[i]),
					KeyUp(SC_LCTRL),
					KeyUp(SC_LSHIFT)
				}
			}));
		}
	}
}

TEST_F(KeyEventTest, VIM_MODE_LALT_ARROWKEYS_HL) {
	std::vector<ScanCodes> keys = { SC_H, SC_L };
	std::vector<ScanCodes> arrowKeys = { SC_HOME, SC_END };
	int size = keys.size();

	for (int i = 0; i < size; i++) {
		EXPECT_TRUE(validateKeyMapsAndOutputThem("Vim Mode", {
			{
				KeyDown(SC_CAPSLOCK),
				KeyDown(SC_LALT),
				KeyDown(keys[i]),
				KeyUp(keys[i]),
				KeyUp(SC_LALT),
				KeyUp(SC_CAPSLOCK)
			},
			{
				KeyDown(SC_LCTRL),
				KeyUp(SC_LCTRL),
				Key(arrowKeys[i]),
				KeyDown(SC_LCTRL),
				KeyUp(SC_LCTRL)
			}
		}));
	}
}

TEST_F(KeyEventTest, VIM_MODE_LALT_ARROWKEYS_HL_REPEAT) {
	std::vector<ScanCodes> keys = { SC_H, SC_L };
	std::vector<ScanCodes> arrowKeys = { SC_HOME, SC_END };
	int size = keys.size();

	for (int i = 0; i < size; i++) {
		EXPECT_TRUE(validateKeyMapsAndOutputThem("Vim Mode", {
			{
				KeyDown(SC_CAPSLOCK),
				KeyDown(SC_LALT),
				KeyDown(keys[i]),
				KeyUp(keys[i]),
				KeyDown(keys[i]),
				KeyUp(keys[i]),
				KeyUp(SC_LALT),
				KeyUp(SC_CAPSLOCK)
			},
			{
				KeyDown(SC_LCTRL),
				KeyUp(SC_LCTRL),
				Key(arrowKeys[i]),
				KeyDown(SC_LCTRL),
				KeyUp(SC_LCTRL),
				Key(arrowKeys[i]),
				KeyDown(SC_LCTRL),
				KeyUp(SC_LCTRL)
			}
		}));
	}
}

TEST_F(KeyEventTest, VIM_MODE_LALT_SHIFT_ARROWKEYS_HL) {
	std::vector<ScanCodes> keys = { SC_H, SC_L };
	std::vector<ScanCodes> arrowKeys = { SC_HOME, SC_END };
	std::vector<ScanCodes> vimShiftKeys = { SC_S, SC_LSHIFT };
	int size = keys.size();
	int vimShiftsize = vimShiftKeys.size();

	for (int i = 0; i < size; i++) {
		for (int j = 0; j < vimShiftsize; j++) {
			EXPECT_TRUE(validateKeyMapsAndOutputThem("Vim Mode", {
				{
					KeyDown(SC_CAPSLOCK),
					KeyDown(vimShiftKeys[j]),
					KeyDown(SC_LALT),
					KeyDown(keys[i]),
					KeyUp(keys[i]),
					KeyUp(SC_LALT),
					KeyUp(vimShiftKeys[j]),
					KeyUp(SC_CAPSLOCK)
				},
				{
					KeyDown(SC_LSHIFT),
					KeyDown(SC_LCTRL),
					KeyUp(SC_LCTRL),
					Key(arrowKeys[i]),
					KeyDown(SC_LCTRL),
					KeyUp(SC_LCTRL),
					KeyUp(SC_LSHIFT)
				}
			}));
		}
	}
}

TEST_F(KeyEventTest, VIM_MODE_LALT_SHIFT_ARROWKEYS_HL_REPEAT) {
	std::vector<ScanCodes> keys = { SC_H, SC_L };
	std::vector<ScanCodes> arrowKeys = { SC_HOME, SC_END };
	std::vector<ScanCodes> vimShiftKeys = { SC_S, SC_LSHIFT };
	int size = keys.size();
	int vimShiftsize = vimShiftKeys.size();

	for (int i = 0; i < size; i++) {
		for (int j = 0; j < vimShiftsize; j++) {
			EXPECT_TRUE(validateKeyMapsAndOutputThem("Vim Mode", {
				{
					KeyDown(SC_CAPSLOCK),
					KeyDown(vimShiftKeys[j]),
					KeyDown(SC_LALT),
					KeyDown(keys[i]),
					KeyUp(keys[i]),
					KeyDown(keys[i]),
					KeyUp(keys[i]),
					KeyUp(SC_LALT),
					KeyUp(vimShiftKeys[j]),
					KeyUp(SC_CAPSLOCK)
				},
				{
					KeyDown(SC_LSHIFT),
					KeyDown(SC_LCTRL),
					KeyUp(SC_LCTRL),
					Key(arrowKeys[i]),
					KeyDown(SC_LCTRL),
					KeyUp(SC_LCTRL),
					Key(arrowKeys[i]),
					KeyDown(SC_LCTRL),
					KeyUp(SC_LCTRL),
					KeyUp(SC_LSHIFT)
				}
			}));
		}
	}
}

TEST_F(KeyEventTest, VIM_MODE_LALT_ARROWKEYS_JK) {
	std::vector<ScanCodes> keys = { SC_J, SC_K };
	std::vector<ScanCodes> arrowKeys = { SC_END, SC_HOME };
	int size = keys.size();

	for (int i = 0; i < size; i++) {
		EXPECT_TRUE(validateKeyMapsAndOutputThem("Vim Mode", {
			{
				KeyDown(SC_CAPSLOCK),
				KeyDown(SC_LALT),
				KeyDown(keys[i]),
				KeyUp(keys[i]),
				KeyUp(SC_LALT),
				KeyUp(SC_CAPSLOCK)
			},
			{
				KeyDown(SC_LCTRL),
				Key(arrowKeys[i]),
				KeyUp(SC_LCTRL)
			}
		}));
	}
}

TEST_F(KeyEventTest, VIM_MODE_LALT_ARROWKEYS_JK_REPEAT) {
	std::vector<ScanCodes> keys = { SC_J, SC_K };
	std::vector<ScanCodes> arrowKeys = { SC_END, SC_HOME };
	int size = keys.size();

	for (int i = 0; i < size; i++) {
		EXPECT_TRUE(validateKeyMapsAndOutputThem("Vim Mode", {
			{
				KeyDown(SC_CAPSLOCK),
				KeyDown(SC_LALT),
				KeyDown(keys[i]),
				KeyUp(keys[i]),
				KeyDown(keys[i]),
				KeyUp(keys[i]),
				KeyUp(SC_LALT),
				KeyUp(SC_CAPSLOCK)
			},
			{
				KeyDown(SC_LCTRL),
				Key(arrowKeys[i]),
				Key(arrowKeys[i]),
				KeyUp(SC_LCTRL)
			}
		}));
	}
}

TEST_F(KeyEventTest, VIM_MODE_LALT_SHIFT_ARROWKEYS_JK) {
	std::vector<ScanCodes> keys = { SC_J, SC_K };
	std::vector<ScanCodes> arrowKeys = { SC_END, SC_HOME };
	std::vector<ScanCodes> vimShiftKeys = { SC_S, SC_LSHIFT };
	int size = keys.size();
	int vimShiftsize = vimShiftKeys.size();

	for (int i = 0; i < size; i++) {
		for (int j = 0; j < vimShiftsize; j++) {
			EXPECT_TRUE(validateKeyMapsAndOutputThem("Vim Mode", {
				{
					KeyDown(SC_CAPSLOCK),
					KeyDown(vimShiftKeys[j]),
					KeyDown(SC_LALT),
					KeyDown(keys[i]),
					KeyUp(keys[i]),
					KeyUp(SC_LALT),
					KeyUp(vimShiftKeys[j]),
					KeyUp(SC_CAPSLOCK)
				},
				{
					KeyDown(SC_LSHIFT),
					KeyDown(SC_LCTRL),
					Key(arrowKeys[i]),
					KeyUp(SC_LCTRL),
					KeyUp(SC_LSHIFT)
				}
			}));
		}
	}
}

TEST_F(KeyEventTest, VIM_MODE_LALT_SHIFT_ARROWKEYS_JK_REPEAT) {
	std::vector<ScanCodes> keys = { SC_J, SC_K };
	std::vector<ScanCodes> arrowKeys = { SC_END, SC_HOME };
	std::vector<ScanCodes> vimShiftKeys = { SC_S, SC_LSHIFT };
	int size = keys.size();
	int vimShiftsize = vimShiftKeys.size();

	for (int i = 0; i < size; i++) {
		for (int j = 0; j < vimShiftsize; j++) {
			EXPECT_TRUE(validateKeyMapsAndOutputThem("Vim Mode", {
				{
					KeyDown(SC_CAPSLOCK),
					KeyDown(vimShiftKeys[j]),
					KeyDown(SC_LALT),
					KeyDown(keys[i]),
					KeyUp(keys[i]),
					KeyDown(keys[i]),
					KeyUp(keys[i]),
					KeyUp(SC_LALT),
					KeyUp(vimShiftKeys[j]),
					KeyUp(SC_CAPSLOCK)
				},
				{
					KeyDown(SC_LSHIFT),
					KeyDown(SC_LCTRL),
					Key(arrowKeys[i]),
					Key(arrowKeys[i]),
					KeyUp(SC_LCTRL),
					KeyUp(SC_LSHIFT)
				}
			}));
		}
	}
}

TEST_F(KeyEventTest, LALT_GRAVE_THEN_TAB) {
	EXPECT_TRUE(validateKeyMapsAndOutputThem("LAlt", {
		{
			KeyDown(SC_LALT),
			KeyDown(SC_GRAVE),
			KeyUp(SC_GRAVE),
			KeyDown(SC_TAB),
			KeyUp(SC_TAB),
			KeyUp(SC_LALT)
		},
		{
			KeyDown(SC_LCTRL),
			KeyUp(SC_LCTRL),
			KeyDown(SC_LALT),
			Key(SC_GRAVE),
			KeyUp(SC_LALT),
			KeyDown(SC_LCTRL),
			KeyUp(SC_LCTRL),
			KeyDown(SC_LALT),
			Key(SC_TAB),
			KeyUp(SC_LALT)
		}
	}));
}

TEST_F(KeyEventTest, LALT_TAB_THEN_Q) {
	EXPECT_TRUE(validateKeyMapsAndOutputThem("LAlt", {
		{
			KeyDown(SC_LALT),
			KeyDown(SC_TAB),
			KeyUp(SC_TAB),
			KeyDown(SC_Q),
			KeyUp(SC_Q),
			KeyUp(SC_LALT)
		},
		{
			KeyDown(SC_LCTRL),
			KeyUp(SC_LCTRL),
			KeyDown(SC_LALT),
			Key(SC_TAB),
			Key(SC_SUPR),
			KeyUp(SC_LALT)
		}
	}));
}

TEST_F(KeyEventTest, LALT_TAB_THEN_ESC) {
	EXPECT_TRUE(validateKeyMapsAndOutputThem("LAlt", {
		{
			KeyDown(SC_LALT),
			KeyDown(SC_TAB),
			KeyUp(SC_TAB),
			KeyDown(SC_ESC),
			KeyUp(SC_ESC),
			KeyUp(SC_LALT)
		},
		{
			KeyDown(SC_LCTRL),
			KeyUp(SC_LCTRL),
			KeyDown(SC_LALT),
			Key(SC_TAB),
			Key(SC_ESC),
			KeyUp(SC_LALT),
			KeyDown(SC_LCTRL),
			KeyUp(SC_LCTRL)
		}
	}));
}

TEST_F(KeyEventTest, LALT_LSHIFT_TAB) {
	EXPECT_TRUE(validateKeyMapsAndOutputThem("LAlt", {
		{
			KeyDown(SC_LALT),
			KeyDown(SC_LSHIFT),
			KeyDown(SC_TAB),
			KeyUp(SC_TAB),
			KeyDown(SC_TAB),
			KeyUp(SC_TAB),
			KeyUp(SC_LSHIFT),
			KeyUp(SC_LALT)
		},
		{
			KeyDown(SC_LCTRL),
			KeyDown(SC_LSHIFT),
			KeyUp(SC_LCTRL),
			KeyDown(SC_LALT),
			Key(SC_TAB),
			KeyDown(SC_LALT),
			Key(SC_TAB),
			KeyUp(SC_LSHIFT),
			KeyUp(SC_LALT)
		}
	}));
}

TEST_F(KeyEventTest, LCTRL_TAB) {
	EXPECT_TRUE(validateKeyMapsAndOutputThem("LCtrl", {
		{
			KeyDown(SC_LCTRL),
			KeyDown(SC_TAB),
			KeyUp(SC_TAB),
			KeyUp(SC_LCTRL)
		},
		{
			KeyDown(SC_LALT),
			KeyUp(SC_LALT),
			KeyDown(SC_LCTRL),
			Key(SC_TAB),
			KeyUp(SC_LCTRL)
		}
	}));
}

TEST_F(KeyEventTest, LCTRL_OR_LALT_Letter) {
	std::vector<ScanCodes> keys = { SC_LCTRL, SC_LALT };
	std::vector<ScanCodes> switchedKeys = { SC_LALT, SC_LCTRL };
	int size = keys.size();

	for (int i = 0; i < size; i++) {
		EXPECT_TRUE(validateKeyMapsAndOutputThem("LCtrl or LAlt", {
			{
				KeyDown(keys[i]),
				KeyDown(SC_C),
				KeyUp(SC_C),
				KeyUp(keys[i])
			},
			{
				KeyDown(switchedKeys[i]),
				KeyDown(SC_C),
				KeyUp(SC_C),
				KeyUp(switchedKeys[i])
			}
		}));

		EXPECT_TRUE(validateKeyMapsAndOutputThem("LCtrl or LAlt", {
			{
				KeyDown(keys[i]),
				KeyDown(SC_C)
			},
			{
				KeyDown(switchedKeys[i]),
				KeyDown(SC_C)
			}
		}));
	}
}

TEST_F(KeyEventTest, LCTRL_OR_LALT_LSHIFT) {
	std::vector<ScanCodes> keys = { SC_LCTRL, SC_LALT };
	std::vector<ScanCodes> switchedKeys = { SC_LALT, SC_LCTRL };
	int size = keys.size();

	for (int i = 0; i < size; i++) {
		EXPECT_TRUE(validateKeyMapsAndOutputThem("LCtrl or LAlt", {
			{
				KeyDown(keys[i]),
				KeyDown(SC_LSHIFT),
				KeyUp(SC_LSHIFT),
				KeyUp(keys[i])
			},
			{
				KeyDown(switchedKeys[i]),
				KeyDown(SC_LSHIFT),
				KeyUp(SC_LSHIFT),
				KeyUp(switchedKeys[i])
			}
		}));
	}
}

TEST_F(KeyEventTest, LCTRL_OR_LALT) {
	std::vector<ScanCodes> keys = { SC_LCTRL, SC_LALT };
	std::vector<ScanCodes> switchedKeys = { SC_LALT, SC_LCTRL };
	int size = keys.size();

	for (int i = 0; i < size; i++) {
		EXPECT_TRUE(validateKeyMapsAndOutputThem("LCtrl or LAlt", {
			{
				KeyDown(keys[i]),
				KeyUp(keys[i])
			},
			{
				KeyDown(switchedKeys[i]),
				KeyUp(switchedKeys[i])
			}
		}));

		EXPECT_TRUE(validateKeyMapsAndOutputThem("LCtrl or LAlt", {
			{
				KeyDown(keys[i])
			},
			{
				KeyDown(switchedKeys[i])
			}
		}));
	}
}

// NICE TO HAVE

TEST_F(KeyEventTest, handleLWinKey_LWIN_1234_SC2) {
	setActiveProcessName("SC2_x64.exe");

	EXPECT_TRUE(validateKeyMapsAndOutputThem("LWin", {
		{
			KeyDown(SC_LWIN),
			KeyDown(SC_1),
			KeyUp(SC_1),
			KeyUp(SC_LWIN)
		},
		{
			KeyDown(SC_LALT),
			Key(SC_1),
			KeyUp(SC_LALT)
		}
	}, "SC2_x64.exe"));
}

TEST_F(KeyEventTest, handleLWinKey_LWIN_BACK_gitbash) {
	setActiveProcessName("mintty.exe");

	EXPECT_TRUE(validateKeyMapsAndOutputThem("LWin", {
		{
			KeyDown(SC_LWIN),
			KeyDown(SC_BACK),
			KeyUp(SC_BACK),
			KeyUp(SC_LWIN)
		},
		{
			KeyDown(SC_LCTRL),
			Key(SC_W),
			KeyUp(SC_LCTRL)
		}
	}, "mintty.exe"));
}

TEST_F(KeyEventTest, handleLWinKey_LWIN_H) {
	EXPECT_TRUE(validateKeyMapsAndOutputThem("LWin", {
		{
			KeyDown(SC_LWIN),
			KeyDown(SC_H),
			KeyUp(SC_H),
			KeyUp(SC_LWIN)
		},
		{
			KeyDown(SC_LALT),
			Key(SC_LEFT),
			KeyUp(SC_LALT)
		}
	}));
}

TEST_F(KeyEventTest, handleLWinKey_LWIN_L) {
	EXPECT_TRUE(validateKeyMapsAndOutputThem("LWin", {
		{
			KeyDown(SC_LWIN),
			KeyDown(SC_L),
			KeyUp(SC_L),
			KeyUp(SC_LWIN)
		},
		{
			KeyDown(SC_LALT),
			Key(SC_RIGHT),
			KeyUp(SC_LALT)
		}
	}));
}

TEST_F(KeyEventTest, handleLWinKey_LWIN_BACK) {
	EXPECT_TRUE(validateKeyMapsAndOutputThem("LWin", {
		{
			KeyDown(SC_LWIN),
			KeyDown(SC_BACK),
			KeyUp(SC_BACK),
			KeyUp(SC_LWIN)
		},
		{
			KeyDown(SC_LCTRL),
			Key(SC_BACK),
			KeyUp(SC_LCTRL)
		}
	}));
}

TEST_F(KeyEventTest, handleLWinKey_LWIN_D) {
	EXPECT_TRUE(validateKeyMapsAndOutputThem("LWin", {
		{
			KeyDown(SC_LWIN),
			KeyDown(SC_D),
			KeyUp(SC_D),
			KeyUp(SC_LWIN)
		},
		{
			KeyDown(SC_LWIN),
			Key(SC_D),
			KeyUp(SC_LWIN)
		}
	}));
}

TEST_F(KeyEventTest, handleLWinKey_LWIN) {
	EXPECT_TRUE(validateKeyMapsAndOutputThem("LWin", {
		{
			KeyDown(SC_LWIN),
			KeyUp(SC_LWIN)
		},
		{}
	}));
}

// handleLAltKey

// TODO: MORE

TEST_F(KeyEventTest, handleLAltKey_LALT_ESC) {
	EXPECT_TRUE(validateKeyMapsAndOutputThem("LAlt", {
		{
			KeyDown(SC_LALT),
			KeyDown(SC_ESC),
			KeyUp(SC_ESC),
			KeyUp(SC_LALT)
		},
		{
			KeyDown(SC_LCTRL),
			KeyUp(SC_LCTRL)
		}
	}));
}

TEST_F(KeyEventTest, handleLAltKey_LALT_Q) {
	EXPECT_TRUE(validateKeyMapsAndOutputThem("LAlt", {
		{
			KeyDown(SC_LALT),
			KeyDown(SC_Q),
			KeyUp(SC_Q),
			KeyUp(SC_LALT)
		},
		{
			KeyDown(SC_LCTRL),
			KeyUp(SC_LCTRL),
			KeyDown(SC_LALT),
			Key(SC_F4),
			KeyUp(SC_LALT),
			KeyDown(SC_LCTRL),
			KeyUp(SC_LCTRL)
		}
	}));
}

TEST_F(KeyEventTest, handleLAltKey_LALT_BACK) {
	EXPECT_TRUE(validateKeyMapsAndOutputThem("LAlt", {
		{
			KeyDown(SC_LALT),
			KeyDown(SC_BACK),
			KeyUp(SC_BACK),
			KeyUp(SC_LALT)
		},
		{
			KeyDown(SC_LCTRL),
			KeyUp(SC_LCTRL),
			KeyDown(SC_LSHIFT),
			Key(SC_HOME),
			KeyUp(SC_LSHIFT),
			Key(SC_BACK),
			KeyDown(SC_LCTRL),
			KeyUp(SC_LCTRL)
		}
	}));
}

TEST_F(KeyEventTest, handleLAltKey_LALT_J_OR_K) {
	EXPECT_TRUE(validateKeyMapsAndOutputThem("LAlt", {
		{
			KeyDown(SC_LALT),
			KeyDown(SC_J),
			KeyUp(SC_J),
			KeyUp(SC_LALT)
		},
		{
			KeyDown(SC_LCTRL),
			KeyUp(SC_LCTRL),
			Key(SC_NEXT),
			KeyDown(SC_LCTRL),
			KeyUp(SC_LCTRL)
		}
	}));
}

TEST_F(KeyEventTest, handleLAltKey_LALT_Tab_Letter) {
	EXPECT_TRUE(validateKeyMapsAndOutputThem("LAlt", {
		{
			KeyDown(SC_LALT),
			KeyDown(SC_TAB),
			KeyUp(SC_TAB),
			KeyDown(SC_C),
			KeyUp(SC_C)
		},
		{
			KeyDown(SC_LCTRL),
			KeyUp(SC_LCTRL),
			KeyDown(SC_LALT),
			Key(SC_TAB)
		}
	}));
}

TEST_F(KeyEventTest, handleLAltKey_LALT_Tab) {
	EXPECT_TRUE(validateKeyMapsAndOutputThem("LAlt", {
		{
			KeyDown(SC_LALT),
			KeyDown(SC_TAB),
			KeyUp(SC_TAB)
		},
		{
			KeyDown(SC_LCTRL),
			KeyUp(SC_LCTRL),
			KeyDown(SC_LALT),
			Key(SC_TAB)
		}
	}));
}

TEST_F(KeyEventTest, handleLAltKey_LALTDown_LetterDown) {
	EXPECT_TRUE(validateKeyMapsAndOutputThem("LAlt", {
		{
			KeyDown(SC_LALT),
			KeyDown(SC_C)
		},
		{
			KeyDown(SC_LCTRL),
			KeyDown(SC_C)
		}
	}));
}

TEST_F(KeyEventTest, handleLAltKey_LALT_Letter) {
	EXPECT_TRUE(validateKeyMapsAndOutputThem("LAlt", {
		{
			KeyDown(SC_LALT),
			KeyDown(SC_C),
			KeyUp(SC_C),
			KeyUp(SC_LALT)
		},
		{
			KeyDown(SC_LCTRL),
			KeyDown(SC_C),
			KeyUp(SC_C),
			KeyUp(SC_LCTRL)
		}
	}));
}

TEST_F(KeyEventTest, handleLAltKey_LALTDown) {
	EXPECT_TRUE(validateKeyMapsAndOutputThem("LAlt", {
		{
			KeyDown(SC_LALT)
		},
		{
			KeyDown(SC_LCTRL)
		}
	}));
}

TEST_F(KeyEventTest, handleLAltKey_LALT) {
	EXPECT_TRUE(validateKeyMapsAndOutputThem("LAlt", {
		{
			KeyDown(SC_LALT),
			KeyUp(SC_LALT)
		},
		{
			KeyDown(SC_LCTRL),
			KeyUp(SC_LCTRL)
		}
	}));
}

// handleShiftKey

TEST_F(KeyEventTest, handleShiftKey_LSHIFT_Letter) {
	EXPECT_TRUE(validateKeyMapsAndOutputThem("Shift", {
		{
			KeyDown(SC_LSHIFT),
			KeyDown(SC_C),
			KeyUp(SC_C),
			KeyUp(SC_LSHIFT)
		},
		{
			KeyDown(SC_LSHIFT),
			KeyDown(SC_C),
			KeyUp(SC_C),
			KeyUp(SC_LSHIFT)
		}
	}));
}

TEST_F(KeyEventTest, handleShiftKey_LSHIFT) {
	EXPECT_TRUE(validateKeyMapsAndOutputThem("Shift", {
		{
			KeyDown(SC_LSHIFT),
			KeyUp(SC_LSHIFT)
		},
		{
			KeyDown(SC_LSHIFT),
		KeyUp(SC_LSHIFT)
		}
	}));
}

// handleKey

TEST_F(KeyEventTest, handleKey_chrome_F3) {
	setActiveProcessName("chrome.exe");

	EXPECT_TRUE(validateKeyMapsAndOutputThem("Key", {
		{
			KeyDown(SC_F3),
			KeyUp(SC_F3)
		},
		{
			KeyDown(SC_LCTRL),
			KeyDown(SC_LSHIFT),
			Key(SC_TAB),
			KeyUp(SC_LSHIFT),
			KeyUp(SC_LCTRL)
		}
	}, "chrome.exe"));
}

TEST_F(KeyEventTest, handleKey_chrome_F4) {
	setActiveProcessName("chrome.exe");

	EXPECT_TRUE(validateKeyMapsAndOutputThem("Key", {
		{
			KeyDown(SC_F4),
			KeyUp(SC_F4)
		},
		{
			KeyDown(SC_LCTRL),
			Key(SC_TAB),
			KeyUp(SC_LCTRL)
		}
	}, "chrome.exe"));
}

TEST_F(KeyEventTest, handleKey_chrome_F5) {
	setActiveProcessName("chrome.exe");

	EXPECT_TRUE(validateKeyMapsAndOutputThem("Key", {
		{
			KeyDown(SC_F5),
			KeyUp(SC_F5)
		},
		{
			KeyDown(SC_LALT),
			Key(SC_M),
			KeyUp(SC_LALT)
		}
	}, "chrome.exe"));
}

TEST_F(KeyEventTest, handleKey_chrome_F6) {
	setActiveProcessName("chrome.exe");

	EXPECT_TRUE(validateKeyMapsAndOutputThem("Key", {
		{
			KeyDown(SC_F6),
			KeyUp(SC_F6)
		},
		{
			KeyDown(SC_LALT),
			Key(SC_T),
			KeyUp(SC_LALT)
		}
	}, "chrome.exe"));
}

TEST_F(KeyEventTest, handleKey_F1) {
	EXPECT_TRUE(validateKeyMapsAndOutputThem("Key", {
		{
			KeyDown(SC_F1),
			KeyUp(SC_F1)
		},
		{
			Key(SC_BRIGHTNESSDOWN)
		}
	}));
}

TEST_F(KeyEventTest, handleKey_F2) {
	EXPECT_TRUE(validateKeyMapsAndOutputThem("Key", {
		{
			KeyDown(SC_F2),
			KeyUp(SC_F2)
		},
		{
			Key(SC_BRIGHTNESSUP)
		}
	}));
}

// F10↕ = MUTE↕
TEST_F(KeyEventTest, handleKey_F10) {
	EXPECT_TRUE(validateKeyMapsAndOutputThem("Key", {
		{
			KeyDown(SC_F10),
			KeyUp(SC_F10)
		},
		{
			Key(SC_MUTE)
		}
	}));
}

// F11↕ = VOLUMEDOWN↕
TEST_F(KeyEventTest, handleKey_F11) {
	EXPECT_TRUE(validateKeyMapsAndOutputThem("Key", {
		{
			KeyDown(SC_F11),
			KeyUp(SC_F11)
		},
		{
			Key(SC_VOLUMEDOWN),
			Key(SC_VOLUMEDOWN),
			Key(SC_VOLUMEDOWN),
			Key(SC_VOLUMEDOWN)
		}
	}));
}

// F12↕ = VOLUMEUP↕
TEST_F(KeyEventTest, handleKey_F12) {
	EXPECT_TRUE(validateKeyMapsAndOutputThem("Key", {
		{
			KeyDown(SC_F12),
			KeyUp(SC_F12)
		},
		{
			Key(SC_VOLUMEUP),
			Key(SC_VOLUMEUP),
			Key(SC_VOLUMEUP),
			Key(SC_VOLUMEUP)
		}
	}));
}

// Letter↕
TEST_F(KeyEventTest, handleKey_Letter) {
	EXPECT_TRUE(validateKeyMapsAndOutputThem("Key", {
		{
			KeyDown(SC_C),
			KeyUp(SC_C)
		},
		{
			KeyDown(SC_C),
			KeyUp(SC_C)
		}
	}));
}
