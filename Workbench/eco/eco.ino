#include "mcp_can.h"
#include "setup_config.h"
#include <Wire.h>
#include <SPI.h>

#define CAN0CS 10 //43
#define CAN0INT 9//37
//#define CAN0CS 45
//#define CAN0INT 39
MCP_CAN CAN0(CAN0CS);

INT8U  len      = 8;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  pinMode(CAN0INT, INPUT);                                // Configuring pin for inputene.

  // init CAN1 bus
  if (CAN0.begin(MCP_ANY, CAN0Speed, MCP_8MHZ) == CAN_OK) {
    Serial.print("CAN0: Init OK!\r\n");
    Serial.println(CAN0INT);
    CAN0.setMode(MCP_NORMAL);
  } else Serial.print("CAN1: Init Fail!!!\r\n");

  SPI.setClockDivider(SPI_CLOCK_DIV2);                       // Set SPI to run at 8MHz (16MHz / 2 = 8 MHz)




}

void loop() {
  // put your main code here, to run repeatedly:z
  if (!digitalRead(CAN0INT)) {
    CAN0.readMsgBuf(&rxId, &len, rxBuf);
    Serial.print("0x");
    Serial.print(rxId, HEX);
    Serial.print(",");
    Serial.print(len);
    Serial.print(",");
    Serial.print(rxBuf[7]);
    Serial.print(",");
    Serial.print(rxBuf[6]);
    Serial.print(",");
    Serial.print(rxBuf[5]);
    Serial.print(",");
    Serial.print(rxBuf[4]);
    Serial.print(",");
    Serial.print(rxBuf[3]);
    Serial.print(",");
    Serial.print(rxBuf[2]);
    Serial.print(",");
    Serial.print(rxBuf[1]);
    Serial.print(",");
    Serial.println(rxBuf[0]);
    delay(100);
   
    
  }else{
    Serial.println("Error");
  }
  
}
