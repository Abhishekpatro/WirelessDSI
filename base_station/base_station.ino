// File -> Preferences -> Additional Boards Manager URLs: https://dl.espressif.com/dl/package_esp32_index.json
// Tools -> Board -> Boards Manager: search for "esp32" and install "ESP32 by Espressif Systems"
// Tools -> Board -> ESP32 Arduino -> select the board that matches your ESP32 development board(ESP32 Dev Module)
// (if you don't see it listed, make sure you have the latest version of the ESP32 board package installed)


#include <WiFi.h>
#include <WebSocketsServer.h>

// Replace with your network credentials
const char* ssid = "WirelessDSIMobileUnit";
const char* password = "blah1234";


//const char* ssid = "NCAN WiFi";
//const char* password = "aHYca5\\$qr!";

// Create a WebSocket server on port 80
WebSocketsServer webSocket(80);

// Define the red LED pin
const int RED_LED_PIN = 2; // You can change this to the appropriate GPIO pin for your ESP-32

void handle_websocket_data(uint8_t *payload, size_t length) {
  Serial.write(payload, length);
  Serial.flush();
}

 
void handle_serial_data() {
    if (Serial.available()) {
      uint8_t n = Serial.read();
      uint8_t buffer[n + 1];
      buffer[0] = n;
      Serial.readBytes(&buffer[1], n);
      //Serial.print("forwarding "); Serial.print(n+1); Serial.println(" bytes"); Serial.flush();
      webSocket.broadcastBIN(buffer, n + 1);
    }
}

void setup() {
  // Set up the red LED pin
  pinMode(RED_LED_PIN, OUTPUT);
  digitalWrite(RED_LED_PIN, LOW);

  // Start the serial connection
  Serial.begin(921600);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
 // IPAddress myIP(192, 168, 4, 13);
  IPAddress gatewayIP(192, 168, 4, 1);
  IPAddress subnet(255, 255, 0, 0);
  //if (!WiFi.config(myIP, gatewayIP, subnet)) Serial.println("Failed to configure WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
   // Serial.println("Connecting to WiFi...");
   // Serial.println(WiFi.localIP());
  }

  // Print a message indicating that the Wi-Fi connection was successful
  Serial.println("Connected to Wi-Fi");

  // Start the WebSocket server
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
  
  // Turn on the red LED to indicate that the WebSocket server is running
  digitalWrite(RED_LED_PIN, HIGH);
}

void loop() {
  webSocket.loop();
  handle_serial_data();
  
}

// Function to handle WebSocket events
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  switch (type) {
    case WStype_DISCONNECTED:
      Serial.printf("[%u] Disconnected!\n", num);
      break;
      
    case WStype_CONNECTED:
      {
        IPAddress ip = webSocket.remoteIP(num);
        Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);
      }
      break;

    case WStype_TEXT:
      Serial.printf("Received tetx message: %s\n", payload);
      break;

    case WStype_BIN:
       handle_websocket_data(payload, length);
      break;
  }
}


