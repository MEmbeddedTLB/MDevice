// FILE: src/device_stm.h
#ifndef DEVICE_STM_H
#define DEVICE_STM_H


#if defined(ARDUINO_ARCH_STM32) || defined(STM32_CORE_VERSION)
    #include <Arduino.h>
  #include <WiFi.h>  
  #include <ArduinoHttpClient.h>


#include <ArduinoJson.h>
#include "MDevice.h"

class DeviceSTM32 : public DeviceBase {
private:
    StaticJsonDocument<1024> doc;
    
    WiFiClient wifiClient;
    HttpClient httpClient;
    
    String output;  // Using String for consistency
    String deviceId;
    String server_url;
    String connect_endpoint;
    String commands_endpoint;
    String data_endpoint;
    bool isConnected = false;
    std::vector<Command> pendingCommands;

public:
    DeviceSTM32(const char* devId);
    ~DeviceSTM32();
    
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
#endif // DEVICE_STM_H