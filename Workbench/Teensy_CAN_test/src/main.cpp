#include <FlexCAN_T4.h>

FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> myCan1;
FlexCAN_T4<CAN2, RX_SIZE_256, TX_SIZE_16> myCan2;
FlexCAN_T4<CAN3, RX_SIZE_256, TX_SIZE_16> myCan3;

void setup() {
  // put your setup code here, to run once:
  pinMode(LED_BUILTIN, OUTPUT);          // added to show loop() activity
  myCan1.begin();
  myCan2.begin();
  myCan3.begin();
  myCan1.setBaudRate(250 * 1000);
  myCan2.setBaudRate(250 * 1000);
  myCan3.setBaudRate(250 * 1000);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalToggle(LED_BUILTIN);          // added to show loop() activity
  CAN_message_t msg;
  msg.len = 8;
  msg.id = 1;
  msg.buf[0] = 1;
  msg.buf[1] = 2;
  msg.buf[2] = 3;
  msg.buf[3] = 4;
  msg.buf[4] = 5;
  msg.buf[5] = 6;
  msg.buf[6] = 7;
  msg.buf[7] = 8;

  if(myCan1.write(msg)==1){
    Serial.println("Enviado");
  }

  msg.id = 2;
  myCan2.write(msg);

  msg.id = 3;
  myCan3.write(msg);
  delay (500);
}