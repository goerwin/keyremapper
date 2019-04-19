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
			if (keyEvents[i].code != SC_NULL) {
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

	static bool validateKeyMapsAndOutputThem(
		std::string name,
		std::pair<std::vector<Key>, std::vector<Key>> keys
	) {
		return compareKeyEvents(getKeyEvents(keys.first), keys.second);
	}
};

// handleLWinKey

TEST_F(KeyEventTest, handleWinKey_LWIN_1234) {
	setActiveProcessName("SC2_x64.exe");

	EXPECT_TRUE(validateKeyMapsAndOutputThem("handleLWinKey.md", {
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
	}));
}

TEST_F(KeyEventTest, handleWinKey_LWIN_BACK_gitbash) {
	setActiveProcessName("mintty.exe");

	EXPECT_TRUE(validateKeyMapsAndOutputThem("handleLWinKey.md", {
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
	}));
}

TEST_F(KeyEventTest, handleWinKey_LWIN_H) {
	EXPECT_TRUE(validateKeyMapsAndOutputThem("handleLWinKey.md", {
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

TEST_F(KeyEventTest, handleWinKey_LWIN_L) {
	EXPECT_TRUE(validateKeyMapsAndOutputThem("handleLWinKey.md", {
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

TEST_F(KeyEventTest, handleWinKey_LWIN_BACK) {
	EXPECT_TRUE(validateKeyMapsAndOutputThem("handleLWinKey.md", {
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

TEST_F(KeyEventTest, handleWinKey_LWIN_D) {
	EXPECT_TRUE(validateKeyMapsAndOutputThem("handleLWinKey.md", {
		{
			KeyDown(SC_LWIN),
			KeyDown(SC_D),
			KeyUp(SC_D),
			KeyUp(SC_LWIN)
		},
		{
			KeyDown(SC_LWIN, 2),
			Key(SC_D),
			KeyUp(SC_LWIN, 3)
		}
	}));
}

TEST_F(KeyEventTest, handleWinKey_LWIN) {
	EXPECT_TRUE(validateKeyMapsAndOutputThem("handleLWinKey.md", {
		{
			KeyDown(SC_LWIN),
			KeyUp(SC_LWIN)
		},
		{}
	}));
}

// handleLAltKey

// LALT↓TAB↕Q↕LALT↑ = LCTRL↕LALT↓TAB↕SUPR↕LALT↑
TEST_F(KeyEventTest, handleLAltKey_LALT_TAB_Q) {
	EXPECT_TRUE(validateKeyMapsAndOutputThem("handleLAltKey.md", {
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

// LALT↓Q↕LALT↑ = LCTRL↕LALT↓F4↕LALT↑
TEST_F(KeyEventTest, handleLAltKey_LALT_Q) {
	EXPECT_TRUE(validateKeyMapsAndOutputThem("handleLAltKey.md", {
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
			KeyDown(SC_F4),
			KeyUp(SC_F4),
			KeyUp(SC_LALT)
		}
	}));
}

// LALT↓BACK↕LALT↑ = LCTRL↕LSHIFT↓HOME↕LSHIFT↑BACK↕LCTRL↑
TEST_F(KeyEventTest, handleLAltKey_LALT_BACK) {
	EXPECT_TRUE(validateKeyMapsAndOutputThem("handleLAltKey.md", {
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

// LALT↓LSHIFT↓J↕LSHIFT↑LALT↑ = LCTRL↕LSHIFT↓PgDown↕LSHIFT↑LCTRL↑
TEST_F(KeyEventTest, handleLAltKey_LALT_LSHIFT_J) {
	EXPECT_TRUE(validateKeyMapsAndOutputThem("handleLAltKey.md", {
		{
			KeyDown(SC_LALT),
			KeyDown(SC_LSHIFT),
			KeyDown(SC_J),
			KeyUp(SC_J),
			KeyUp(SC_LSHIFT),
			KeyUp(SC_LALT)
		},
		{
			KeyDown(SC_LCTRL),
			KeyDown(SC_LSHIFT),
			KeyUp(SC_LCTRL),
			Key(SC_NEXT),
			KeyDown(SC_LCTRL),
			KeyUp(SC_LSHIFT),
			KeyUp(SC_LCTRL)
		}
	}));
}

// LALT↓LSHIFT↓K↕LSHIFT↑LALT↑ = LCTRL↕LSHIFT↓PgUp↕LSHIFT↑LCTRL↑
TEST_F(KeyEventTest, handleLAltKey_LALT_LSHIFT_K) {
	EXPECT_TRUE(validateKeyMapsAndOutputThem("handleLAltKey.md", {
		{
			KeyDown(SC_LALT),
			KeyDown(SC_LSHIFT),
			KeyDown(SC_K),
			KeyUp(SC_K),
			KeyUp(SC_LSHIFT),
			KeyUp(SC_LALT)
		},
		{
			KeyDown(SC_LCTRL),
			KeyDown(SC_LSHIFT),
			KeyUp(SC_LCTRL),
			Key(SC_PRIOR),
			KeyDown(SC_LCTRL),
			KeyUp(SC_LSHIFT),
			KeyUp(SC_LCTRL)
		}
	}));
}

// LALT↓J↕LALT↑ = LCTRL↕PgDown↕LCTRL↑
TEST_F(KeyEventTest, handleLAltKey_LALT_J) {
	EXPECT_TRUE(validateKeyMapsAndOutputThem("handleLAltKey.md", {
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

// LALT↓K↕LALT↑ = LCTRL↕PgUp↕LCTRL↑
TEST_F(KeyEventTest, handleLAltKey_LALT_K) {
	EXPECT_TRUE(validateKeyMapsAndOutputThem("handleLAltKey.md", {
		{
			KeyDown(SC_LALT),
			KeyDown(SC_K),
			KeyUp(SC_K),
			KeyUp(SC_LALT)
		},
		{
			KeyDown(SC_LCTRL),
			KeyUp(SC_LCTRL),
			Key(SC_PRIOR),
			KeyDown(SC_LCTRL),
			KeyUp(SC_LCTRL)
		}
	}));
}

// LALT↓Space↕LALT↑ = LCTRL↓F12↕LCTRL↑
TEST_F(KeyEventTest, handleLAltKey_LALT_Space) {
	EXPECT_TRUE(validateKeyMapsAndOutputThem("handleLAltKey.md", {
		{
			KeyDown(SC_LALT),
			KeyDown(SC_SPACE),
			KeyUp(SC_SPACE),
			KeyUp(SC_LALT)
		},
		{
			KeyDown(SC_LCTRL),
			Key(SC_F12),
			KeyUp(SC_LCTRL)
		}
	}));
}

// LALT↓F{n}↕LALT↑ = LCTRL↕F{n}↕LCTRL↑
TEST_F(KeyEventTest, handleLAltKey_LALT_FnKey) {
	EXPECT_TRUE(validateKeyMapsAndOutputThem("handleLAltKey.md", {
		{
			KeyDown(SC_LALT),
			KeyDown(SC_F3),
			KeyUp(SC_F3),
			KeyUp(SC_LALT)
		},
		{
			KeyDown(SC_LCTRL),
			KeyUp(SC_LCTRL),
			Key(SC_F3),
			KeyUp(SC_LCTRL)
		}
	}));
}

// LALT↓TAB↕Letter↕ = LCTRL↕LALT↓TAB↕
TEST_F(KeyEventTest, handleLAltKey_LALT_Tab_Letter) {
	EXPECT_TRUE(validateKeyMapsAndOutputThem("handleLAltKey.md", {
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

// LALT↓TAB↕ = LCTRL↕LALT↓TAB↕
TEST_F(KeyEventTest, handleLAltKey_LALT_Tab) {
	EXPECT_TRUE(validateKeyMapsAndOutputThem("handleLAltKey.md", {
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

// LALT↓Letter↓ = LCTRL↓Letter↕
TEST_F(KeyEventTest, handleLAltKey_LALTDown_LetterDown) {
	EXPECT_TRUE(validateKeyMapsAndOutputThem("handleLAltKey.md", {
		{
			KeyDown(SC_LALT),
			KeyDown(SC_C)
		},
		{
			KeyDown(SC_LCTRL),
			Key(SC_C)
		}
	}));
}

// LALT↓Letter↕LALT↑ = LCTRL↓Letter↕LCTRL↑
TEST_F(KeyEventTest, handleLAltKey_LALT_Letter) {
	EXPECT_TRUE(validateKeyMapsAndOutputThem("handleLAltKey.md", {
		{
			KeyDown(SC_LALT),
			KeyDown(SC_C),
			KeyUp(SC_C),
			KeyUp(SC_LALT)
		},
		{
			KeyDown(SC_LCTRL),
			Key(SC_C),
			KeyUp(SC_LCTRL)
		}
	}));
}

// LALT↓ = LCTRL↓
TEST_F(KeyEventTest, handleLAltKey_LALTDown) {
	EXPECT_TRUE(validateKeyMapsAndOutputThem("handleLAltKey.md", {
		{
			KeyDown(SC_LALT)
		},
		{
			KeyDown(SC_LCTRL)
		}
	}));
}

// LALT↕ = LCTRL↕
TEST_F(KeyEventTest, handleLAltKey_LALT) {
	EXPECT_TRUE(validateKeyMapsAndOutputThem("handleLAltKey.md", {
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
	EXPECT_TRUE(validateKeyMapsAndOutputThem("handleShiftKey.md", {
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
	EXPECT_TRUE(validateKeyMapsAndOutputThem("handleShiftKey.md", {
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

// F3↕ = LCTRL↓LSHIFT↓TAB↕LSHIFT↑LCTRL↑
TEST_F(KeyEventTest, handleKey_chrome_F3) {
	setActiveProcessName("chrome.exe");

	EXPECT_TRUE(validateKeyMapsAndOutputThem("handleShiftKey.md", {
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
	}));
}

// F4↕ = LCTRL↓TAB↕LCTRL↑
TEST_F(KeyEventTest, handleKey_chrome_F4) {
	setActiveProcessName("chrome.exe");

	EXPECT_TRUE(validateKeyMapsAndOutputThem("handleShiftKey.md", {
		{
			KeyDown(SC_F4),
			KeyUp(SC_F4)
		},
		{
			KeyDown(SC_LCTRL),
			Key(SC_TAB),
			KeyUp(SC_LCTRL)
		}
	}));
}

// F5↕ = LALT↓M↕LALT↑
TEST_F(KeyEventTest, handleKey_chrome_F5) {
	setActiveProcessName("chrome.exe");

	EXPECT_TRUE(validateKeyMapsAndOutputThem("handleShiftKey.md", {
		{
			KeyDown(SC_F5),
			KeyUp(SC_F5)
		},
		{
			KeyDown(SC_LALT),
			Key(SC_M),
			KeyUp(SC_LALT)
		}
	}));
}

// F6↕ = LALT↓T↕LALT↑
TEST_F(KeyEventTest, handleKey_chrome_F6) {
	setActiveProcessName("chrome.exe");

	EXPECT_TRUE(validateKeyMapsAndOutputThem("handleShiftKey.md", {
		{
			KeyDown(SC_F6),
			KeyUp(SC_F6)
		},
		{
			KeyDown(SC_LALT),
			Key(SC_T),
			KeyUp(SC_LALT)
		}
	}));
}

// F1↕ = BRIGHTNESSDOWN↕
TEST_F(KeyEventTest, handleKey_F1) {
	EXPECT_TRUE(validateKeyMapsAndOutputThem("handleShiftKey.md", {
		{
			KeyDown(SC_F1),
			KeyUp(SC_F1)
		},
		{
			KeyDown(SC_BRIGHTNESSDOWN)
		}
	}));
}

// F2↕ = BRIGHTNESSUP↕
TEST_F(KeyEventTest, handleKey_F2) {
	EXPECT_TRUE(validateKeyMapsAndOutputThem("handleShiftKey.md", {
		{
			KeyDown(SC_F2),
			KeyUp(SC_F2)
		},
		{
			KeyDown(SC_BRIGHTNESSUP)
		}
	}));
}

// F10↕ = MUTE↕
TEST_F(KeyEventTest, handleKey_F10) {
	EXPECT_TRUE(validateKeyMapsAndOutputThem("handleShiftKey.md", {
		{
			KeyDown(SC_F10),
			KeyUp(SC_F10)
		},
		{
			Key(SC_MUTE, 5)
		}
	}));
}

// F11↕ = VOLUMEDOWN↕
TEST_F(KeyEventTest, handleKey_F11) {
	EXPECT_TRUE(validateKeyMapsAndOutputThem("handleShiftKey.md", {
		{
			KeyDown(SC_F11),
			KeyUp(SC_F11)
		},
		{
			Key(SC_VOLUMEDOWN, 5),
			Key(SC_VOLUMEDOWN, 5),
			Key(SC_VOLUMEDOWN, 5),
			Key(SC_VOLUMEDOWN, 5)
		}
	}));
}

// F12↕ = VOLUMEUP↕
TEST_F(KeyEventTest, handleKey_F12) {
	EXPECT_TRUE(validateKeyMapsAndOutputThem("handleShiftKey.md", {
		{
			KeyDown(SC_F12),
			KeyUp(SC_F12)
		},
		{
			Key(SC_VOLUMEUP, 5),
			Key(SC_VOLUMEUP, 5),
			Key(SC_VOLUMEUP, 5),
			Key(SC_VOLUMEUP, 5)
		}
	}));
}

// Letter↕
TEST_F(KeyEventTest, handleKey_Letter) {
	EXPECT_TRUE(validateKeyMapsAndOutputThem("handleShiftKey.md", {
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
