// FILE: src/device_arduino.h
#ifndef DEVICE_ARDUINO_H
#define DEVICE_ARDUINO_H

#if defined(ARDUINO_ARCH_AVR) || defined(ARDUINO_ARCH_SAMD) || defined(ARDUINO_ARCH_SAM) || \
      defined(ARDUINO_ARCH_MBED) || defined(ARDUINO_ARCH_RP2040) || defined(ARDUINO_ARCH_NRF52) || \
      defined(ARDUINO) 
#include <Arduino.h>
#include <ArduinoJson.h>

#if !defined(ESP32)
#include <WiFiNINA.h>
#endif

#include "MDevice.h"

class DeviceArduino : public DeviceBase {
private:
    StaticJsonDocument<1024> doc;
    
    // For non-ESP32 platforms
  
    WiFiClient client;
  
    
    String output;
    String deviceId;
    String server_url;
    String connect_endpoint;
    String commands_endpoint;
    String data_endpoint;
    bool isConnected = false;
    std::vector<Command> pendingCommands;
    
    // Helper method for HTTP requests
    bool sendHttpRequest(String endpoint, String jsonPayload, String& response);
    bool performHttpGet(String endpoint, String& response);
    
public:
    DeviceArduino(const char* devId);
    ~DeviceArduino();
    
    virtual void initialize() override;
    virtual bool sendData(String type, String name, String component, int status) override;
    virtual bool sendData(String type, String name, String component, String status) override;
    virtual bool sendData(String type, String name, String component, int status, JsonArray dataArray) override;
    virtual bool sendData(String type, String name, String component, double status) override;
    virtual bool sendData(String type, String name, String component, long status) override;
    virtual void uninitialize() override;
    virtual bool connectWiFi(const char* ssid, const char* password) override;
    virtual bool sendDeviceConnect() override;
    virtual void checkForCommands() override;
    virtual bool getConnectionStatus() const override;
    virtual std::vector<Command>& getPendingCommands() override;
};

#endif
#endif // DEVICE_ARDUINO_H