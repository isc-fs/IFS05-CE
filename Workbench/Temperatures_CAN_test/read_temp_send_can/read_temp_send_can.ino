
#include "Wire.h"
#include "mcp_can.h"
#include "OneWire.h"
#include "DallasTemperature.h"


// Set up OneWire and DallasTemperature libraries
OneWire oneWire(2);
DallasTemperature sensors(&oneWire);

// Set up CAN bus
MCP_CAN can(10);
unsigned char txBuf[8];
unsigned char rxBuf[8];

void setup() {
  // Start serial communication
  Serial.begin(9600);
  while (!Serial);

  // Initialize CAN bus
  if (can.begin(MCP_ANY, CAN_1000KBPS, MCP_16MHZ) == CAN_OK) {
    Serial.println("CAN bus initialized");
  } else {
    Serial.println("Error initializing CAN bus");
  }

  // Set up temperature sensors
  sensors.begin();
}

void loop() {
  // Read temperatures from sensors
  sensors.requestTemperatures();
  float temps[24];
  for (int i = 0; i < 24; i++) {
    temps[i] = sensors.getTempCByIndex(i);
  }

  // Pack temperatures into CAN message and send
  unsigned long canId = 0x123;
  int canDataLen = sizeof(temps);
  memcpy(txBuf, &temps, canDataLen);
  can.sendMsgBuf(canId, 0, canDataLen, txBuf);

  // Wait for next reading
  delay(1000);
}
