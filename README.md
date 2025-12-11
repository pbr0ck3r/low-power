# Low Power

> A minimalist, configurable watchface for Pebble smartwatches

[![License](https://img.shields.io/badge/License-Apache%202.0-blue.svg)](LICENSE)
[![Platform](https://img.shields.io/badge/Platform-Pebble-orange.svg)](https://rebble.io/)

Low Power is a clean, customizable watchface featuring large time display with optional date and seconds. Designed for readability and battery efficiency.

## Features

- ⌚ **Large Time Display** - Custom 72px font for easy reading
- 📅 **Configurable Date** - Toggle weekday, month, and day display
- ⏱️ **Optional Seconds** - Show/hide seconds on middle left
- 🎨 **Customizable Colors** - 9 colors for text and background (color watches)
- 📱 **Local Configuration** - No internet required
- 💾 **Persistent Settings** - Preferences saved across restarts
- 🔋 **Smart Battery Management** - Adaptive timer switches between second/minute updates
- 🔧 **Platform-Aware** - Automatically adapts to watch capabilities

## Compatibility

Works on all Pebble platforms:
- ✅ Aplite (original Pebble)
- ✅ Basalt (Pebble Time) 
- ✅ Chalk (Pebble Time Round)
- ✅ Diorite (Pebble 2)
- ✅ Emery (Pebble Time 2)
- ✅ Flint (Pebble 2)

## Installation

### From Source

```bash
git clone https://github.com/yourusername/low-power.git
cd low-power
pebble build

# Install to phone
pebble install --phone YOUR_PHONE_IP

# Or install to emulator
pebble install --emulator basalt
```

### Pre-built

Upload the generated `.pbw` file through the Pebble mobile app.

## Configuration

### Accessing Settings

1. Open the **Pebble app** on your phone
2. Navigate to watchfaces → **Low Power** → **⚙️ Settings**
3. Configure your preferences
4. Tap **Save Settings**

### Available Options

| Setting | Description | Default |
|---------|-------------|---------|
| Show Date | Display weekday, month, and day | ✓ ON |
| Show Seconds | Display seconds (increases battery usage) | ✗ OFF |
| Text Color* | Choose from 9 colors | White |
| Background Color* | Choose from 9 colors | Black |

\* _Color options available on Basalt, Chalk, and Emery only_

### Color Palette

<table>
  <tr>
    <td>🍅 Tomato</td>
    <td>🏖️ Sandy Brown</td>
    <td>🥇 Gold</td>
  </tr>
  <tr>
    <td>💎 Aquamarine</td>
    <td>🌿 Green</td>
    <td>🌊 Deep Sky Blue</td>
  </tr>
  <tr>
    <td>🌸 Plum</td>
    <td>⚪ White</td>
    <td>⚫ Black</td>
  </tr>
</table>

## Layout

```
┌─────────────────┐
│ Mon             │  ← Date (if enabled)
│ Dec       ┌──┐  │
│ 11        │12│  │  ← Hour (large)
│           └──┘  │
│ 45              │  ← Seconds (if enabled)
│                 │
│           :34   │  ← Minutes with colon (large)
└─────────────────┘
```

## Popular Color Schemes

### Classic
White text on Black background - High contrast, easy to read

### Inverted
Black text on White background - Bright, modern look

### Ocean
White/Aquamarine text on Deep Sky Blue - Cool, calming theme

### Forest
White text on Green background - Natural, easy on the eyes

### Sunset
Gold text on Tomato background - Warm, vibrant look

### Elegant
Plum text on Black background - Sophisticated and subtle

## Development

### Project Structure

```
low-power/
├── src/
│   ├── c/
│   │   └── low-power.c          # Main watchface C code
│   └── pkjs/
│       └── index.js              # JavaScript configuration handler
├── resources/
│   └── fonts/
│       └── roboto-bold.ttf       # Custom 72px font
├── package.json                  # Pebble configuration
├── wscript                       # Build script
├── LICENSE                       # Apache 2.0 License
├── README.md                     # This file
└── CONTRIBUTING.md               # Contribution guidelines
```

### Building

```bash
# Clean build
pebble clean
pebble build

# Install to phone
pebble install --phone <PHONE_IP>

# Install to emulator
pebble install --emulator basalt  # or aplite, chalk, diorite, emery

# View logs (phone)
pebble logs --phone <PHONE_IP>

# View logs (emulator)
pebble logs --emulator basalt
```

### Message Keys

| Key | Type | Description |
|-----|------|-------------|
| `ShowDate` | boolean | Controls date display visibility |
| `ShowSeconds` | boolean | Controls seconds display visibility |
| `TextColor` | integer | Hex color value for text |
| `BackgroundColor` | integer | Hex color value for background |

## Debugging

Enable Safari Web Inspector to debug the configuration page:

### iOS Setup
1. **Settings** → **Safari** → **Advanced** → Enable **Web Inspector**

### macOS Setup
1. **Safari** → **Preferences** → **Advanced** → Enable **Show Develop menu**

### Debug Session
1. Connect iPhone to Mac via Lightning cable
2. Open Low Power settings in Pebble app
3. Safari → **Develop** → [Your iPhone] → Configuration page
4. Use Console, Elements, and Sources tabs to debug

See the [full debugging guide](README.md#debugging) for more details.

## Battery Impact

⚠️ **Seconds Display:**
- **OFF** (default): Updates every minute → Minimal battery drain
- **ON**: Updates 60 times per minute → Significantly higher battery drain

💡 _Enable seconds only when needed for precise timing_

## Technical Details

- **SDK**: Pebble SDK 3
- **Configuration**: Local HTML generation via JavaScript data URI
- **Storage**: Persistent storage API for all settings
- **Timer**: Dynamic switching between SECOND_UNIT and MINUTE_UNIT
- **Fonts**: System fonts + custom Roboto Bold 72px
- **Colors**: Platform-aware using GColorFromHEX
- **Detection**: Automatic watch capability detection

## Troubleshooting

<details>
<summary><strong>Settings don't open</strong></summary>

- Verify Bluetooth is enabled and watch is connected
- Restart the Pebble app
- Ensure watchface installed correctly
</details>

<details>
<summary><strong>Settings don't save</strong></summary>

- Enable Web Inspector and check Console for errors
- Verify you tapped "Save Settings"
- Try toggling settings one at a time
</details>

<details>
<summary><strong>Seconds not updating</strong></summary>

- Confirm "Show Seconds" is enabled
- Ensure watchface is active (not in menu)
- Check battery level
</details>

<details>
<summary><strong>Color settings not showing</strong></summary>

Color customization is only available on:
- ✅ Basalt (Pebble Time)
- ✅ Chalk (Pebble Time Round)  
- ✅ Emery (Pebble Time 2)

Black & white watches (Aplite, Diorite, Flint) always use white on black.
</details>

## Contributing

Contributions are welcome! Please see [CONTRIBUTING.md](CONTRIBUTING.md) for:
- Bug reports and feature requests
- Development guidelines
- Pull request process
- Feature roadmap

## License

Licensed under the Apache License 2.0 - see [LICENSE](LICENSE) for details.

**Permissions:**
- ✓ Commercial use
- ✓ Modification
- ✓ Distribution
- ✓ Private use

**Conditions:**
- Include copyright notice
- Include license text
- State changes
- Include NOTICE file (if applicable)

## Credits

- **Developer**: [pbr0ck3r](https://github.com/pbr0ck3r)
- **Font**: Roboto Bold ([Apache License 2.0](https://fonts.google.com/specimen/Roboto))
- **Platform**: [Pebble SDK 3](https://developer.rebble.io/)

## Resources

- [Pebble Developer Documentation](https://developer.rebble.io/)
- [Rebble Community](https://rebble.io/)
- [Pebble AppMessage Guide](https://developer.rebble.io/developer.pebble.com/guides/communication/using-pebblekit-js/index.html)

---

**Made with ❤️ for the Pebble community** | ⌚ [Get Started](#installation)
