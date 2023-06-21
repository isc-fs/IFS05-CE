#include "Wire.h"
#include "mcp_can.h"



// Set up CAN bus
MCP_CAN can(10);

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
}

void loop() {
  // Check for incoming CAN message
  unsigned long ID = 0;
  unsigned char len = 0;
  unsigned char rxBuf[8];
  if (can.checkReceive() == CAN_MSGAVAIL) {
    can.readMsgBuf(&ID, &len, rxBuf);
    if (ID == 0x123 && len == 24*sizeof(float)) {
      // Calculate mean, minimum, and maximum temperatures
      float temps[24];
      memcpy(&temps, rxBuf, len);
      float sum = 0;
      float minTemp = temps[0];
      float maxTemp = temps[0];
      for (int i = 0; i < 24; i++) {
        sum += temps[i];
        if (temps[i] < minTemp) {
          minTemp = temps[i];
        }
        if (temps[i] > maxTemp) {
          maxTemp = temps[i];
        }
      }
      float meanTemp = sum / 24;

      // Display temperatures
      Serial.print("Mean temperature: ");
      Serial.print(meanTemp);
      Serial.print(" C, Minimum temperature: ");
      Serial.print(minTemp);
      Serial.print(" C, Maximum temperature: ");
      Serial.print(maxTemp);
      Serial.println(" C");
    }
  }
}
