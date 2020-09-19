#pragma once

#include "libraries/json.hpp"

namespace Symbols
{
  using json = nlohmann::json;

  nlohmann::json symbols{
    {"A", 0x1E},
    {"B", 0x30},
    {"C", 0x2E},
    {"D", 0x20},
    {"E", 0x12},
    {"F", 0x21},
    {"G", 0x22},
    {"H", 0x23},
    {"I", 0x17},
    {"J", 0x24},
    {"K", 0x25},
    {"L", 0x26},
    {"M", 0x32},
    {"N", 0x31},
    {"O", 0x18},
    {"P", 0x19},
    {"Q", 0x10},
    {"R", 0x13},
    {"S", 0x1F},
    {"T", 0x14},
    {"U", 0x16},
    {"V", 0x2F},
    {"W", 0x11},
    {"X", 0x2D},
    {"Y", 0x15},
    {"Z", 0x2C},
    {"1", 0x02},
    {"2", 0x03},
    {"3", 0x04},
    {"4", 0x05},
    {"5", 0x06},
    {"6", 0x07},
    {"7", 0x08},
    {"8", 0x09},
    {"9", 0x0A},
    {"0", 0x0B},
    {"NP1", 0x4F},
    {"NP2", 0x50},
    {"NP3", 0x51},
    {"NP4", 0x4B},
    {"NP5", 0x4C},
    {"NP6", 0x4D},
    {"NP7", 0x47},
    {"NP8", 0x48},
    {"NP9", 0x49},
    {"NP0", 0x52},
    {"MUTE", 0xA0},
    {"VOLUMEDOWN", 0xAE},
    {"VOLUMEUP", 0xB0},
    {"Esc", 0x01},
    {"Caps", 0x3A},
    {"Left", 0xCB},
    {"Right", 0xCD},
    {"Up", 0xC8},
    {"Down", 0xD0},
    {"Space", 0x39},
    {"Win", 0x5B},
    {"Win", 0x5C},
    {"Alt", 0x38},
    {"Alt", 0xB8},
    {"Ctrl", 0x1D},
    {"Ctrl", 0x9D},
    {"Shift", 0x2A},
    {"Shift", 0x36},
    {"LBSLASH", 0x56},
    {"Enter", 0x1C},
    {"Supr", 0x53},
    {"Back", 0x0E},
    {"Tab", 0x0F},
    {"Home", 0xC7},
    {"End", 0xCF},
    {"Prior", 0xC9},
    {"Next", 0xD1},
    {";", 0x27},
    {"-", 0x0C},
    {"`", 0x29},
    {"NumLock", 0x45},
    {"Insert", 0xD2},
    {"Del", 0xD3},
    {"F1", 0x3B},
    {"F2", 0x3C},
    {"F3", 0x3D},
    {"F4", 0x3E},
    {"F5", 0x3F},
    {"F6", 0x40},
    {"F7", 0x41},
    {"F8", 0x42},
    {"F9", 0x43},
    {"F10", 0x44},
    {"F11", 0x57},
    {"F12", 0x58},
    {"MOUSELEFT", 0xF1},
    {"MOUSERIGHT", 0xF2},
    {"BRIGHTNESSDOWN", 0xF3},
    {"BRIGHTNESSUP", 0xF4} };

  std::string getSymbolByScanCode(short scanCode)
  {
    for (json::iterator it = symbols.begin(); it != symbols.end(); ++it)
    {
      if (it.value() == scanCode)
        return it.key();
    }

    return nullptr;
  }

  short getSymbolScanCode(std::string symbol)
  {
    short scanCode = symbols[symbol];
    return scanCode;
  }
} // namespace Symbols
