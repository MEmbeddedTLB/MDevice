// MDevice.h
#ifndef MDevice_H
#define MDevice_H

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
    virtual bool sendData(String type, String name, String component, double status) = 0;
    virtual bool sendData(String type, String name, String component, long status) = 0;
    
    // Connection and command methods
    virtual bool connectWiFi(const char* ssid, const char* password) = 0;
    virtual bool sendDeviceConnect() = 0;
    virtual void checkForCommands() = 0;
    virtual bool getConnectionStatus() const = 0;
    
    // Method to access pending commands
    virtual std::vector<Command>& getPendingCommands() = 0;
};

// Forward declarations of derived classes
class DeviceESP32;
class DeviceSTM32;
class DeviceArduino;

// Platform detection and implementation selection
// Important: We'll include the specific device implementations AFTER we've defined the base class

#if defined(ESP32)
    #include "device_esp.h"
    typedef DeviceESP32 Device;
#elif defined(ARDUINO_ARCH_STM32) || defined(STM32_CORE_VERSION)
    #include "device_stm.h"
    typedef DeviceSTM32 Device;
#else
    #include "device_arduino.h"
    typedef DeviceArduino Device;
#endif

#endif // MDevice_H