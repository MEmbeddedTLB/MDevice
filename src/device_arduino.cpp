#include "device_arduino.h"

// Constructor
DeviceArduino::DeviceArduino(char* devId) 
    : doc(1024) {
    deviceId = devId;
    server_url = "devica.membeddedtechlab.com";  // Domain without protocol
    connect_endpoint = "/api/device-connect";
    commands_endpoint = "/api/device-commands/";
    data_endpoint = "/api/device-data";
}

// Destructor
DeviceArduino::~DeviceArduino() {
    sslClient.stop();
}

void DeviceArduino::initialize() {
    // Nothing specific to initialize for the Arduino client
}

bool DeviceArduino::sendData(String type, String name, String component, int status) {
    if (WiFi.status() != WL_CONNECTED) return false;

    doc.clear();
    doc["type"] = type;
    doc["deviceId"] = deviceId;
    doc["name"] = name;
    doc["component"] = component;
    doc["status"] = status;

    String jsonStr;
    serializeJson(doc, jsonStr);

    // Connect and send data using SSL
    if (sslClient.connect(server_url.c_str(), 443)) {
        sslClient.print("POST ");
        sslClient.print(data_endpoint);
        sslClient.println(" HTTP/1.1");
        sslClient.print("Host: ");
        sslClient.println(server_url);
        sslClient.println("Content-Type: application/json");
        sslClient.println("Connection: close");
        sslClient.print("Content-Length: ");
        sslClient.println(jsonStr.length());
        sslClient.println();
        sslClient.println(jsonStr);

        // Wait for response
        unsigned long timeout = millis();
        while (sslClient.available() == 0) {
            if (millis() - timeout > 5000) {
                sslClient.stop();
                Serial.println("Client Timeout!");
                return false;
            }
        }

        // Read and discard response
        while (sslClient.available()) {
            sslClient.read();
        }

        sslClient.stop();
        return true;
    }
    return false;
}

bool DeviceArduino::sendData(String type, String name, String component, String status) {
    if (WiFi.status() != WL_CONNECTED) return false;

    doc.clear();
    doc["type"] = type;
    doc["deviceId"] = deviceId;
    doc["name"] = name;
    doc["component"] = component;
    doc["status"] = status;

    String jsonStr;
    serializeJson(doc, jsonStr);

    if (sslClient.connect(server_url.c_str(), 443)) {
        sslClient.print("POST ");
        sslClient.print(data_endpoint);
        sslClient.println(" HTTP/1.1");
        sslClient.print("Host: ");
        sslClient.println(server_url);
        sslClient.println("Content-Type: application/json");
        sslClient.println("Connection: close");
        sslClient.print("Content-Length: ");
        sslClient.println(jsonStr.length());
        sslClient.println();
        sslClient.println(jsonStr);

        unsigned long timeout = millis();
        while (sslClient.available() == 0) {
            if (millis() - timeout > 5000) {
                sslClient.stop();
                Serial.println("Client Timeout!");
                return false;
            }
        }

        // Read and discard response
        while (sslClient.available()) {
            sslClient.read();
        }

        sslClient.stop();
        return true;
    }
    return false;
}

bool DeviceArduino::sendData(String type, String name, String component, int status, JsonArray dataArray) {
    if (WiFi.status() != WL_CONNECTED) return false;

    doc.clear();
    doc["type"] = type;
    doc["deviceId"] = deviceId;
    doc["name"] = name;
    doc["component"] = component;
    doc["status"] = status;
    doc["data"] = dataArray;

    String jsonStr;
    serializeJson(doc, jsonStr);

    if (sslClient.connect(server_url.c_str(), 443)) {
        sslClient.print("POST ");
        sslClient.print(data_endpoint);
        sslClient.println(" HTTP/1.1");
        sslClient.print("Host: ");
        sslClient.println(server_url);
        sslClient.println("Content-Type: application/json");
        sslClient.println("Connection: close");
        sslClient.print("Content-Length: ");
        sslClient.println(jsonStr.length());
        sslClient.println();
        sslClient.println(jsonStr);

        unsigned long timeout = millis();
        while (sslClient.available() == 0) {
            if (millis() - timeout > 5000) {
                sslClient.stop();
                Serial.println("Client Timeout!");
                return false;
            }
        }

        // Read and discard response
        while (sslClient.available()) {
            sslClient.read();
        }

        sslClient.stop();
        return true;
    }
    return false;
}

bool DeviceArduino::connectWiFi(const char* ssid, const char* password) {
    // Initialize WiFi
    WiFi.begin(ssid, password);
    Serial.print("Connecting to WiFi");

    // Wait for connection with timeout
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
    if (WiFi.status() == WL_CONNECTED && deviceId != nullptr) {
        doc.clear();
        doc["type"] = "deviceConnect";
        doc["deviceId"] = deviceId;

        String jsonStr;
        serializeJson(doc, jsonStr);

        if (sslClient.connect(server_url.c_str(), 443)) {
            sslClient.print("POST ");
            sslClient.print(connect_endpoint);
            sslClient.println(" HTTP/1.1");
            sslClient.print("Host: ");
            sslClient.println(server_url);
            sslClient.println("Content-Type: application/json");
            sslClient.println("Connection: close");
            sslClient.print("Content-Length: ");
            sslClient.println(jsonStr.length());
            sslClient.println();
            sslClient.println(jsonStr);

            unsigned long timeout = millis();
            while (sslClient.available() == 0) {
                if (millis() - timeout > 5000) {
                    sslClient.stop();
                    Serial.println("Device Connect Timeout!");
                    isConnected = false;
                    return false;
                }
            }

            // Process the response
            String response = "";
            while (sslClient.available()) {
                char c = sslClient.read();
                response += c;
            }
            
            sslClient.stop();
            
            // Check if response contains success code
            if (response.indexOf("200 OK") > 0) {
                isConnected = true;
                return true;
            } else {
                isConnected = false;
                return false;
            }
        }
    }
    return false;
}

void DeviceArduino::checkForCommands() {
    if (WiFi.status() == WL_CONNECTED && deviceId != nullptr) {
        String fullEndpoint = commands_endpoint + deviceId;
        
        if (sslClient.connect(server_url.c_str(), 443)) {
            sslClient.print("GET ");
            sslClient.print(fullEndpoint);
            sslClient.println(" HTTP/1.1");
            sslClient.print("Host: ");
            sslClient.println(server_url);
            sslClient.println("Connection: close");
            sslClient.println();

            // Wait for response
            unsigned long timeout = millis();
            while (sslClient.available() == 0) {
                if (millis() - timeout > 5000) {
                    sslClient.stop();
                    Serial.println("Command Check Timeout!");
                    return;
                }
            }

            // Skip HTTP headers
            String line = "";
            while (sslClient.available()) {
                line = sslClient.readStringUntil('\n');
                if (line == "\r") break;
            }

            // Read payload
            String payload = "";
            while (sslClient.available()) {
                char c = sslClient.read();
                payload += c;
            }
            
            sslClient.stop();

            // Parse JSON
            doc.clear();
            DeserializationError error = deserializeJson(doc, payload);
            
            if (!error) {
                pendingCommands.clear();

                if (doc.containsKey("commands") && doc["commands"].is<JsonArray>()) {
                    JsonArray commands = doc["commands"].as<JsonArray>();

                    for (JsonVariant commandObj : commands) {
                        Command newCommand;
                        newCommand.name = commandObj["name"].as<String>();
                        newCommand.status = commandObj["status"].as<int>();

                        pendingCommands.push_back(newCommand);
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

void DeviceArduino::uninitialize() {
    sslClient.stop();
}

bool DeviceArduino::getConnectionStatus() const {
    return isConnected;
}

std::vector<Command>& DeviceArduino::getPendingCommands() {
    return pendingCommands;
}