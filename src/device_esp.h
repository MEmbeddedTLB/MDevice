// FILE: src/device_esp32.h
#ifndef DEVICE_ESP32_H
#define DEVICE_ESP32_H

#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <vector>
#include "device.h" // Include base class

class DeviceESP32 : public DeviceBase {
private:
    // Server details
    String server_url;
    String deviceId;
    
    // API endpoints
    String connect_endpoint;
    String commands_endpoint;
    String data_endpoint;
    
    DynamicJsonDocument doc;
    String output;
    HTTPClient httpClient;
    bool isConnected = false;
    
    // Vector to store pending commands
    std::vector<Command> pendingCommands;

public:
    // Constructor
    DeviceESP32(char* devId);
    
    // Destructor
    virtual ~DeviceESP32();
    
    // Initialization methods
    virtual void initialize() override;
    virtual void uninitialize() override;
    
    // Data sending methods
    virtual bool sendData(String type, String name, String component, int status) override;
    virtual bool sendData(String type, String name, String component, String status) override;
    virtual bool sendData(String type, String name, String component, int status, JsonArray dataArray) override;
    
    // Connection and command methods
    virtual bool connectWiFi(const char* ssid, const char* password) override;
    virtual bool sendDeviceConnect() override;
    virtual void checkForCommands() override;
    virtual bool getConnectionStatus() const override;
    
    // Method to access pending commands
    virtual std::vector<Command>& getPendingCommands() override;
};

#endif // DEVICE_ESP32_H