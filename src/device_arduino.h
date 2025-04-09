// FILE: src/device_arduino.h
#ifndef DEVICE_ARDUINO_H
#define DEVICE_ARDUINO_H

#include <WiFi101.h>
#include <WiFiSSLClient.h>  // Include SSL client
#include <ArduinoJson.h>
#include <vector>
#include "Devices.h" // Include base class

class DeviceArduino : public DeviceBase {
private:
    // Server details
    String server_url;
    char* deviceId;
    
    // API endpoints
    String connect_endpoint;
    String commands_endpoint;
    String data_endpoint;
    
    StaticJsonDocument<1024> doc;
    
    WiFiSSLClient sslClient;  // Changed to SSL client
    bool isConnected = false;
    
    // Vector to store pending commands
    std::vector<Command> pendingCommands;

public:
    // Constructor
    DeviceArduino(char* devId);
    
    // Destructor
    virtual ~DeviceArduino();
    
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

#endif // DEVICE_ARDUINO_H