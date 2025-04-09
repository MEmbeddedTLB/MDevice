// FILE: src/device.h
#ifndef DEVICES_H
#define DEVICES_H

#include <ArduinoJson.h>
#include <vector>
#include <Arduino.h>

// Common structure for all platforms
struct Command {
    String name;
    int status;
};

// Platform-independent base class (interface)
class DeviceBase {
public:
    virtual ~DeviceBase() {}
    
    // Initialization methods
    virtual void initialize() = 0;
    virtual void uninitialize() = 0;
    
    // Data sending methods
    virtual bool sendData(String type, String name, String component, int status) = 0;
    virtual bool sendData(String type, String name, String component, String status) = 0;
    virtual bool sendData(String type, String name, String component, int status, JsonArray dataArray) = 0;
    
    // Connection and command methods
    virtual bool connectWiFi(const char* ssid, const char* password) = 0;
    virtual bool sendDeviceConnect() = 0;
    virtual void checkForCommands() = 0;
    virtual bool getConnectionStatus() const = 0;
    
    // Method to access pending commands
    virtual std::vector<Command>& getPendingCommands() = 0;
};

// Platform detection
#if defined(ESP8266)
    #include "device_esp8266.h"
    typedef DeviceESP8266 Device;
#elif defined(ESP32)
    #include "device_esp32.h"
    typedef DeviceESP32 Device;
#elif defined(ARDUINO_ARCH_STM32) || defined(STM32_CORE_VERSION)
    #include "device_stm32.h"
    typedef DeviceSTM32 Device;
#else
    #include "device_arduino.h"
    typedef DeviceArduino Device;
#endif

#endif // DEVICES_H