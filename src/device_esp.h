// FILE: src/device_esp.h (partial fix)
#ifndef DEVICE_ESP_H
#define DEVICE_ESP_H

#include <Arduino.h>

#if defined(ESP32)
#include <WiFi.h> 
#include <HTTPClient.h> 


#include <ArduinoJson.h>
#include "MDevice.h"

class DeviceESP32 : public DeviceBase {
private:
    StaticJsonDocument<1024> doc;
    HTTPClient httpClient;
    String output;  // Changed from char[] to String to fix type issues
    String deviceId;
    String server_url;
    String connect_endpoint;
    String commands_endpoint;
    String data_endpoint;
    bool isConnected = false;
    std::vector<Command> pendingCommands;

public:
    DeviceESP32(const char* devId);
    ~DeviceESP32();
    
    virtual void initialize() override;
    virtual bool sendData(String type, String name, String component, int status) override;
    virtual bool sendData(String type, String name, String component, String status) override;
    virtual bool sendData(String type, String name, String component, int status, JsonArray dataArray) override;
    virtual void uninitialize() override;
    virtual bool connectWiFi(const char* ssid, const char* password) override;
    virtual bool sendDeviceConnect() override;
    virtual void checkForCommands() override;
    virtual bool getConnectionStatus() const override;
    virtual std::vector<Command>& getPendingCommands() override;
};
#endif
#endif // DEVICE_ESP_H