#include <Wire.h>
#include <BH1750.h>
#include "LedLib.h"
#include "WiFi.h"
#include "Room.h"
#include <WebSocketsClient.h>

const int IDS[2] = { 3, 4 };
const String SecretKey = "rahasia";

TwoWire I2CBus1 = TwoWire(0);
TwoWire I2CBus2 = TwoWire(1);
const int udpPort = 4210;

IPAddress ServerIP;
volatile bool connectedToWS = false;
volatile bool EspState = false;

WebSocketsClient webSocket;
BH1750 lightMeter1;
BH1750 lightMeter2;
Room FirstRoom(4, 5, lightMeter1, IDS[0]);
Room SecondRoom(18, 19, lightMeter2, IDS[1]);

SemaphoreHandle_t sendSemaphore;

void wsTask(void* param) {
  while (true) {
    webSocket.loop();
    if (xSemaphoreTake(sendSemaphore, 0) == pdTRUE) {
      String msg = buildJsonMessage();
      webSocket.sendTXT(msg);
    }
    vTaskDelay(10);
  }
}

void setup() {
  Serial.begin(115200);
  I2CBus1.begin(21, 22);
  I2CBus2.begin(25, 26);

  delay(1000);
  scanI2C(I2CBus1, "I2CBus1");
  scanI2C(I2CBus2, "I2CBus2");

  WiFi.begin("fireDetector", "password123");
  Serial.print("Connecting to WiFi...");



  if (!lightMeter1.begin(BH1750::CONTINUOUS_HIGH_RES_MODE, 0x23, &I2CBus1)) {
    Serial.println("Sensor BH1 undetected!");
    FirstRoom.redLight.on();
    delay(1000);
  }

  if (!lightMeter2.begin(BH1750::CONTINUOUS_HIGH_RES_MODE, 0x23, &I2CBus2)) {
    SecondRoom.redLight.on();
    Serial.println("Sensor BH2 undetected!");
    delay(1000);
  }

  if (FirstRoom.redLight.isOn()) {
    FirstRoom.redLight.off();
  }

  if (SecondRoom.redLight.isOn()) {
    SecondRoom.redLight.off();
  }



  sendSemaphore = xSemaphoreCreateBinary();
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected!");

  webSocket.begin("192.168.10.1", 80, "/?token=" + SecretKey, "");
  webSocket.onEvent(webSocketEvent);
  webSocket.setReconnectInterval(5000);
  xTaskCreatePinnedToCore(wsTask, "WebSocket Task", 4096, NULL, 1, NULL, 1);
  Serial.println("Sensor siap!");
}

void loop() {
  if (EspState) {
    FirstRoom.loop();
    SecondRoom.loop();

    float lux1 = FirstRoom.getLux();
    float lux2 = SecondRoom.getLux();
    Serial.printf("Lux Sensor 1: %.2f | Lux Sensor 2: %.2f\n", lux1, lux2);
    if (connectedToWS) {
      xSemaphoreGive(sendSemaphore);
    }
  }

  delay(200);
}

void webSocketEvent(WStype_t type, uint8_t* payload, size_t length) {
  switch (type) {
    case WStype_CONNECTED:
      Serial.println("[WebSocket] Connected to server");
      connectedToWS = true;
      webSocket.sendTXT("status");
      break;
    case WStype_DISCONNECTED:
      Serial.println("[WebSocket] Disconnected");
      connectedToWS = false;
      break;
    case WStype_TEXT:
      Serial.printf("[WebSocket] Received text: %s\n", payload);
      if (String((char*)payload) == "off") {
        EspState = false;
      } else {
        EspState = true;
      }
      break;
  }
}

String buildJsonMessage() {
  return "{\"rooms\":[" + FirstRoom.toJson() + "," + SecondRoom.toJson() + "]}";
}



void scanI2C(TwoWire& wire, const char* label) {
  Serial.printf("Scanning %s...\n", label);
  wire.beginTransmission(0x23);
  if (wire.endTransmission() == 0) {
    Serial.printf("Found BH1750 at 0x23 on %s\n", label);
  } else {
    Serial.printf("BH1750 not found at 0x23 on %s\n", label);
  }
}
