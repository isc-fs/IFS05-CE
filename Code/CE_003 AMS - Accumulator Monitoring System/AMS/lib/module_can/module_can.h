// -----------------------------------------------------------------------------
// Author       :   Luis de la Barba
// Date         :   03/04/2020
// Name         :   module_can.h
// Description  :
// * This file is for the CAN comunications
// -----------------------------------------------------------------------------

#ifndef MODULE_CAN_H
#define MODULE_CAN_H

  #include "Arduino.h"
  #include <mcp_can.h>
  #include <SPI.h>

  // CAN0
  #define CAN0SPEED   CAN_250KBPS
  #define CAN0CLOCK   MCP_8MHZ
  #define CAN0INT     2
  #define CAN0CS      53//10//53

  // CAN1
  #define CAN1SPEED   CAN_250KBPS//CAN_500KBPS
  #define CAN1CLOCK   MCP_8MHZ
  #define CAN1INT     3
  #define CAN1CS      48

  #define MAX_NUM_QUERY 5 // Number of maximums queries to be sended before skipping process of sending a message
  #define FULL_DATA     1 // Data in buffer
  #define EMPTY_DATA    0 // No data in buffer

  // CAN structure for buffer of messages
  struct CANMsg{
      INT32U id = 0;      // CAN ID
      INT8U  len = 0;     // Vector length
      INT8U  buf[8];      // CAN vector
      int    bus;         // From which CAN bus is received
      INT32U time = 0;    // When was the message received
  };

  // Declare global functions
  void setup_module_can();
  byte module_send_message_CAN0(INT32U id, INT8U ext, INT8U len, INT8U *buf);
  byte module_send_message_CAN1(INT32U id, INT8U ext, INT8U len, INT8U *buf);
  void module_get_data_CAN0();
  void module_get_data_CAN1();
  byte module_get_buffer_data(CANMsg *var);
  void moduleCANCheckForData(void);

#endif
