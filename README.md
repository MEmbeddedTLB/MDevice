# Device Library

## Overview

Device is a versatile WiFi-enabled device communication library designed to simplify IoT device connectivity across multiple platforms including Arduino, ESP8266, ESP32, and STM32 boards.

## Features

- Multi-platform WiFi connectivity support
- HTTP-based device communication
- Easy data transmission
- Command management
- Secure device connection handling

## Supported Platforms

- Arduino
- ESP8266
- ESP32
- STM32

## Installation

### Arduino IDE
1. Download the library from GitHub
2. In Arduino IDE, go to Sketch > Include Library > Add .ZIP Library
3. Select the downloaded library ZIP file

### PlatformIO
Add to your `platformio.ini`:
```ini
lib_deps = 
    Device
```

## Basic Usage

### Initialization
```cpp
#include <.h>

// Create device with unique ID
Device myDevice("ID");

void setup() {
    // Connect to WiFi
    myDevice.connectWiFi("YourSSID", "YourPassword");
}

void loop() {
    // Send sensor data
    myDevice.sendData("temperature", "Living Room", "TempSensor", 22);

    // Check for pending commands
    myDevice.checkForCommands();
}
```

## Key Methods

### Connection
- `connectWiFi(ssid, password)`: Connect to WiFi network
- `sendDeviceConnect()`: Register device with server
- `getConnectionStatus()`: Check current connection status

### Data Transmission
- `sendData(type, name, component, status)`: Send device data
- `sendData(type, name, component, status, string)`: Send device data
- `sendData(type, name, component, status, dataArray)`: Send data with additional array

### Command Management
- `checkForCommands()`: Retrieve pending commands
- `getPendingCommands()`: Access list of pending commands

## Configuration
**Important Note**: Device configuration and code generation are managed through our web platform 

### Code Generation Workflow

1. Visit our web platform
2. Configure your device specifications
3. Select desired data components
4. Generate custom device communication code
5. Download and upload to your device

### Web Platform Features

- Drag-and-drop component selection
- Real-time code preview
- Automatic library configuration
- Device ID management
- Endpoint configuration


### Server Configuration
Modify the following in the constructor:
```cpp
Device myDevice("ID");
```

## Error Handling

The library provides basic error logging via Serial:
- WiFi connection failures
- HTTP request errors
- JSON parsing issues

## Dependencies

- ArduinoJson
- Platform-specific WiFi libraries

## Advanced Configuration

### Custom Endpoints
You can customize API endpoints during device initialization:
```cpp
Device myDevice("ID");
```

## Examples


### Command Processing
```cpp
void loop() {
    myDevice.checkForCommands();
    
    // Process any received commands
    for (auto& cmd : myDevice.getPendingCommands()) {
        if (cmd.name == "TURN_ON_LIGHT") {
            digitalWrite(LIGHT_PIN, HIGH);
        }
    }
}
```

## Troubleshooting

- Ensure correct WiFi credentials
- Check server URL and port
- Verify device ID uniqueness
- Monitor Serial output for diagnostic information


## License

This project is licensed under the MIT License - see the LICENSE file for details.

## Support

For issues, please file a GitHub issue with:
- Platform details
- Library version
- Minimal reproducible example
- Serial monitor output

## Version

Current Version: 1.0.0
