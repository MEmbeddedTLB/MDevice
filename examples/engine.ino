//Get a custome code at https://device.membeddedtechlab.com/
#include ""  // Your custom Device class header


const char* ssid = "";       
const char* password = "";    

// Create global device instance
Device device(/*id*/);

// Connection timing constants
unsigned long lastCommandCheckTime = 0;
unsigned long lastDataSendTime = 0;
const unsigned long COMMAND_CHECK_INTERVAL = 2000; // Check for commands every 2 seconds
const unsigned long DATA_SEND_INTERVAL = 5000;    // Send data every 5 seconds

void setup() {
  Serial.begin(115200);
  device.connectWiFi(ssid, password);
}

void loop() {
  unsigned long currentMillis = millis();
  
  // Check WiFi connection
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi connection lost. Reconnecting...");
    device.connectWiFi(ssid, password);
    delay(1000); // Short delay before continuing
    return;
  }
  
  // Check for commands
  if (currentMillis - lastCommandCheckTime >= COMMAND_CHECK_INTERVAL) {
    lastCommandCheckTime = currentMillis;
    device.checkForCommands();
  }

  // Send device data
  if (device.getConnectionStatus() && currentMillis - lastDataSendTime >= DATA_SEND_INTERVAL) {
    lastDataSendTime = currentMillis;
    
    device.initialize();
    
  
    device.sendData("deviceData", "value0", "LabelToggle", 100);
    }

    device.uninitialize();
  }

  // Process pending commands if any
  std::vector<Command>& pendingCommands = device.getPendingCommands();
  if (!pendingCommands.empty()) {
    for (const auto& command : pendingCommands) {
      // Add your command processing logic here
      // Serial.println("Processing command: " + command.name);
    }
    
    pendingCommands.clear();
  }
  
  // Small delay to prevent CPU hogging
  delay(1000);
}
