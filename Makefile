CONFIG ?= $(HOME)/KeyRemapperMac/config.json
PROFILE ?= 0
SYMBOLS := mac/KeyRemapper/Resources/symbols.json

.PHONY: test test-config build dev-build dev

test:
	g++ -o Tests/output -std=c++17 Tests/index.cpp && ./Tests/output

test-config:
	g++ -o Tests/output.config -std=c++17 Tests/configTests.cpp && ./Tests/output.config "$(CONFIG)" $(SYMBOLS)

build:
	cd mac && xcodebuild -quiet -project KeyRemapper.xcodeproj -target KeyRemapper -configuration Debug CODE_SIGNING_ALLOWED=NO APP_CERTIFICATE=unsigned build

dev-build:
	mkdir -p mac/build && clang++ -std=c++17 -fobjc-arc -o mac/build/keyremapper-dev mac/Dev/main.mm -framework AppKit -framework IOKit -framework ApplicationServices

dev: dev-build
	sudo mac/build/keyremapper-dev "$(CONFIG)" $(SYMBOLS) --profile $(PROFILE) $(if $(LOG),--log)
