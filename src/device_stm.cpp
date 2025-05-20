// FILE: src/device_stm.cpp
#include "device_stm.h"

#if defined(ARDUINO_ARCH_STM32) || defined(STM32_CORE_VERSION)
// Constructor - fixed initialization
DeviceSTM32::DeviceSTM32(const char* devId) 
:httpClient(wifiClient, "devica.membeddedtechlab.com", 443)
 {
    deviceId = String(devId);
    server_url = "https://devica.membeddedtechlab.com";
    connect_endpoint = "/api/device-connect";
    commands_endpoint = "/api/device-commands/" + deviceId;
    data_endpoint = "/api/device-data";
}

// Destructor
DeviceSTM32::~DeviceSTM32() {
    uninitialize();
}

void DeviceSTM32::initialize() {
    if (WiFi.status() != WL_CONNECTED) return;
    
    // WiFi is connected, nothing else to initialize at this point
    // The HTTP client will be initialized for each request
}

bool DeviceSTM32::sendData(String type, String name, String component, int status) {
    if (WiFi.status() != WL_CONNECTED) return false;
    
    doc.clear();
    doc["type"] = type;
    doc["deviceId"] = deviceId;
    doc["name"] = name;
    doc["component"] = component;
    doc["status"] = status;
    
    output = "";  // Clear the string
    serializeJson(doc, output);
    
    httpClient.beginSSL("devica.membeddedtechlab.com", 443);
    httpClient.addHeader("Content-Type", "application/json");
    int httpResponseCode = httpClient.post(data_endpoint, "application/json", output);
    
    if (httpResponseCode <= 0) {
        Serial.printf("Error sending data for %s: %d\n", name.c_str(), httpResponseCode);
        return false;
    }

    return true;
    
}

bool DeviceSTM32::sendData(String type, String name, String component, long status) {
    if (WiFi.status() != WL_CONNECTED) return false;
    
    doc.clear();
    doc["type"] = type;
    doc["deviceId"] = deviceId;
    doc["name"] = name;
    doc["component"] = component;
    doc["status"] = status;
    
    output = "";  // Clear the string
    serializeJson(doc, output);
    
    httpClient.beginSSL("devica.membeddedtechlab.com", 443);
    httpClient.addHeader("Content-Type", "application/json");
    int httpResponseCode = httpClient.post(data_endpoint, "application/json", output);
    
    if (httpResponseCode <= 0) {
        Serial.printf("Error sending data for %s: %d\n", name.c_str(), httpResponseCode);
        return false;
    }

    return true;
    
}

bool DeviceSTM32::sendData(String type, String name, String component, double status) {
    if (WiFi.status() != WL_CONNECTED) return false;
    
    doc.clear();
    doc["type"] = type;
    doc["deviceId"] = deviceId;
    doc["name"] = name;
    doc["component"] = component;
    doc["status"] = status;
    
    output = "";  // Clear the string
    serializeJson(doc, output);
    
    httpClient.beginSSL("devica.membeddedtechlab.com", 443);
    httpClient.addHeader("Content-Type", "application/json");
    int httpResponseCode = httpClient.post(data_endpoint, "application/json", output);
    
    if (httpResponseCode <= 0) {
        Serial.printf("Error sending data for %s: %d\n", name.c_str(), httpResponseCode);
        return false;
    }

    return true;
    
}

bool DeviceSTM32::sendData(String type, String name, String component, String status) {
    
    if (WiFi.status() != WL_CONNECTED) return false;
    
    doc.clear();
    doc["type"] = type;
    doc["deviceId"] = deviceId;
    doc["name"] = name;
    doc["component"] = component;
    doc["status"] = status;
    
    output = "";  // Clear the string
    serializeJson(doc, output);
    
    httpClient.beginSSL("devica.membeddedtechlab.com", 443);
    httpClient.addHeader("Content-Type", "application/json");
    int httpResponseCode = httpClient.post(data_endpoint, "application/json", output);
    
    if (httpResponseCode <= 0) {
        Serial.printf("Error sending data for %s: %d\n", name.c_str(), httpResponseCode);
        return false;
    }

    return true;
  
}

bool DeviceSTM32::sendData(String type, String name, String component, int status, JsonArray dataArray) {
 
    if (WiFi.status() != WL_CONNECTED) return false;
    
    doc.clear();
    doc["type"] = type;
    doc["deviceId"] = deviceId;
    doc["name"] = name;
    doc["component"] = component;
    doc["status"] = status;
    doc["data"] = dataArray;
    
    output = "";  // Clear the string
    serializeJson(doc, output);
    
    httpClient.beginSSL("devica.membeddedtechlab.com", 443);
    httpClient.addHeader("Content-Type", "application/json");
    int httpResponseCode = httpClient.post(data_endpoint, "application/json", output);
    
    if (httpResponseCode <= 0) {
        Serial.printf("Error sending data for %s: %d\n", name.c_str(), httpResponseCode);
        return false;
    }

    return true;
    
}

void DeviceSTM32::uninitialize() {
    httpClient.stop();
}

bool DeviceSTM32::connectWiFi(const char* ssid, const char* password) {
    WiFi.begin(ssid, password);
    Serial.print("Connecting to WiFi");
    
    // Set a timeout for WiFi connection attempts
    int timeout = 0;
    while (WiFi.status() != WL_CONNECTED && timeout < 20) { // 10 second timeout
        delay(500);
        Serial.print(".");
        timeout++;
    }
    
    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\nConnected to WiFi");
        Serial.printf("IP Address: %s\n", WiFi.localIP().toString().c_str());
        return sendDeviceConnect();
    } else {
        Serial.println("\nFailed to connect to WiFi. Will retry in next loop.");
        return false;
    }
}

bool DeviceSTM32::sendDeviceConnect() {
    if (WiFi.status() == WL_CONNECTED) {
        // Prepare the JSON payload
        doc.clear();  // Clear the existing document
        doc["type"] = "deviceConnect";
        doc["deviceId"] = deviceId;
        
        output = "";  // Clear the string
        serializeJson(doc, output);

        // Send HTTP POST request
        httpClient.beginSSL("devica.membeddedtechlab.com", 443);
        httpClient.addHeader("Content-Type", "application/json");
        int httpResponseCode = httpClient.post(connect_endpoint, "application/json", output);
        
        if (httpResponseCode > 0) {
            Serial.print("Device connect HTTP Response code: ");
            Serial.println(httpResponseCode);
            isConnected = true;
        } else {
            Serial.print("Device connect error code: ");
            Serial.println(httpResponseCode);
            isConnected = false;
        }
        
        httpClient.stop();
        return isConnected;
    }
    return false;
}

void DeviceSTM32::checkForCommands() {
    if (WiFi.status() == WL_CONNECTED) {
        // Create a new HTTP request for commands
        httpClient.beginSSL("devica.membeddedtechlab.com", 443);
        int httpResponseCode = httpClient.get(commands_endpoint);
        
        if (httpResponseCode > 0) {
            String payload = httpClient.responseBody();
            
            StaticJsonDocument<512> commandDoc;
            DeserializationError error = deserializeJson(commandDoc, payload);
            
            if (!error) {
                // Clear previous pending commands
                pendingCommands.clear();
                
                // Check if "commands" array exists in the JSON
                if (commandDoc.containsKey("commands") && commandDoc["commands"].is<JsonArray>()) {
                    JsonArray commands = commandDoc["commands"].as<JsonArray>();
                    
                    for (JsonVariant commandObj : commands) {
                        // Create a new Command struct and add to pending commands
                        Command newCommand;
                        newCommand.name = commandObj["name"].as<String>();
                        newCommand.status = commandObj["status"].as<int>();
                        
                        pendingCommands.push_back(newCommand);
                        
                        // Debug print
                        Serial.println("Received command: " + newCommand.name + 
                                     ", Status: " + String(newCommand.status));
                    }
                }
            } else {
                Serial.print("JSON deserialization failed: ");
                Serial.println(error.c_str());
            }
        }
        
        httpClient.stop();
    }
}

bool DeviceSTM32::getConnectionStatus() const {
    return isConnected;
}

std::vector<Command>& DeviceSTM32::getPendingCommands() {
    return pendingCommands;
}
#endif