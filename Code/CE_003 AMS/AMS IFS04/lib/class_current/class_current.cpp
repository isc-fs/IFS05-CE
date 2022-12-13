// -----------------------------------------------------------------------------
// Author       :   Javier R. Juliani
// Date         :   20/05/2022
// Name         :   class_current.cpp
// Description  :
// * This file is for defining the class of the current
// -----------------------------------------------------------------------------

#include "class_current.h"


int pin=8;

// ********************************************************************************************************
// **Function name:           Current_MOD
// **Descriptions:            Initialization function of current class
// **********************************************************************************************************

Current_MOD::Current_MOD(INT32U ID,int _C_MAX)
{
  CANID = ID;
  C_MAX= _C_MAX;
  flag_error_current = 1;
}

  // ********************************************************************************************************
  // ** Function name:           query
  // ** Descriptions:            Function that transforms the voltage measured by the sensor to its equivalent current
  // **********************************************************************************************************
  int Current_MOD::query(int time, char *buffer)
  {

    error = Current_OK;

    VoltagemV=analogRead(pin);
    if(VoltagemV <=  400)
    {
     //error=Current_ERROR_Comunication;
    }
    VoltageV=VoltagemV*5/1023; //AnalogRead function reads a value between 0-1023 (1024, 10 bits) here I get the real voltage value based on the value the function gets

    if(VoltagemV >= 2.8) flag_current == 1;

    Current=(2.5-VoltageV)*60/0.34; //Sensitivity is 5,7 mv/A
    if(Current > C_MAX*0.8 && Current < C_MAX)
    {
        if(flag_error_current == 0) module_send_message_CAN1(0x500, 0, 1, message); /////////////If current between 80 and 100% of maximun, sends alert
    }

    if(Current > C_MAX)
    {
      if(flag_error_current == 1) module_send_message_CAN1(0x501,0,2,message); ////////////If current over maximun, sends alert
      flag_error_current++;
      if(flag_charger != 1) /////////////Only cut the AMS if accu not connected to charger, during charging the current control is on charger
      {
        if(flag_error_current >= 100) ///////////////i need to pass the maximun current for more than 100 times for cutting, maybe it was EMI
        {
          error = Current_ERROR_MAXIMUN_C;
          Serial.println("MAXIMA Corriente");
        }
      }
    }
    else
    {
      if(flag_error_current != 0)
      for(int i=0; i<5; i++)
      {
        module_send_message_CAN1(0x502, 0, 2, message); //////////////If current normal, sends green flag
      }
      flag_error_current = 0;
    }

    if(time>time_lim_sended)
    {
      time_lim_sended += TIME_LIM_SEND;
      message[0] =  0;
      message[1] =  Current & 0xFF;
      module_send_message_CAN1(CANID,0,2,message); //////////////Sends current through CAN each interval of ms
    }

// I'm gonna add the part where the info function is called
    if (TIME_LIM_PLOT>0 && time>time_lim_plotted)
    {
      //if(Serial) Serial.println(Current);

      time_lim_plotted += TIME_LIM_PLOT;
      //info(buffer);
    }
    return error;
  }

// ********************************************************************************************************
// **Function name:           info
// **Descriptions:            Function for printing the class data
// **********************************************************************************************************
void Current_MOD::info(char *buffer)
{

  if(Serial){     // Send the message just if there is a serial port connected
    Serial.println("\n***********************");
    Serial.println("         Current");
    Serial.println("***********************");
    sprintf(buffer, " - ERROR:     %i", error);
    sprintf(buffer, " - CAN ID:    0x%lx", CANID);
    sprintf(buffer, " - LIM C =    %i A", C_MAX);
    Serial.println("-----------------------");
    sprintf(buffer, "Current (A): %i", Current);
  }

}

// ********************************************************************************************************
//  END FILE
// **********************************************************************************************************
