#include "device_stm.h"

// Constructor
Device::Device(const char* devId) {
    deviceId = String(devId);
    server_url = "https://devica.membeddedtechlab.com";
    connect_endpoint = server_url + "/api/device-connect";
    commands_endpoint = server_url + "/api/device-commands/" + deviceId;
    data_endpoint = server_url + "/api/device-data";
}

// Destructor (can be empty for now)
Device::~Device() {}

void Device::initialize() {
    // Initialize WiFi module or any additional setup
    WiFi.mode(WIFI_STA);
    delay(100);
}

// Private helper method for sending HTTP requests
bool Device::sendHttpRequest(const String& method, const String& endpoint, const String& payload) {
    // Extract host and path from full URL
    String host, path;
    int protocolIndex = endpoint.indexOf("://");
    int pathIndex = endpoint.indexOf("/", protocolIndex + 3);
    
    if (protocolIndex != -1 && pathIndex != -1) {
        host = endpoint.substring(protocolIndex + 3, pathIndex);
        path = endpoint.substring(pathIndex);
    } else {
        Serial.println("Invalid endpoint URL");
        return false;
    }

    // Attempt to connect to server
    if (!client.connect(host.c_str(), 8000)) {
        Serial.println("Connection failed");
        return false;
    }

    // Construct HTTP request
    client.print(method + " " + path + " HTTP/1.1\r\n");
    client.print("Host: " + host + "\r\n");
    
    if (!payload.isEmpty()) {
        client.println("Content-Type: application/json");
        client.print("Content-Length: ");
        client.println(payload.length());
    }
    
    client.println("Connection: close");
    client.println();
    
    if (!payload.isEmpty()) {
        client.println(payload);
    }

    // Wait for response with timeout
    unsigned long startTime = millis();
    while (!client.available()) {
        if (millis() - startTime > 5000) {
            Serial.println("Request timeout");
            client.stop();
            return false;
        }
    }

    // Skip HTTP headers
    while (client.available()) {
        String line = client.readStringUntil('\n');
        if (line == "\r") break;
    }

    return true;
}

bool Device::sendData(String type, String name, String component, int status) {
    if (WiFi.status() != WL_CONNECTED) return false;

    // Prepare JSON payload
    doc.clear();
    doc["type"] = type;
    doc["deviceId"] = deviceId;
    doc["name"] = name;
    doc["component"] = component;
    doc["status"] = status;

    // Serialize JSON
    memset(output, 0, sizeof(output));
    serializeJson(doc, output, sizeof(output));

    // Send HTTP POST request
    return sendHttpRequest("POST", data_endpoint, output);
}

bool Device::sendData(String type, String name, String component, String status) {
    if (WiFi.status() != WL_CONNECTED) return false;

    // Prepare JSON payload
    doc.clear();
    doc["type"] = type;
    doc["deviceId"] = deviceId;
    doc["name"] = name;
    doc["component"] = component;
    doc["status"] = status;

    // Serialize JSON
    memset(output, 0, sizeof(output));
    serializeJson(doc, output, sizeof(output));

    // Send HTTP POST request
    return sendHttpRequest("POST", data_endpoint, output);
}

bool Device::sendData(String type, String name, String component, int status, JsonArray dataArray) {
    if (WiFi.status() != WL_CONNECTED) return false;

    // Prepare JSON payload
    doc.clear();
    doc["type"] = type;
    doc["deviceId"] = deviceId;
    doc["name"] = name;
    doc["component"] = component;
    doc["status"] = status;
    doc["data"] = dataArray;

    // Serialize JSON
    memset(output, 0, sizeof(output));
    serializeJson(doc, output, sizeof(output));

    // Send HTTP POST request
    return sendHttpRequest("POST", data_endpoint, output);
}

bool Device::connectWiFi(const char* ssid, const char* password) {
    // Begin WiFi connection
    WiFi.begin(ssid, password);
    
    Serial.print("Connecting to WiFi");
    
    // Wait for connection with timeout
    unsigned long startTime = millis();
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
        
        // 10-second timeout
        if (millis() - startTime > 10000) {
            Serial.println("\nWiFi connection failed");
            return false;
        }
    }
    
    Serial.println("\nConnected to WiFi");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
    
    // Send device connection after successful WiFi connection
    return sendDeviceConnect();
}

bool Device::sendDeviceConnect() {
    if (WiFi.status() != WL_CONNECTED) return false;

    // Prepare device connect payload
    doc.clear();
    doc["type"] = "deviceConnect";
    doc["deviceId"] = deviceId;

    // Serialize JSON
    memset(output, 0, sizeof(output));
    serializeJson(doc, output, sizeof(output));

    // Send HTTP POST request
    bool result = sendHttpRequest("POST", connect_endpoint, output);
    
    // Update connection status
    isConnected = result;
    return result;
}

void Device::checkForCommands() {
    if (WiFi.status() != WL_CONNECTED) return;

    // Send GET request to commands endpoint
    if (sendHttpRequest("GET", commands_endpoint)) {
        // Read response payload
        String payload = client.readString();
        client.stop();

        // Parse JSON
        DeserializationError error = deserializeJson(doc, payload);
        if (!error) {
            // Clear previous pending commands
            pendingCommands.clear();

            // Check for commands array
            if (doc.containsKey("commands") && doc["commands"].is<JsonArray>()) {
                JsonArray commands = doc["commands"].as<JsonArray>();

                for (JsonVariant commandObj : commands) {
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
            Serial.print("JSON parsing failed: ");
            Serial.println(error.c_str());
        }
    }
}

void Device::uninitialize() {
    // Disconnect WiFi
    WiFi.disconnect();
}

bool Device::getConnectionStatus() const {
    return isConnected;
}

std::vector<Command>& Device::getPendingCommands() {
    return pendingCommands;
}