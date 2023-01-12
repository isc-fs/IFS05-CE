// -----------------------------------------------------------------------------
// Author       :   Luis de la Barba & Javier R. Juliani
// Date         :   01/08/2020
// Name         :   class_cpu.cpp
// Description  :
// * This file is for defining the class of the connection protocols with putside
// -----------------------------------------------------------------------------

#include "class_cpu.h"



//
// FUNCTION
// ********************************************************************************************************
// **Function name:           CPU_MOD
// **Descriptions:            Initialization function of teh class
// **********************************************************************************************************

CPU_MOD::CPU_MOD(INT32U _ID_send,INT32U _ID_recv, int _LAG) ////////////////////////////////////Habia un _LAG=0
{
  CANID_send       = _ID_send;
  CANID_recv       = _ID_recv;

  time_lim_plotted += _LAG;
  time_lim_sended += _LAG;
  time_lim_received += _LAG;
}

// ********************************************************************************************************
// **Function name:           info
// **Descriptions:            Function for printing the class data
// **********************************************************************************************************
void CPU_MOD::info(char *buffer)
{

  if(Serial){     // Send the message just if there is a serial por connected
    Serial.println("\n***********************");
    Serial.println("         CPU");
    Serial.println("***********************");
    sprintf(buffer, " - ERROR:     %i", error);           Serial.println(buffer);
    sprintf(buffer, " - CAN ID_send:    0x%lx", CANID_send);        Serial.println(buffer);
    sprintf(buffer, " - CAN ID_recv:    0x%lx", CANID_recv);        Serial.println(buffer);
    sprintf(buffer, " - DC BUS:         %i", DC_BUS);        Serial.println(buffer);
    sprintf(buffer, " - DC STATE:       %i", current_state);        Serial.println(buffer);
    Serial.println("-----------------------");
  }
}

// ********************************************************************************************************
// **Function name:           parse
// **Descriptions:            Function for parsing the received data via CAN protocl
// **********************************************************************************************************
boolean CPU_MOD::parse(INT32U id,byte *buf,INT32U t)
{

  if (id==0x100){
      error = CPU_OK;
      time_lim_received   = t + TIME_LIM_RECV;
      DC_BUS = ((buf[0]<<8) + buf[1]); //////////////////In this direction it is send the voltage in DC_BUS
      //Serial.println(DC_BUS);
      if (DC_BUS>280) //(DC_BUS>0.9*voltage_acum)
      {
        error = CPU_BUS_LINE_OK;
        if (module_send_message_CAN1(CANID_send,2,1,currentState)!=CAN_OK)   error = CPU_ERROR_COMMUNICATION;
      }
      return true;
  }
    return false;
}


// ********************************************************************************************************
// **Function name:           return_error
// **Descriptions:            Function for returning the state of the BMS
// **********************************************************************************************************
int CPU_MOD::return_error()
{
  return error;
}

// ********************************************************************************************************
// ** Function name:           query
// ** Descriptions:            Function to check if i need to send a mesage new mesage and the received mesajes interval are within the limits
// **********************************************************************************************************
int CPU_MOD::query(INT32U time, char *buffer)
{
// Function for performing a correct behaivour
    if (time>time_lim_sended){ // HERE I HAVE TO SEND THE REQUEST MESSAGE FOR THE BMS
      time_lim_sended += TIME_LIM_SEND;
    }
    if (time>time_lim_received) error = CPU_ERROR_COMMUNICATION; ////////////Cheking if everything is alright each time
    if (TIME_LIM_PLOT>0 && time>time_lim_plotted){
      time_lim_plotted += TIME_LIM_PLOT;
      info(buffer);
    }
  return error;
}

void CPU_MOD::updateState(int s)
{
  currentState[0] = s & 0xFF;
  current_state    = s;
}
// ********************************************************************************************************
//  END FILE
// **********************************************************************************************************
