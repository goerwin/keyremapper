#ifndef UNICODE
#define UNICODE
#endif

#include "pch.h"
#include <vector>
#include <windows.h>
#include "../InterceptionKeyRemapper/interception.h"
#include "../InterceptionKeyRemapper/KeyEvent.cpp"

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
			if (keyEvents[i].keyCode != SC_NULL) {
				newKeyEvents.insert(newKeyEvents.end(), keyEvents[i]);
			}
		}

		return newKeyEvents;
	}

	static bool compareKeyEvents(std::vector<Key> keyEvents, std::vector<Key> keyEvents2) {
		keyEvents = removeNullKeyEvents(keyEvents);
		keyEvents2 = removeNullKeyEvents(keyEvents2);

		int size1 = keyEvents.size();
		int size2 = keyEvents2.size();

		if (size1 != size2) {
			return false;
		}

		try {
			for (int i = 0; i < size2; i++) {
				if (
					keyEvents[i].keyCode != keyEvents2[i].keyCode ||
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
};

// handleLAltKey

// LALT↓TAB↕Letter↕ = LCTRL↕LALT↓TAB↕
TEST_F(KeyEventTest, handleLAltKey_LALT_Letter_AltTabbed) {
	auto keyEvents = getKeyEvents({
		InterceptionKeyStroke({ SC_LALT, 0 }),
		InterceptionKeyStroke({ SC_TAB, 0 }),
		InterceptionKeyStroke({ SC_TAB, 1 }),
		InterceptionKeyStroke({ SC_C, 0 }),
		InterceptionKeyStroke({ SC_C, 1 })
		});

	std::vector<Key> expectedKeyEvents = {
		KeyDown(SC_LCTRL),
		KeyUp(SC_LCTRL),
		KeyDown(SC_LALT),
		KeyDown(SC_TAB),
		KeyUp(SC_TAB),
		KeyUp(SC_TAB)
	};

	EXPECT_TRUE(compareKeyEvents(keyEvents, expectedKeyEvents));
}

// LALT↓Letter↓ = LCTRL↓Letter↕
TEST_F(KeyEventTest, handleLAltKey_LALTDown_LetterDown) {
	auto keyEvents = getKeyEvents({
		InterceptionKeyStroke({ SC_LALT, 0 }),
		InterceptionKeyStroke({ SC_C, 0 })
		});

	std::vector<Key> expectedKeyEvents = {
		KeyDown(SC_LCTRL),
		KeyDown(SC_C),
		KeyUp(SC_C)
	};

	EXPECT_TRUE(compareKeyEvents(keyEvents, expectedKeyEvents));
}

// LALT↓Letter↕LALT↑ = LCTRL↓Letter↕LCTRL↑
TEST_F(KeyEventTest, handleLAltKey_LALT_Letter) {
	auto keyEvents = getKeyEvents({
		InterceptionKeyStroke({ SC_LALT, 0 }),
		InterceptionKeyStroke({ SC_C, 0 }),
		InterceptionKeyStroke({ SC_C, 1 }),
		InterceptionKeyStroke({ SC_LALT, 1 })
		});

	std::vector<Key> expectedKeyEvents = {
		KeyDown(SC_LCTRL),
		KeyDown(SC_C),
		KeyUp(SC_C),
		KeyUp(SC_LCTRL)
	};

	EXPECT_TRUE(compareKeyEvents(keyEvents, expectedKeyEvents));
}

// LALT↓ = LCTRL↓
TEST_F(KeyEventTest, handleLAltKey_LALTDown) {
		auto keyEvents = getKeyEvents({
			InterceptionKeyStroke({ SC_LALT, 0 })
		});

	std::vector<Key> expectedKeyEvents = {
		KeyDown(SC_LCTRL)
	};

	EXPECT_TRUE(compareKeyEvents(keyEvents, expectedKeyEvents));
}

// LALT↕ = LCTRL↕
TEST_F(KeyEventTest, handleLAltKey_LALT) {
	auto keyEvents = getKeyEvents({
		InterceptionKeyStroke({ SC_LALT, 0 }),
		InterceptionKeyStroke({ SC_LALT, 1 })
		});

	std::vector<Key> expectedKeyEvents = {
		KeyDown(SC_LCTRL),
		KeyUp(SC_LCTRL)
	};

	EXPECT_TRUE(compareKeyEvents(keyEvents, expectedKeyEvents));
}

// handleShiftKey

// LSHIFT↓Letter↕LSHIFT↑
TEST_F(KeyEventTest, handleShiftKey_LSHIFT_Letter) {
	auto keyEvents = getKeyEvents({
		InterceptionKeyStroke({ SC_LSHIFT, 0 }),
		InterceptionKeyStroke({ SC_C, 0 }),
		InterceptionKeyStroke({ SC_C, 1 }),
		InterceptionKeyStroke({ SC_LSHIFT, 1 })
		});

	std::vector<Key> expectedKeyEvents = {
		KeyDown(SC_LSHIFT),
		KeyDown(SC_C),
		KeyUp(SC_C),
		KeyUp(SC_LSHIFT)
	};

	EXPECT_TRUE(compareKeyEvents(keyEvents, expectedKeyEvents));
}

// LSHIFT↕
TEST_F(KeyEventTest, handleShiftKey_LSHIFT) {
	auto keyEvents = getKeyEvents({
		InterceptionKeyStroke({ SC_LSHIFT, 0 }),
		InterceptionKeyStroke({ SC_LSHIFT, 1 })
		});

	std::vector<Key> expectedKeyEvents = {
		KeyDown(SC_LSHIFT),
		KeyUp(SC_LSHIFT)
	};

	EXPECT_TRUE(compareKeyEvents(keyEvents, expectedKeyEvents));
}

// handleKey

// F3↕ = LCTRL↓LSHIFT↓TAB↕LSHIFT↑LCTRL↑
TEST_F(KeyEventTest, handleKey_chrome_F3) {
	setActiveProcessName("chrome.exe");

	auto keyEvents = getKeyEvents({
		InterceptionKeyStroke({ SC_F3, 0 }),
		InterceptionKeyStroke({ SC_F3, 1 })
		});

	std::vector<Key> expectedKeyEvents = {
		KeyDown(SC_LCTRL),
		KeyDown(SC_LSHIFT),
		KeyDown(SC_TAB),
		KeyUp(SC_TAB),
		KeyUp(SC_LSHIFT),
		KeyUp(SC_LCTRL)
	};

	EXPECT_TRUE(compareKeyEvents(keyEvents, expectedKeyEvents));
}

// F4↕ = LCTRL↓TAB↕LCTRL↑
TEST_F(KeyEventTest, handleKey_chrome_F4) {
	setActiveProcessName("chrome.exe");

	auto keyEvents = getKeyEvents({
		InterceptionKeyStroke({ SC_F4, 0 }),
		InterceptionKeyStroke({ SC_F4, 1 })
		});

	std::vector<Key> expectedKeyEvents = {
		KeyDown(SC_LCTRL),
		KeyDown(SC_TAB),
		KeyUp(SC_TAB),
		KeyUp(SC_LCTRL)
	};

	EXPECT_TRUE(compareKeyEvents(keyEvents, expectedKeyEvents));
}

// F5↕ = LALT↓M↕LALT↑
TEST_F(KeyEventTest, handleKey_chrome_F5) {
	setActiveProcessName("chrome.exe");

	auto keyEvents = getKeyEvents({
		InterceptionKeyStroke({ SC_F5, 0 }),
		InterceptionKeyStroke({ SC_F5, 1 })
		});

	std::vector<Key> expectedKeyEvents = {
		KeyDown(SC_LALT),
		KeyDown(SC_M),
		KeyUp(SC_M),
		KeyUp(SC_LALT)
	};

	EXPECT_TRUE(compareKeyEvents(keyEvents, expectedKeyEvents));
}

// F6↕ = LALT↓T↕LALT↑
TEST_F(KeyEventTest, handleKey_chrome_F6) {
	setActiveProcessName("chrome.exe");

	auto keyEvents = getKeyEvents({
		InterceptionKeyStroke({ SC_F6, 0 }),
		InterceptionKeyStroke({ SC_F6, 1 })
		});

	std::vector<Key> expectedKeyEvents = {
		KeyDown(SC_LALT),
		KeyDown(SC_T),
		KeyUp(SC_T),
		KeyUp(SC_LALT)
	};

	EXPECT_TRUE(compareKeyEvents(keyEvents, expectedKeyEvents));
}

// F1↕ = BRIGHTNESSDOWN↕
TEST_F(KeyEventTest, handleKey_F1) {
	auto keyEvents = getKeyEvents({
		InterceptionKeyStroke({ SC_F1, 0 }),
		InterceptionKeyStroke({ SC_F1, 1 })
	});

	std::vector<Key> expectedKeyEvents = {
		KeyDown(SC_BRIGHTNESSDOWN),
		g_nullKeyEvent
	};

	EXPECT_TRUE(compareKeyEvents(keyEvents, expectedKeyEvents));
}

// F2↕ = BRIGHTNESSUP↕
TEST_F(KeyEventTest, handleKey_F2) {
	auto keyEvents = getKeyEvents({
		InterceptionKeyStroke({ SC_F2, 0 }),
		InterceptionKeyStroke({ SC_F2, 1 })
	});

	std::vector<Key> expectedKeyEvents = {
		KeyDown(SC_BRIGHTNESSUP),
		g_nullKeyEvent
	};

	EXPECT_TRUE(compareKeyEvents(keyEvents, expectedKeyEvents));
}

// F10↕ = MUTE↕
TEST_F(KeyEventTest, handleKey_F10) {
	auto keyEvents = getKeyEvents({
		InterceptionKeyStroke({ SC_F10, 0 }),
		InterceptionKeyStroke({ SC_F10, 1 })
	});

	std::vector<Key> expectedKeyEvents = {
		KeyDown(SC_MUTE, 2),
		KeyUp(SC_MUTE, 3)
	};

	EXPECT_TRUE(compareKeyEvents(keyEvents, expectedKeyEvents));
}

// F11↕ = VOLUMEDOWN↕
TEST_F(KeyEventTest, handleKey_F11) {
	auto keyEvents = getKeyEvents({
		InterceptionKeyStroke({ SC_F11, 0 }),
		InterceptionKeyStroke({ SC_F11, 1 })
	});

	std::vector<Key> expectedKeyEvents = {
		KeyDown(SC_VOLUMEDOWN, 2), KeyUp(SC_VOLUMEDOWN, 3),
		KeyDown(SC_VOLUMEDOWN, 2), KeyUp(SC_VOLUMEDOWN, 3),
		KeyDown(SC_VOLUMEDOWN, 2), KeyUp(SC_VOLUMEDOWN, 3),
		KeyDown(SC_VOLUMEDOWN, 2), KeyUp(SC_VOLUMEDOWN, 3)
	};

	EXPECT_TRUE(compareKeyEvents(keyEvents, expectedKeyEvents));
}

// F12↕ = VOLUMEUP↕
TEST_F(KeyEventTest, handleKey_F12) {
	auto keyEvents = getKeyEvents({
		InterceptionKeyStroke({ SC_F12, 0 }),
		InterceptionKeyStroke({ SC_F12, 1 })
		});

	std::vector<Key> expectedKeyEvents = {
		KeyDown(SC_VOLUMEUP, 2), KeyUp(SC_VOLUMEUP, 3),
		KeyDown(SC_VOLUMEUP, 2), KeyUp(SC_VOLUMEUP, 3),
		KeyDown(SC_VOLUMEUP, 2), KeyUp(SC_VOLUMEUP, 3),
		KeyDown(SC_VOLUMEUP, 2), KeyUp(SC_VOLUMEUP, 3)
	};

	EXPECT_TRUE(compareKeyEvents(keyEvents, expectedKeyEvents));
}

// Letter↕
TEST_F(KeyEventTest, handleKey_Letter) {
	auto keyEvents = getKeyEvents({
		InterceptionKeyStroke({ SC_C, 0 }),
		InterceptionKeyStroke({ SC_C, 1 })
	});

	EXPECT_TRUE(compareKeyEvents(
		keyEvents,
		{ KeyDown(SC_C), KeyUp(SC_C) }
	));
}
