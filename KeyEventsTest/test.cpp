#include "pch.h"
#include <vector>
#include <windows.h>
#include "../InterceptionKeyRemapper/interception.h"
#include "../InterceptionKeyRemapper/KeyEvent.cpp"

TEST(TestCaseName, TestName) {
  EXPECT_EQ(1, 1);
  EXPECT_TRUE(true);
}

bool compareKeyEvents(std::vector<Key> keyEvents, std::vector<Key> keyEvents2) {
	int size1 = keyEvents.size();
	int size2 = keyEvents.size();
	
	if (size1 != size2) {
		return false;
	}

	try {	
		for (int i = 0; i < size1; i++) {
			if (
				keyEvents[i].keyCode != keyEvents2[i].keyCode ||
				keyEvents[i].state != keyEvents2[i].state
			) {
				return false;
			}
		}

		return true;
	} catch (const std::exception&)
	{
		return false;
	}
}

TEST(handleKey, F1) {
	auto keyEvents = getKeyEvents({
		InterceptionKeyStroke({ SC_F1, 0 }),
		InterceptionKeyStroke({ SC_F1, 1 })
		});

	EXPECT_TRUE(compareKeyEvents(keyEvents, concatKeyVectors(nullKeyEvent, nullKeyEvent)));
}

TEST(handleKey, F2) {
	auto keyEvents = getKeyEvents({
		InterceptionKeyStroke({ SC_F2, 0 }),
		InterceptionKeyStroke({ SC_F2, 1 })
		});

	EXPECT_TRUE(compareKeyEvents(keyEvents, concatKeyVectors(nullKeyEvent, nullKeyEvent)));
}

TEST(handleKey, F2) {
	auto keyEvents = getKeyEvents({
		InterceptionKeyStroke({ SC_F2, 0 }),
		InterceptionKeyStroke({ SC_F2, 1 })
		});

	EXPECT_TRUE(compareKeyEvents(keyEvents, concatKeyVectors(nullKeyEvent, nullKeyEvent)));
}

TEST(handleKey, Letter) {
	auto keyEvents = getKeyEvents({
		InterceptionKeyStroke({ SC_C, 0 }),
		InterceptionKeyStroke({ SC_C, 1 })
	});

	EXPECT_TRUE(compareKeyEvents(
		keyEvents, 
		{ KeyDown(SC_C), KeyUp(SC_C) }
	));
}