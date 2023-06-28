#include <FlexCAN_T4.h>

FlexCAN_T4<CAN2, RX_SIZE_256, TX_SIZE_16> myCan2;

void setup() {
  // put your setup code here, to run once:
  pinMode(LED_BUILTIN, OUTPUT);          // added to show loop() activity
  Serial.begin(115200);
  myCan2.begin();

  myCan2.setBaudRate(250 * 1000);

}

void loop() {
  // put your main code here, to run repeatedly:
  digitalToggle(LED_BUILTIN);          // added to show loop() activity
  CAN_message_t msg;
  msg.len = 8;
  msg.id = 0x12C;
  msg.buf[0] = 1;
  msg.buf[1] = 2;
  msg.buf[2] = 3;
  msg.buf[3] = 4;
  msg.buf[4] = 5;
  msg.buf[5] = 6;
  msg.buf[6] = 7;
  msg.buf[7] = 8;

 if(myCan2.write(msg)==1){
  Serial.println("Enviado");

 }else{
  Serial.println("No");
 }
}