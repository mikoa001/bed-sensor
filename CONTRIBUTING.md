# Contributing to HA-SleepGuard

Thank you for considering contributing to HA-SleepGuard! This document provides guidelines for contributing to the project.

## How to Contribute

### Reporting Issues

If you encounter a bug or have a feature request:

1. **Check existing issues** - Search the issue tracker to avoid duplicates
2. **Create a detailed report** - Include:
   - Hardware used (ESP8266 model, sensor type)
   - Software versions (Arduino IDE, Home Assistant)
   - Steps to reproduce the issue
   - Expected vs actual behavior
   - Serial Monitor output or error messages
   - Photos of wiring (if hardware-related)

### Suggesting Enhancements

We welcome feature suggestions! Please:

1. **Describe the use case** - Why is this feature needed?
2. **Explain the expected behavior** - What should it do?
3. **Consider alternatives** - Are there other ways to achieve this?
4. **Check feasibility** - Is it possible with ESP8266 limitations?

### Code Contributions

#### Getting Started

1. **Fork the repository**
2. **Clone your fork**
   ```bash
   git clone https://github.com/your-username/HA-SleepGuard.git
   cd HA-SleepGuard
   ```
3. **Create a feature branch**
   ```bash
   git checkout -b feature/your-feature-name
   ```

#### Development Guidelines

**ESP8266 Code:**
- Follow Arduino coding style
- Add comments for complex logic
- Keep memory usage minimal (ESP8266 has limited RAM)
- Test on actual hardware before submitting
- Include Serial debug output for troubleshooting

**Home Assistant Configurations:**
- Follow YAML best practices
- Add comments explaining automations
- Test with Home Assistant before submitting
- Consider backwards compatibility

**Documentation:**
- Use clear, simple language
- Include examples where appropriate
- Add diagrams for complex concepts
- Update README if adding features

#### Code Style

**C++ (Arduino/ESP8266):**
```cpp
// Use descriptive variable names
const int SENSOR_PIN = A0;  // Not: const int sp = A0;

// Add comments for non-obvious code
// Calculate rolling average to reduce noise
int getAverageSensorValue() {
    // Implementation
}

// Use consistent indentation (2 spaces)
void setup() {
  Serial.begin(115200);
  if (WiFi.status() != WL_CONNECTED) {
    connectWiFi();
  }
}
```

**YAML (Home Assistant):**
```yaml
# Use clear automation names
- id: 'trigger_aggressive_alarm'
  alias: 'Trigger Aggressive Alarm'
  
  # Add descriptions
  description: 'Start the aggressive alarm at the scheduled time'
  
  # Indent consistently (2 spaces)
  trigger:
    - platform: time
      at: input_datetime.alarm_time
```

#### Testing Your Changes

Before submitting:

1. **Test ESP8266 code:**
   - Upload to actual ESP8266
   - Verify WiFi connection
   - Verify MQTT connection
   - Test sensor readings
   - Check Serial Monitor output

2. **Test Home Assistant integration:**
   - Verify MQTT discovery works
   - Test automations trigger correctly
   - Check entity states update
   - Verify dashboard cards display properly

3. **Test documentation:**
   - Check links work
   - Verify code examples are correct
   - Ensure instructions are clear

#### Submitting a Pull Request

1. **Commit your changes**
   ```bash
   git add .
   git commit -m "Add feature: brief description"
   ```
   
   Commit message format:
   - `Add feature: description` - New features
   - `Fix: description` - Bug fixes
   - `Update docs: description` - Documentation
   - `Refactor: description` - Code improvements

2. **Push to your fork**
   ```bash
   git push origin feature/your-feature-name
   ```

3. **Create Pull Request**
   - Go to the original repository
   - Click "New Pull Request"
   - Select your branch
   - Fill in the PR template:
     - **Description:** What does this PR do?
     - **Motivation:** Why is this change needed?
     - **Testing:** How did you test it?
     - **Screenshots:** If UI changes

4. **PR Review Process**
   - Maintainers will review your code
   - Address any requested changes
   - Once approved, PR will be merged

### Types of Contributions Needed

#### High Priority
- [ ] Additional sensor support (ultrasonic, PIR, etc.)
- [ ] Power optimization (deep sleep modes)
- [ ] Multi-sensor support
- [ ] Alternative communication protocols (HTTP, WebSocket)
- [ ] Mobile app integration examples

#### Medium Priority
- [ ] Additional automation examples
- [ ] Sensor calibration tools/scripts
- [ ] 3D printable enclosure designs
- [ ] PCB design for permanent installation
- [ ] Video tutorials

#### Always Welcome
- [ ] Documentation improvements
- [ ] Bug fixes
- [ ] Translation to other languages
- [ ] Example configurations
- [ ] Troubleshooting additions

### Documentation Contributions

Help improve documentation:

1. **Fix typos/errors** - Small fixes are always welcome
2. **Add examples** - Share your working configurations
3. **Improve clarity** - Make instructions easier to understand
4. **Add troubleshooting** - Share solutions to problems you've solved
5. **Translate** - Help make content accessible to more users

### Community Guidelines

- **Be respectful** - Treat everyone with respect
- **Be helpful** - Share knowledge and assist others
- **Be patient** - Everyone was a beginner once
- **Be constructive** - Provide helpful feedback
- **Give credit** - Acknowledge others' contributions

### Hardware Contributions

If you've tested with different hardware:

1. **Document your setup**
   - Hardware specifications
   - Wiring diagrams
   - Configuration values
   - Photos

2. **Share results**
   - What works well?
   - What issues did you encounter?
   - Performance characteristics
   - Cost and availability

3. **Create guides**
   - Step-by-step instructions
   - Troubleshooting tips
   - Optimization suggestions

### Feature Requests

When requesting features:

1. **Search existing requests** - Avoid duplicates
2. **Describe the problem** - What are you trying to solve?
3. **Propose a solution** - How could this work?
4. **Consider impact** - Who else would benefit?
5. **Discuss feasibility** - Are there technical limitations?

### Questions and Support

- **Documentation first** - Check README, guides, and wiki
- **Search issues** - Your question may be answered
- **Ask in discussions** - Use GitHub Discussions for questions
- **Be specific** - Provide details for better help

### Recognition

Contributors will be:
- Listed in the README
- Mentioned in release notes
- Credited in their contributions

Thank you for helping make HA-SleepGuard better!

## Development Setup

### Requirements
- Arduino IDE 1.8.x or 2.x
- ESP8266 board support installed
- PubSubClient library
- Home Assistant instance for testing
- MQTT broker (Mosquitto recommended)

### Local Testing
```bash
# Clone repository
git clone https://github.com/mikoa001/HA-SleepGuard.git

# Install Arduino IDE and dependencies
# See README.md for detailed setup

# Test ESP8266 code
# Open esp8266_bed_sensor/bed_sensor.ino in Arduino IDE
# Update config values
# Upload to ESP8266
# Monitor Serial output

# Test Home Assistant config
# Copy homeassistant/*.yaml to your HA config
# Restart Home Assistant
# Verify entities appear
```

### Need Help?

- **Documentation:** Check the README and guides
- **Issues:** Open an issue for bugs or feature requests
- **Discussions:** Use GitHub Discussions for questions
- **Email:** [Project maintainer contact if available]

## License

By contributing, you agree that your contributions will be licensed under the MIT License.
