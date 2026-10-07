CONFIG ?= $(HOME)/KeyRemapperMac/config.json
PROFILE ?= 0
SYMBOLS := mac/KeyRemapper/Resources/symbols.json
ICON_SVG := common/images/icon.svg
MENUBAR_ICON_SVG := common/images/menubar-icon.svg
APP_ICONSET := mac/KeyRemapper/Assets.xcassets/AppIcon.appiconset
MENUBAR_IMAGESET := mac/KeyRemapper/Assets.xcassets/MenuBarIcon.imageset

FRAMEWORKS := -framework AppKit -framework IOKit -framework ApplicationServices

.PHONY: test test-runtime test-app test-config build dev-build dev icon release-patch release-minor release-major

test:
	g++ -o Tests/output -std=c++17 Tests/index.cpp && ./Tests/output

test-runtime:
	mkdir -p mac/build && clang++ -std=c++17 -fobjc-arc -o mac/build/runtime-tests mac/Tests/runtime.mm $(FRAMEWORKS) && mac/build/runtime-tests mac/Tests/runtime.json $(SYMBOLS)

test-app:
	mac/Tests/app.sh

test-config:
	g++ -o Tests/output.config -std=c++17 Tests/configTests.cpp && ./Tests/output.config "$(CONFIG)" $(SYMBOLS)

build:
	cd mac && xcodebuild -quiet -project KeyRemapper.xcodeproj -target KeyRemapper -configuration Debug CODE_SIGNING_ALLOWED=NO build

dev-build:
	mkdir -p mac/build && clang++ -std=c++17 -fobjc-arc -o mac/build/keyremapper-dev mac/Dev/main.mm $(FRAMEWORKS)

dev: dev-build
	sudo mac/build/keyremapper-dev "$(CONFIG)" $(SYMBOLS) --profile $(PROFILE) $(if $(LOG),--log)

# Requires rsvg-convert (brew install librsvg)
icon:
	for size in 16 32 64 128 256 512 1024; do rsvg-convert -w $$size -h $$size $(ICON_SVG) -o $(APP_ICONSET)/$$size.png; done
	cp $(MENUBAR_ICON_SVG) $(MENUBAR_IMAGESET)/icon.svg

release-patch release-minor release-major:
	./release.sh $(@:release-%=%)
