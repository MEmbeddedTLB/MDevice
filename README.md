# MDevice Library for Arduino, ESP32, and STM32

A platform-independent connectivity library to build IoT projects that work across different hardware with minimal code changes.

## Overview

The MDevice library eliminates the headache of managing different WiFi implementations across Arduino, ESP32, and STM32 boards. Write your code once and deploy it to any supported platform without changing your core application logic.

## Features

- **Cross-platform compatibility** - Works seamlessly on Arduino (with WiFi support), ESP32, and STM32 boards
- **Automatic platform detection** - No need to modify your code when switching hardware
- **Simple API** - Common interface for all your connectivity needs
- **Command handling** - Receive and process commands from a remote server
- **JSON support** - Send structured data with minimum effort

## Installation

### Using Arduino Library Manager

1. Open the Arduino IDE
2. Go to Sketch → Include Library → Manage Libraries...
3. Search for "MDevice"
4. Look for "MDevice by MEmbedded TechLab"
5. Click Install

### Manual Installation

1. Download the latest release as ZIP
2. In Arduino IDE: Sketch → Include Library → Add .ZIP Library...
3. Select the downloaded ZIP file

## Dependencies

This library requires:
- ArduinoJson (6.0.0+)
- WiFiNINA (for Arduino boards with WiFi)
- ArduinoHttpClient (for STM32 boards)

These will be automatically installed if you use the Arduino Library Manager.

## Quick Start

```cpp
#include <MDevice.h>

// The Device type is automatically selected based on your platform
Device myDevice("device123");

void setup() {
  Serial.begin(115200);
  
  // Initialize the device
  myDevice.initialize();
  
  // Connect to WiFi
  if(myDevice.connectWiFi("your_ssid", "your_password")) {
    Serial.println("Connected to WiFi!");
    
    // Register device with server
    if(myDevice.sendDeviceConnect()) {
      Serial.println("Device registered!");
    }
  }
}

void loop() {
  // Check for incoming commands
  myDevice.checkForCommands();
  
  // Process any pending commands
  auto& commands = myDevice.getPendingCommands();
  for(auto& cmd : commands) {
    Serial.print("Received command: ");
    Serial.print(cmd.name);
    Serial.print(" with status: ");
    Serial.println(cmd.status);
    
    // Process command here...
  }
  commands.clear();  // Clear processed commands
  
  // Send some data to the server
  myDevice.sendData("sensor", "temp_sensor", "temperature", 25);
  
  delay(5000);  // Wait 5 seconds before next update
}
```

## Platform-Specific Details

The library uses conditional compilation to select the appropriate implementation:

- **Arduino boards** with WiFi support (Nano 33 IoT, MKR WiFi 1010, etc.) - Uses WiFiNINA library
- **ESP32 boards** - Uses the native ESP32 WiFi and HTTP libraries
- **STM32 boards** - Uses ArduinoHttpClient with WiFi support

## Command Structure

Commands received from the server follow this structure:

```cpp
struct Command {
  String name;   // Name of the command
  int status;    // Status or value associated with the command
};
```

## Data Sending Methods

The library provides multiple ways to send data:

```cpp
// Send numeric data
myDevice.sendData("sensor", "temp_sensor", "temperature", 25);

// Send string data
myDevice.sendData("status", "system", "message", "all systems normal");

// Send complex data with array
JsonArray dataArray = doc.createNestedArray("readings");
dataArray.add(25);
dataArray.add(26);
dataArray.add(24);
myDevice.sendData("sensor", "temp_sensor", "temperature_history", 3, dataArray);
```

## About

Developed by [MEmbedded TechLab](https://github.com/MEmbeddedTLB) 