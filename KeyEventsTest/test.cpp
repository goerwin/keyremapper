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
	void SetUp() override {}

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
		String program = ""
	) {
		setGlobalDefaultValues();
		setActiveProcessName(program);

		auto inputKeys = keys.first;
		auto convertedInputKeys = removeNullKeyEvents(getKeyEvents(inputKeys));
		auto expectedKeys = removeNullKeyEvents(keys.second);
		auto result = compareKeyEvents(convertedInputKeys, expectedKeys);

		if (result) {
			String symbolMappedKeys = getKeySymbols(inputKeys);

			symbolMappedKeys = symbolMappedKeys
				.append(" = ")
				.append(getKeySymbols(convertedInputKeys))
				.append(program != "" ? String(" // ").append(program) : "")
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

TEST_F(KeyEventTest, MODIFIER) {
	String testName = "1 Modifier - ";
	// tuple<title, key1, expectedKey1, expectedGeneral>
	std::vector<std::tuple<String, ScanCodes, ScanCodes, ScanCodes>> modifiers = {
		{ "Caps", SC_CAPSLOCK, SC_NULL, SC_NULL },
		{ "Ctrl", SC_LCTRL, SC_LALT, SC_GENERAL },
		{ "Win", SC_LWIN, SC_NULL, SC_NULL },
		{ "Alt", SC_LALT, SC_LCTRL, SC_GENERAL },
		{ "Shift", SC_LSHIFT, SC_LSHIFT, SC_GENERAL }
	};

	for (int i = 0; i < modifiers.size(); i++) {
		EXPECT_TRUE(validateKeyMapsAndOutputThem(testName + std::get<0>(modifiers[i]), {
			{
				KeyDown(std::get<1>(modifiers[i])),
				KeyUp(std::get<1>(modifiers[i]))
			},
			{
				KeyDown(std::get<2>(modifiers[i])),
				KeyUp(std::get<2>(modifiers[i]))
			}
		}));

		EXPECT_TRUE(validateKeyMapsAndOutputThem(testName + std::get<0>(modifiers[i]) + " + _", {
			{
				KeyDown(std::get<1>(modifiers[i])),
				KeyDown(SC_GENERAL),
				KeyUp(SC_GENERAL),
				KeyUp(std::get<1>(modifiers[i]))
			},
			{
				KeyDown(std::get<2>(modifiers[i])),
				KeyDown(std::get<3>(modifiers[i])),
				KeyUp(std::get<3>(modifiers[i])),
				KeyUp(std::get<2>(modifiers[i]))
			}
		}));
	}
}

TEST_F(KeyEventTest, TWO_MODIFIERS) {
	String testName = "2 Modifiers - ";

	// tuple<title, key1, key2, expectedKey1, expectedK2, expectedGeneral>
	std::vector<std::tuple<String, ScanCodes, ScanCodes, ScanCodes, ScanCodes, ScanCodes>> modifiers = {
		{ "Caps + Shift", SC_CAPSLOCK, SC_LSHIFT, SC_NULL, SC_LSHIFT, SC_NULL },
		{ "Caps + Alt", SC_CAPSLOCK, SC_LALT, SC_NULL, SC_LCTRL, SC_NULL },
		{ "Ctrl + Shift", SC_LCTRL, SC_LSHIFT, SC_LALT, SC_LSHIFT, SC_GENERAL },
		// TODO: { "Ctrl + Alt", SC_LCTRL, SC_LALT, SC_LALT, SC_LCTRL },
		{ "Win + Shift", SC_LWIN, SC_LSHIFT, SC_NULL, SC_LSHIFT, SC_NULL },
		{ "Win + Alt", SC_LWIN, SC_LALT, SC_NULL, SC_LCTRL, SC_NULL },
	};

	for (int i = 0; i < modifiers.size(); i++) {
		EXPECT_TRUE(validateKeyMapsAndOutputThem(testName + std::get<0>(modifiers[i]), {
			{
				KeyDown(std::get<1>(modifiers[i])),
				KeyDown(std::get<2>(modifiers[i])),
				KeyUp(std::get<2>(modifiers[i])),
				KeyUp(std::get<1>(modifiers[i]))
			},
			{
				KeyDown(std::get<3>(modifiers[i])),
				KeyDown(std::get<4>(modifiers[i])),
				KeyUp(std::get<4>(modifiers[i])),
				KeyUp(std::get<3>(modifiers[i]))
			}
		}));

		EXPECT_TRUE(validateKeyMapsAndOutputThem(testName + std::get<0>(modifiers[i]) + " + _", {
			{
				KeyDown(std::get<1>(modifiers[i])),
				KeyDown(std::get<2>(modifiers[i])),
				KeyDown(SC_GENERAL),
				KeyUp(SC_GENERAL),
				KeyUp(std::get<2>(modifiers[i])),
				KeyUp(std::get<1>(modifiers[i]))
			},
			{
				KeyDown(std::get<3>(modifiers[i])),
				KeyDown(std::get<4>(modifiers[i])),
				KeyDown(std::get<5>(modifiers[i])),
				KeyUp(std::get<5>(modifiers[i])),
				KeyUp(std::get<4>(modifiers[i])),
				KeyUp(std::get<3>(modifiers[i]))
			}
		}));
	}
}

TEST_F(KeyEventTest, KEY) {
	String testName = "Key - ";

	EXPECT_TRUE(validateKeyMapsAndOutputThem(testName + "DownUp", {
		{ KeyDown(SC_H), KeyUp(SC_H) },
		{ KeyDown(SC_H), KeyUp(SC_H) }
	}));

	EXPECT_TRUE(validateKeyMapsAndOutputThem(testName + "Down", {
		{ KeyDown(SC_H), KeyDown(SC_H), KeyDown(SC_H) },
		{ KeyDown(SC_H), KeyDown(SC_H), KeyDown(SC_H) }
	}));

	EXPECT_TRUE(validateKeyMapsAndOutputThem(testName + "Down then Up", {
		{ KeyDown(SC_H), KeyDown(SC_H), KeyUp(SC_H) },
		{ KeyDown(SC_H), KeyDown(SC_H), KeyUp(SC_H) }
	}));

	EXPECT_TRUE(validateKeyMapsAndOutputThem(testName + "2 Keys In Order", {
		{ KeyDown(SC_H), KeyDown(SC_J), KeyUp(SC_J), KeyUp(SC_H) },
		{ KeyDown(SC_H), KeyDown(SC_J), KeyUp(SC_J), KeyUp(SC_H) }
	}));

	EXPECT_TRUE(validateKeyMapsAndOutputThem(testName + "2 Keys No Order", {
		{ KeyDown(SC_H), KeyDown(SC_J), KeyUp(SC_H), KeyUp(SC_J) },
		{ KeyDown(SC_H), KeyDown(SC_J), KeyUp(SC_H), KeyUp(SC_J) }
	}));

	EXPECT_TRUE(validateKeyMapsAndOutputThem(testName + "3 Keys In Order", {
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

	EXPECT_TRUE(validateKeyMapsAndOutputThem(testName + "3 Keys No Order", {
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

// Ctrl

TEST_F(KeyEventTest, CTRL_TAB) {
	EXPECT_TRUE(validateKeyMapsAndOutputThem("Ctrl", {
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

TEST_F(KeyEventTest, CTRL_SHIFT_TAB) {
	EXPECT_TRUE(validateKeyMapsAndOutputThem("Ctrl", {
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

// Alt

TEST_F(KeyEventTest, ALT_ESC) {
	EXPECT_TRUE(validateKeyMapsAndOutputThem("Alt", {
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

TEST_F(KeyEventTest, ALT_Q) {
	EXPECT_TRUE(validateKeyMapsAndOutputThem("Alt", {
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

TEST_F(KeyEventTest, ALT_BACK) {
	EXPECT_TRUE(validateKeyMapsAndOutputThem("Alt", {
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

TEST_F(KeyEventTest, ALT_J_OR_K) {
	EXPECT_TRUE(validateKeyMapsAndOutputThem("Alt", {
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

TEST_F(KeyEventTest, ALT_TAB) {
	EXPECT_TRUE(validateKeyMapsAndOutputThem("Alt", {
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

TEST_F(KeyEventTest, ALT_SHIFT_TAB) {
	EXPECT_TRUE(validateKeyMapsAndOutputThem("Alt", {
		{
			KeyDown(SC_LALT),
			KeyDown(SC_LSHIFT),
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
			KeyUp(SC_LSHIFT),
			KeyUp(SC_LALT)
		}
	}));
}

TEST_F(KeyEventTest, ALT_TAB_Letter) {
	EXPECT_TRUE(validateKeyMapsAndOutputThem("Alt", {
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

TEST_F(KeyEventTest, ALT_TAB_THEN_ESC) {
	EXPECT_TRUE(validateKeyMapsAndOutputThem("Alt", {
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

TEST_F(KeyEventTest, ALT_TAB_THEN_Q) {
	EXPECT_TRUE(validateKeyMapsAndOutputThem("Alt", {
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

TEST_F(KeyEventTest, ALT_GRAVE) {
	EXPECT_TRUE(validateKeyMapsAndOutputThem("Alt", {
		{
			KeyDown(SC_LALT),
			KeyDown(SC_GRAVE),
			KeyUp(SC_GRAVE),
			KeyUp(SC_LALT)
		},
		{
			KeyDown(SC_LCTRL),
			KeyUp(SC_LCTRL),
			KeyDown(SC_LALT),
			Key(SC_GRAVE),
			KeyUp(SC_LALT),
			KeyDown(SC_LCTRL),
			KeyUp(SC_LCTRL)
		}
	}));
}

TEST_F(KeyEventTest, ALT_GRAVE_THEN_TAB) {
	EXPECT_TRUE(validateKeyMapsAndOutputThem("Alt", {
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

// CUSTOM - NOT SO IMPORTANT

TEST_F(KeyEventTest, CUSTOM_F3) {
	EXPECT_TRUE(validateKeyMapsAndOutputThem("Custom", {
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

TEST_F(KeyEventTest, CUSTOM_F4) {
	EXPECT_TRUE(validateKeyMapsAndOutputThem("Custom", {
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

TEST_F(KeyEventTest, CUSTOM_F5) {
	EXPECT_TRUE(validateKeyMapsAndOutputThem("Custom", {
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

TEST_F(KeyEventTest, CUSTOM_F6) {
	EXPECT_TRUE(validateKeyMapsAndOutputThem("Custom", {
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

TEST_F(KeyEventTest, CUSTOM_F1) {
	EXPECT_TRUE(validateKeyMapsAndOutputThem("Custom", {
		{
			KeyDown(SC_F1),
			KeyUp(SC_F1)
		},
		{
			Key(SC_BRIGHTNESSDOWN)
		}
	}));
}

TEST_F(KeyEventTest, CUSTOM_F2) {
	EXPECT_TRUE(validateKeyMapsAndOutputThem("Custom", {
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
TEST_F(KeyEventTest, CUSTOM_F10) {
	EXPECT_TRUE(validateKeyMapsAndOutputThem("Custom", {
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
TEST_F(KeyEventTest, CUSTOM_F11) {
	EXPECT_TRUE(validateKeyMapsAndOutputThem("Custom", {
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
TEST_F(KeyEventTest, CUSTOM_F12) {
	EXPECT_TRUE(validateKeyMapsAndOutputThem("Custom", {
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
