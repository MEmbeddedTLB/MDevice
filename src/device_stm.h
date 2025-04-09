// FILE: src/device_stm32.h
#ifndef DEVICE_STM32_H
#define DEVICE_STM32_H

#include <WiFi.h>  // STM32 WiFi library
#include <WiFiClient.h>
#include <ArduinoJson.h>
#include <vector>
#include "device.h" // Include base class

class DeviceSTM32 : public DeviceBase {
private:
    // Server details
    String server_url;
    String deviceId;
    
    // API endpoints
    String connect_endpoint;
    String commands_endpoint;
    String data_endpoint;
    
    StaticJsonDocument<512> doc;
    char output[256];
    
    WiFiClient client;
    bool isConnected = false;
    
    // Vector to store pending commands
    std::vector<Command> pendingCommands;
    
    // Private helper method for sending HTTP requests
    bool sendHttpRequest(const String& method, const String& endpoint, const String& payload = "");

public:
    // Constructor
    DeviceSTM32(char* devId);
    
    // Destructor
    virtual ~DeviceSTM32();
    
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

#endif // DEVICE_STM32_H