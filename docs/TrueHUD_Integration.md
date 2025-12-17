# TrueHUD Integration for MaxsuPoise

## Overview

MaxsuPoise now integrates with [TrueHUD](https://www.nexusmods.com/skyrimspecialedition/mods/62775) to display the poise bar as a special resource bar in the HUD. This is an **optional feature** - MaxsuPoise will work with or without TrueHUD installed.

## Features

### Poise Bar Display
- **Current Poise** vs **Maximum Poise** shown as a special bar
- Dynamically calculates max poise based on:
  - Actor mass and scale
  - Equipped armor (heavy armor provides +50% poise)
  - Base poise values from `MaxsuPoise.ini`

### Visual Feedback
- **Long Flash**: When poise breaks completely (stagger level = Largest)
- **Short Flash**: When taking medium or higher poise damage (without breaking)
- Provides immediate visual feedback on poise status

## Installation

1. Install MaxsuPoise as usual
2. Install [TrueHUD](https://www.nexusmods.com/skyrimspecialedition/mods/62775) (optional)
3. Load both mods - integration happens automatically

## Technical Details

### API Usage

MaxsuPoise uses TrueHUD's Special Resource Bar API:
- **Request Control**: Takes control of the special resource bar slot
- **Register Callbacks**: Provides functions to query current/max poise
- **Flash Effects**: Triggers visual feedback on poise damage

### Implementation Files

- [TrueHUDAPI.h](../include/TrueHUDAPI.h) - TrueHUD API header (v4 compatible)
- [TrueHUDHandler.h](../include/TrueHUDHandler.h) - Integration handler interface
- [TrueHUDHandler.cpp](../src/TrueHUDHandler.cpp) - Implementation with callbacks

### Integration Points

1. **Initialization** ([LoadGame.cpp](../src/LoadGame.cpp))
   - Checks for TrueHUD presence
   - Requests special bar control
   - Registers poise callbacks

2. **Poise Damage** ([StaggerHandler.cpp](../src/StaggerHandler.cpp))
   - Long flash on poise break
   - Short flash on medium+ stagger
   - No flash on small staggers (visual clarity)

## Compatibility

- **TrueHUD v1.1.8+**: Fully compatible
- **Without TrueHUD**: All features work normally, no HUD bar shown
- **Other Special Bar Mods**: If another mod takes the special bar slot, MaxsuPoise logs a warning and continues without visual integration

## For Developers

### Adding Flash Effects

```cpp
#include "TrueHUDHandler.h"

// Flash the poise bar
TrueHUDHandler::GetSingleton()->FlashPoiseBar(actor, true); // long flash
TrueHUDHandler::GetSingleton()->FlashPoiseBar(actor, false); // short flash
```

### Checking if TrueHUD is Available

```cpp
if (TrueHUDHandler::GetSingleton()->IsEnabled()) {
    // TrueHUD integration is active
}
```

### Custom Bar Colors

TrueHUD allows users to customize bar colors in its MCM. The poise bar will respect those settings.

## Credits

- **TrueHUD** by ersh1 - [GitHub](https://github.com/ersh1/TrueHUD)
- Integration implements TrueHUD API v4
