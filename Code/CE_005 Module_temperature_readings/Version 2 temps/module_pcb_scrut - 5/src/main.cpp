// CAN Receive Example
//

#include "Arduino.h"
#include "module_can.h"
#include "module_state_machine.h"

CANMsg message;

void setup()
{
  Serial.begin(115200);
  setup_module_can();
  //setup_state_machine();
}

void loop()
{
  select_state();
  //module_get_data_CAN0();
  moduleCANCheckForData();
  if (module_get_buffer_data(&message)==FULL_DATA)
  {
      parse_state(message);
      //digitalWrite(54,1);
      /*
      Serial.print("ID: ");
      Serial.print(message.id);
      Serial.print(" DLC: ");
      Serial.print(message.len);
      Serial.print(" DATA: ");
      for(byte i = 0; i<8; i++){
          Serial.print(message.buf[i]);
          Serial.print(" ");
      }
      Serial.println("");
      */

  }else{
    //if (module_send_message_CAN0(bms_id,bms_ext,bms_len,bms_volt1)!=CAN_OK)     Serial.println("MESSAGE NOT SENDED");
    //delay(500);
  }

}

/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
