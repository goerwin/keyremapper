CONFIG ?= $(HOME)/KeyRemapperMac/config.json
SYMBOLS := mac/KeyRemapper/Resources/symbols.json

.PHONY: test test-config build

test:
	g++ -o Tests/output -std=c++17 Tests/index.cpp && ./Tests/output

test-config:
	g++ -o Tests/output.config -std=c++17 Tests/configTests.cpp && ./Tests/output.config "$(CONFIG)" $(SYMBOLS)

build:
	cd mac && xcodebuild -quiet -project KeyRemapper.xcodeproj -target KeyRemapper -configuration Debug CODE_SIGNING_ALLOWED=NO APP_CERTIFICATE=unsigned build
