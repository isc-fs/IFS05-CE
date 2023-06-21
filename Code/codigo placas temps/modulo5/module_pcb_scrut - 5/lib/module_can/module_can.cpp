// -----------------------------------------------------------------------------
// Author       :   Luis de la Barba
// Date         :   03/04/2020
// Name         :   module_can.h
// Description  :
// * This file is for the CAN comunications
// -----------------------------------------------------------------------------

#include "module_can.h"

MCP_CAN CAN0(CAN0CS);     // Set CS to pin CAN0CS and initialize the CAN class
//MCP_CAN CAN1(CAN1CS);     // Set CS to pin CAN1CS and initialize the CAN class

// CAN MSG Buffer (circular buffer -> if full, overwrites messages)
#define N     50
int           tailCounter = 0;
int           headCounter = 0;
CANMsg MSGBuffer[N];

// FUNCTION
/*********************************************************************************************************
** Function name:           setup_module_can
** Descriptions:            Initializates the CAN libraries
*********************************************************************************************************/
void setup_module_can()
{
  // Initialize MCP2515 running at CAN0CLOCK with a baudrate of CAN0SPEED kb/s and the masks and filters disabled.
    while (CAN0.begin(MCP_ANY, CAN0SPEED, CAN0CLOCK) != CAN_OK){
        Serial.println("CAN0 >> Error Initializing MCP2515...");
        delay(500);
    }
    Serial.println("CAN0 >> MCP2515 Initialized Successfully!");

    /*while (CAN1.begin(MCP_ANY, CAN1SPEED, CAN1CLOCK) != CAN_OK){
        Serial.println("CAN1 >> Error Initializing MCP2515...");
        delay(500);
    }
    Serial.println("CAN1 >> MCP2515 Initialized Successfully!");
*/
    /*
      * MCP_NORMAL:       Normal CAN Bus communication
      * MCP_LOOPBACK:     Internally interconnect Rx and TX for testing
      * MODE_CONFIG:      For changing the bit rate, filter mask, register
      * MCP_SLEEP:        Sleep mode for low consumption
      * MCP_LISTENONLY:   Just listen the inputs, so it cant send data
    */
    CAN0.setMode(MCP_NORMAL);   // Change to normal mode to allow messages to be transmitted
    //CAN1.setMode(MCP_NORMAL);   // Change to normal mode to allow messages to be transmitted

    // Declarte how to manage INT pin, by constant reading or by interrupt
    // pinMode(CAN0INT, INPUT);    // Configuring pin for /INT input
    //attachInterrupt(digitalPinToInterrupt(CAN0INT), module_get_data_CAN0, LOW);
    //attachInterrupt(digitalPinToInterrupt(CAN1INT), module_get_data_CAN1, LOW);
}

/*********************************************************************************************************
** Function name:           module_send_message_CAN0
** Descriptions:            Send a message
*********************************************************************************************************/
byte module_send_message_CAN0(INT32U id, INT8U ext, INT8U len, INT8U *buf)
{
  /*
    * id = direction of the message
    * ext = 0 for standart length of the message and 1 for extended direction
    * len = length of the sended mesage (lenght of the buff array)
    * buf = array of the data to be sent
  */
  byte sndStat;
  int flag = 0;
  while(flag < 5){
      sndStat = CAN0.sendMsgBuf(id, ext, len, buf);
      if(sndStat == CAN_OK) flag = 5;
      else flag++;
  }
  return sndStat;
}

/*********************************************************************************************************
** Function name:           module_send_message_CAN0
** Descriptions:            Send a message
*********************************************************************************************************/
/*byte module_send_message_CAN1(INT32U id, INT8U ext, INT8U len, INT8U *buf)
{
  /*
    * id = direction of the message
    * ext = 0 for standart length of the message and 1 for extended direction
    * len = length of the sended mesage (lenght of the buff array)
    * buf = array of the data to be sent
  */
  /*
  byte sndStat;
  int flag = 0;
  while(flag < 5){
      sndStat = CAN1.sendMsgBuf(id, ext, len, buf);
      if(sndStat == CAN_OK) flag = 5;
      else flag++;
  }
  return sndStat;
}
*/
/*********************************************************************************************************
** Function name:           module_get_data_CAN0
** Descriptions:            Process the message received in the CAN0
*********************************************************************************************************/
void module_get_data_CAN0()
{
    long unsigned int rxId;

    if (!CAN0.readMsgBuf(&rxId, &MSGBuffer[headCounter].len, &MSGBuffer[headCounter].buf[0]))      // Read data: len = data length, buf = data byte(s)
    {
      if((rxId & 0x80000000) == 0x80000000)     // Determine if ID is standard (11 bits) or extended (29 bits)
          MSGBuffer[headCounter].id = (rxId & 0x1FFFFFFF);
      else
          MSGBuffer[headCounter].id = rxId;

      //MSGBuffer[headCounter].time = millis();
      MSGBuffer[headCounter].bus = 0;
      headCounter++;
      headCounter %= N;
    }
  /*
  if(!digitalRead(CAN0_INT))                         // If CAN0_INT pin is low, read receive buffer
  {
    CAN0.readMsgBuf(&rxId, &len, rxBuf);      // Read data: len = data length, buf = data byte(s)

    if((rxId & 0x80000000) == 0x80000000)     // Determine if ID is standard (11 bits) or extended (29 bits)
      sprintf(msgString, "Extended ID: 0x%.8lX  DLC: %1d  Data:", (rxId & 0x1FFFFFFF), len);
    else
      sprintf(msgString, "Standard ID: 0x%.3lX       DLC: %1d  Data:", rxId, len);

    Serial.print(msgString);

    if((rxId & 0x40000000) == 0x40000000){    // Determine if message is a remote request frame.
      sprintf(msgString, " REMOTE REQUEST FRAME");
      Serial.print(msgString);
    } else {
      for(byte i = 0; i<len; i++){
        sprintf(msgString, " 0x%.2X", rxBuf[i]);
        Serial.print(msgString);
      }
    }

    Serial.println();
  }
  */
}

/*********************************************************************************************************
** Function name:           module_get_data_CAN1
** Descriptions:            Process the message received in the CAN1
*********************************************************************************************************/
/*
void module_get_data_CAN1()
{
    long unsigned int rxId;

    if (!CAN1.readMsgBuf(&rxId, &MSGBuffer[headCounter].len, &MSGBuffer[headCounter].buf[0]))      // Read data: len = data length, buf = data byte(s)
    {
      if((rxId & 0x80000000) == 0x80000000)     // Determine if ID is standard (11 bits) or extended (29 bits)
          MSGBuffer[headCounter].id = (rxId & 0x1FFFFFFF);
      else
          MSGBuffer[headCounter].id = rxId;

      MSGBuffer[headCounter].time = millis();
      MSGBuffer[headCounter].bus  = 1;
      headCounter++;
      headCounter %= N;
    }
}

/*********************************************************************************************************
** Function name:           module_process_data_CAN0
** Descriptions:            Process the buffer and assign the data
*********************************************************************************************************/
byte module_get_buffer_data(CANMsg *var)
{

    if (headCounter != tailCounter){
        *var = MSGBuffer[tailCounter];
        tailCounter++;
        tailCounter %= N;
        return FULL_DATA;
    }else{
        return EMPTY_DATA;
    }
}

void moduleCANCheckForData(void){
  if (digitalRead(CAN0INT)==LOW)
  {
    module_get_data_CAN0();
    //Serial.println("HAA");
  }

  //if (digitalRead(CAN1INT)==LOW) module_get_data_CAN1();
  return;
}
/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
