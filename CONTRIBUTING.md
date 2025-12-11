# Contributing to Low Power Watchface

Thank you for your interest in contributing to the Low Power watchface for Pebble! We welcome contributions from the community.

## Ways to Contribute

### 🐛 Report Bugs

If you find a bug, please open an issue with:
- A clear description of the problem
- Steps to reproduce the issue
- Expected vs actual behavior
- Your Pebble model and platform (aplite, basalt, chalk, etc.)
- Screenshots if applicable

### 💡 Suggest Features

Have an idea to improve the watchface? Open an issue with:
- A clear description of the feature
- Why it would be useful
- Any implementation ideas you have

### 🔧 Submit Pull Requests

We welcome code contributions! Here's how:

1. **Fork the repository**
2. **Create a feature branch**
   ```bash
   git checkout -b feature/your-feature-name
   ```
3. **Make your changes**
4. **Test thoroughly** on at least one Pebble platform
5. **Commit with clear messages**
   ```bash
   git commit -m "Add: Brief description of your change"
   ```
6. **Push to your fork**
   ```bash
   git push origin feature/your-feature-name
   ```
7. **Open a Pull Request**

## Development Setup

### Prerequisites

- Pebble SDK 3
- A Pebble watch or emulator
- Basic knowledge of C and JavaScript

### Building the Project

```bash
# Clone the repository
git clone https://github.com/yourusername/low-power.git
cd low-power

# Build the watchface
pebble build

# Install to phone
pebble install --phone YOUR_PHONE_IP

# Or install to emulator
pebble install --emulator basalt
```

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
│       └── roboto-bold.ttf       # Custom font resource
├── package.json                  # Pebble configuration
├── wscript                       # Build script
├── LICENSE                       # Apache 2.0 License
├── README.md                     # Main documentation
└── CONTRIBUTING.md               # This file
```

## Coding Guidelines

### C Code (src/c/low-power.c)

- Follow existing code style and indentation
- Use descriptive variable names with `s_` prefix for static variables
- Add comments for complex logic
- Keep functions focused and modular
- Test on multiple Pebble platforms when possible

**Example:**
```c
static TextLayer *s_time_layer;

static void update_time() {
  // Clear, focused function
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);
  // ...
}
```

### JavaScript Code (src/pkjs/index.js)

- Use clear variable names
- Add console.log statements for debugging
- Handle errors gracefully
- Test configuration page on actual phone

**Example:**
```javascript
Pebble.addEventListener('webviewclosed', function(e) {
  if (e.response) {
    console.log('Config returned:', e.response);
    // Handle configuration...
  }
});
```

### Configuration Page (HTML in index.js)

- Keep UI simple and intuitive
- Ensure mobile-friendly design
- Test on different screen sizes
- Maintain consistent styling

## Feature Ideas

Here are some features we'd love to see contributions for:

### High Priority
- [ ] Battery percentage display
- [ ] Bluetooth connection indicator
- [ ] Additional font size options
- [ ] More color themes/presets
- [ ] Configurable date format (MM/DD vs DD/MM)

### Medium Priority
- [ ] Weather integration (requires API)
- [ ] Step counter display
- [ ] Customizable layout positions
- [ ] Animation effects
- [ ] Multiple timezone support

### Low Priority
- [ ] Vibration on hour change (optional)
- [ ] Night mode (auto dim at night)
- [ ] Calendar event display
- [ ] Custom font upload support

## Testing Checklist

Before submitting a pull request, please test:

- [ ] **Build**: Code compiles without errors
- [ ] **Platforms**: Test on at least one platform (basalt recommended)
- [ ] **Color watches**: Features work on color Pebbles (basalt/chalk/emery)
- [ ] **B&W watches**: Features work on B&W Pebbles (aplite/diorite)
- [ ] **Configuration**: Settings page opens and saves correctly
- [ ] **Persistence**: Settings persist after watchface restart
- [ ] **Battery**: No significant battery drain introduced
- [ ] **Memory**: Check memory usage reports in build output

## Code Review Process

1. **Automated checks**: Code must build successfully
2. **Manual review**: A maintainer will review your code
3. **Testing**: Changes will be tested on physical devices when possible
4. **Feedback**: You may be asked to make adjustments
5. **Merge**: Once approved, your PR will be merged!

## Commit Message Guidelines

Use clear, descriptive commit messages:

- **Add**: New feature
  ```
  Add: Battery percentage display option
  ```
- **Fix**: Bug fix
  ```
  Fix: Seconds not updating when enabled
  ```
- **Update**: Modify existing feature
  ```
  Update: Increase default font size to 18pt
  ```
- **Refactor**: Code restructuring
  ```
  Refactor: Extract color management into separate function
  ```
- **Docs**: Documentation changes
  ```
  Docs: Update README with new color options
  ```

## Getting Help

- **Issues**: Open an issue for questions or problems
- **Discussions**: Use GitHub Discussions for general questions
- **Documentation**: Check README.md and code comments

## License

By contributing, you agree that your contributions will be licensed under the Apache License 2.0, the same license as the project.

## Recognition

Contributors will be acknowledged in the README.md file! 🎉

Thank you for helping make Low Power watchface better! ⌚✨

