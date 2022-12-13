// -----------------------------------------------------------------------------
// Author       :   Luis de la Barba & Javier R. Juliani
// Date         :   21/04/2020
// Name         :   class_bms.cpp
// Description  :
// * This file is for defining the BMS class
// -----------------------------------------------------------------------------

#include "class_bms.h"
///////////////////////////////////////////////Dont ever touch this function if BMS are still ZEVA
// FUNCTION
/*********************************************************************************************************
** Function name:           BMS_MOD
** Descriptions:            Initialization function of teh class
*********************************************************************************************************/

BMS_MOD::BMS_MOD(INT32U _ID, int _MAXV, int _MINV, int _MAXT, INT8U _NUMCELLS, unsigned int _SHUNT, int _LAG=0)
{
  CANID       = _ID;
  LIMIT_MAX_V = _MAXV;
  LIMIT_MIN_V = _MINV;
  LIMIT_MAX_T = _MAXT;
  NUM_CELLS   = _NUMCELLS;

  //TIME_LIM_SEND = 5000;   // Interval to send mesage
  //TIME_LIM_RECV = 1000;   // Limit tiem for communication
  //message_balancing[1] = _SHUNT & 0xFF;           // Coment this two lines for disabling the balancing
  //message_balancing[0] = (_SHUNT >> 8) & 0xFF;    // Coment this two lines for disabling the balancing

  time_lim_plotted += _LAG;
  time_lim_sended += _LAG;
  time_lim_received += _LAG;
}

/*********************************************************************************************************
** Function name:           info
** Descriptions:            Function for printing the class data
*********************************************************************************************************/
void BMS_MOD::info(char *buffer)
{

  if(Serial){     // Send the message just if there is a serial por connected
    Serial.println("\n***********************");
    Serial.println("         BMS");
    Serial.println("***********************");
    sprintf(buffer, " - ERROR:     %i", error);           Serial.println(buffer);
    sprintf(buffer, " - CAN ID:    0x%lx", CANID);        Serial.println(buffer);
    sprintf(buffer, " - MAX V =    %i mV", LIMIT_MAX_V);  Serial.println(buffer);
    sprintf(buffer, " - MIN V =    %i mV", LIMIT_MIN_V);  Serial.println(buffer);
    sprintf(buffer, " - MAX T =    %i Cº", LIMIT_MAX_T);  Serial.println(buffer);
    Serial.println("-----------------------");
    sprintf(buffer, "VOLTS (mV): [%i", cellVoltagemV[0]); Serial.print(buffer);
    for (int i=1; i<12; i++){
      sprintf(buffer, ", %i", cellVoltagemV[i]);
      Serial.print(buffer);
      voltage_acum+=cellVoltagemV[i];
    }
    Serial.println("]");
    sprintf(buffer, " - V(max) = %i mV || V(min) = %i", MAX_V, MIN_V);  Serial.println(buffer);
    sprintf(buffer,"TEMP  (ºC): [%i, %i]", temperature[0], temperature[1]);  Serial.println(buffer);
  }
  sprintf(buffer,"- BALANCING V = %i mV",BALANCING_V); Serial.println(buffer);
}

/*********************************************************************************************************
** Function name:           parse
** Descriptions:            Function for parsing the received data via CAN protocl
*********************************************************************************************************/
boolean BMS_MOD::parse(INT32U id,byte *buf,INT32U t)
{
  if (id>CANID && id<CANID+10){
    int m = id%CANID;
    int pos = 0;
    if (m>0 && m<5){
      time_lim_received   = t + TIME_LIM_RECV;                                                          // Reset the timer flag for checking if the data is received
      if (m<4){
        if(flag_charger == 1) module_send_message_CAN1(id,1,8,buf);
        for (int i = 0; i < 4; i++)                                               // i = number of cell within message
        {
          pos = (m-1)*4 + i;
          cellVoltagemV[pos] = (buf[2*i] << 8) + buf[2*i + 1];
          if ((cellVoltagemV[pos]>LIMIT_MAX_V ||
              cellVoltagemV[pos]<LIMIT_MIN_V) &&
              pos<NUM_CELLS)
          {
            flag_error_volt[pos] = flag_error_volt[pos] + 1;
            if (flag_error_volt[pos] >= max_flag) error = BMS_ERROR_VOLTS;
          }else flag_error_volt[pos] = 0;
        }
        MAX_V = cellVoltagemV[0];
        MIN_V = cellVoltagemV[0];
        for (int i = 1; i < NUM_CELLS; i++)                                               // i = number of cell within message
        {
          if(cellVoltagemV[i] > MAX_V) MAX_V = cellVoltagemV[i];
          else if (cellVoltagemV[i] < MIN_V) MIN_V = cellVoltagemV[i];
        }
        message_balancing[1] = BALANCING_V & 0xFF;           // Coment this two lines for disabling the balancing
        message_balancing[0] = (BALANCING_V >> 8) & 0xFF;    // Coment this two lines for disabling the balancing

      }else if(m==4){
        for (int i = 0; i < 2; i++)
        {
          temperature[i] = buf[i] - 40;
          if (temperature[i]>LIMIT_MAX_T)
          {
            flag_error_temp++;
            if (flag_error_temp >= max_flag) error = BMS_ERROR_TEMP;
          }else flag_error_temp = 0;
        }
      }
      return true;
    }
  }
    return false;
}

/*********************************************************************************************************
** Function name:           return_error
** Descriptions:            Function for returning the state of the BMS
*********************************************************************************************************/
int BMS_MOD::return_error()
{
  return error;
}

/*********************************************************************************************************
** Function name:           query
** Descriptions:            Function to check if i need to send a mesage new mesage and the received mesajes interval are within the limits
*********************************************************************************************************/
int BMS_MOD::query(INT32U time, char *buffer)
{
  //INT8U message_balancing[2] = {0,0}; // Voltage in mV
// Function for performing a correct behaivour
    if (time>time_lim_sended){ // HERE I HAVE TO SEND THE REQUEST MESSAGE FOR THE BMS
      time_lim_sended += TIME_LIM_SEND;
      if (module_send_message_CAN0(CANID,1,2,message_balancing)!=CAN_OK)
      {
        error = BMS_ERROR_COMMUNICATION; // If the message is not sended then, error
      }
    }
    if (time>time_lim_received)
    {
      error = BMS_ERROR_COMMUNICATION;
    }
    if (TIME_LIM_PLOT>0 && time>time_lim_plotted){
      time_lim_plotted += TIME_LIM_PLOT;
      info(buffer);
    }
  return error;
}

/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
