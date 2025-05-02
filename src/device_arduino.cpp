// FILE: src/device_arduino.cpp
#include "device_arduino.h"

#if defined(ARDUINO_ARCH_AVR) || defined(ARDUINO_ARCH_SAMD) || defined(ARDUINO_ARCH_SAM) || \
      defined(ARDUINO_ARCH_MBED) || defined(ARDUINO_ARCH_RP2040) || defined(ARDUINO_ARCH_NRF52) || \
      defined(ARDUINO) 
// Constructor
DeviceArduino::DeviceArduino(const char* devId) {
    deviceId = String(devId);
    server_url = "https://devica.membeddedtechlab.com";
    connect_endpoint = server_url + "/api/device-connect";
    commands_endpoint = server_url + "/api/device-commands/" + deviceId;
    data_endpoint = server_url + "/api/device-data";
}

// Destructor
DeviceArduino::~DeviceArduino() {
    client.stop();
}

void DeviceArduino::initialize() {
    if (WiFi.status() != WL_CONNECTED) return;
}

// Helper method for HTTP POST requests
bool DeviceArduino::sendHttpRequest(String endpoint, String jsonPayload, String& response) {
    // Extract domain and path from endpoint
    String domain = server_url;
    domain.replace("https://", "");
    
    String path = endpoint;
    path.replace(server_url, "");
    
    if (client.connect(domain.c_str(), 443)) {
        // Send HTTP POST request
        client.print("POST ");
        client.print(path);
        client.println(" HTTP/1.1");
        client.print("Host: ");
        client.println(domain);
        client.println("Connection: close");
        client.println("Content-Type: application/json");
        client.print("Content-Length: ");
        client.println(jsonPayload.length());
        client.println();
        client.println(jsonPayload);
        
        // Wait for response
        unsigned long timeout = millis();
        while (client.connected() && !client.available()) {
            if (millis() - timeout > 10000) {
                Serial.println("HTTP request timeout");
                client.stop();
                return false;
            }
        }
        
        // Process response
        response = "";
        while (client.available()) {
            char c = client.read();
            response += c;
        }
        
        client.stop();
        
        // Check if response contains "200 OK"
        if (response.indexOf("200 OK") > 0) {
            return true;
        }
    }
    
    return false;
}

// Helper method for HTTP GET requests
bool DeviceArduino::performHttpGet(String endpoint, String& response) {
    // Extract domain and path from endpoint
    String domain = server_url;
    domain.replace("https://", "");
    
    String path = endpoint;
    path.replace(server_url, "");
    
    if (client.connect(domain.c_str(), 443)) {
        // Send HTTP GET request
        client.print("GET ");
        client.print(path);
        client.println(" HTTP/1.1");
        client.print("Host: ");
        client.println(domain);
        client.println("Connection: close");
        client.println();
        
        // Wait for response
        unsigned long timeout = millis();
        while (client.connected() && !client.available()) {
            if (millis() - timeout > 10000) {
                Serial.println("HTTP request timeout");
                client.stop();
                return false;
            }
        }
        
        // Process response
        response = "";
        while (client.available()) {
            char c = client.read();
            response += c;
        }
        
        client.stop();
        
        // Check if response contains "200 OK"
        if (response.indexOf("200 OK") > 0) {
            return true;
        }
    }
    
    return false;
}

bool DeviceArduino::sendData(String type, String name, String component, int status) {
    doc.clear();
    doc["type"] = type;
    doc["deviceId"] = deviceId;
    doc["name"] = name;
    doc["component"] = component;
    doc["status"] = status;
    
    output = "";  // Clear the string
    serializeJson(doc, output);
    
    String response;
    bool success = sendHttpRequest(data_endpoint, output, response);
    
    if (!success) {
        Serial.printf("Error sending data for %s\n", name.c_str());
        return false;
    }
    
    return true;
}

bool DeviceArduino::sendData(String type, String name, String component, String status) {
    doc.clear();
    doc["type"] = type;
    doc["deviceId"] = deviceId;
    doc["name"] = name;
    doc["component"] = component;
    doc["status"] = status;
    
    output = "";  // Clear the string
    serializeJson(doc, output);
    
    String response;
    bool success = sendHttpRequest(data_endpoint, output, response);
    
    if (!success) {
        Serial.printf("Error sending data for %s\n", name.c_str());
        return false;
    }
    
    return true;
}

bool DeviceArduino::sendData(String type, String name, String component, int status, JsonArray dataArray) {
    doc.clear();
    doc["type"] = type;
    doc["deviceId"] = deviceId;
    doc["name"] = name;
    doc["component"] = component;
    doc["status"] = status;
    doc["data"] = dataArray;
    
    output = "";  // Clear the string
    serializeJson(doc, output);
    
    String response;
    bool success = sendHttpRequest(data_endpoint, output, response);
    
    if (!success) {
        Serial.printf("Error sending data for %s\n", name.c_str());
        return false;
    }
    
    return true;
}

void DeviceArduino::uninitialize() {
    client.stop();
}

bool DeviceArduino::connectWiFi(const char* ssid, const char* password) {
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
        Serial.print("IP Address: ");
        Serial.println(WiFi.localIP());
        return sendDeviceConnect();
    } else {
        Serial.println("\nFailed to connect to WiFi. Will retry in next loop.");
        return false;
    }
}

bool DeviceArduino::sendDeviceConnect() {
    if (WiFi.status() == WL_CONNECTED) {
        // Prepare the JSON payload
        doc.clear();
        doc["type"] = "deviceConnect";
        doc["deviceId"] = deviceId;
        
        output = "";
        serializeJson(doc, output);
        
        String response;
        bool success = sendHttpRequest(connect_endpoint, output, response);
        
        if (success) {
            Serial.println("Device connected successfully");
            isConnected = true;
        } else {
            Serial.println("Device connection failed");
            isConnected = false;
        }
        
        return isConnected;
    }
    return false;
}

void DeviceArduino::checkForCommands() {
    if (WiFi.status() == WL_CONNECTED) {
        String response;
        bool success = performHttpGet(commands_endpoint, response);
        
        if (success) {
            // Extract JSON part from HTTP response
            int jsonStart = response.indexOf("{");
            if (jsonStart >= 0) {
                String jsonPayload = response.substring(jsonStart);
                
                StaticJsonDocument<512> commandDoc;
                DeserializationError error = deserializeJson(commandDoc, jsonPayload);
                
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
        }
    }
}

bool DeviceArduino::getConnectionStatus() const {
    return isConnected;
}

std::vector<Command>& DeviceArduino::getPendingCommands() {
    return pendingCommands;
}
#endif