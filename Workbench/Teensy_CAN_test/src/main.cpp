#include <Arduino.h>
#include "FlexCAN_T4.h"


FlexCAN_T4<CAN1, RX_SIZE_128, TX_SIZE_64> can1;
CAN_message_t msg;

void setup()
{
  Serial.begin(115200);

  can1.begin();
  can1.setBaudRate(250000);
  //can1.setMBFilter(ACCEPT_ALL);
  //can1.distribute();
  //can1.mailboxStatus();

}

void loop()
{
  // put your main code here, to run repeatedly:

  Serial.println("ISC");

  //Tx test

/*   msg.len = 8; msg.id = 0x321;
  msg.buf[0] = 1; msg.buf[1] = 2; msg.buf[2] = 3; msg.buf[3] = 4;
  msg.buf[4] = 5; msg.buf[5] = 6; msg.buf[6] = 7; msg.buf[7] = 8;
  CAN0.write(msg); */

    //Rx test
    
  if (can1.read(msg) ) {
    Serial.print("  ID: 0x"); Serial.print(msg.id, HEX );
    Serial.print("  EXT: "); Serial.print(msg.flags.extended );
    Serial.print("  LEN: "); Serial.print(msg.len);
    Serial.print(" DATA: ");
    for ( uint8_t i = 0; i < 8; i++ ) {
      Serial.print(msg.buf[i]); Serial.print(" ");
    }
    Serial.print("  TS: "); Serial.println(msg.timestamp);
  }

  delay(200);
}

