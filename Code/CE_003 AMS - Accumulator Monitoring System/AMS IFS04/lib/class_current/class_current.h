// -----------------------------------------------------------------------------
// Author       :   Javier R. Juliani
// Date         :   20/05/2022
// Name         :   class_current.h
// Description  :
// * This file is for declaring the functions and variables of the class of the current
// -----------------------------------------------------------------------------

#ifndef CLASS_Current_H
#define CLASS_Current_H

#include "Arduino.h"
#include "module_can.h"

#define Current_OK                    0
#define Current_ERROR_MAXIMUN_C       1
#define Current_ERROR_Comunication    2


//#define pin  8

class Current_MOD {
private:

  int C_MAX             = 0;    //Maximum value of current

  INT32U  CANID         = 0;    // ID of can device
  INT8U   message[2]    = {0,0};

  float VoltageV = 0;             // Voltage in V     //All of them need to be in float, if the voltage changes from 0-5 V, if they were int, there would only be 5 possible values
  float VoltagemV = 0;
  int Current =0 ;              // Current
  float offset_V=0;               //offset voltage
  int time;

  int flag_error_current = 0;



  int TIME_LIM_PLOT   = 5000;     // The info will be shown through Serial comunications each 5 second
  int TIME_LIM_SEND   = 250;     //The value will be sended each second
  int time_lim_plotted= 50;
  int time_lim_sended = 50;

public:
  int error             = 0;    // Variable for error handling //Needs to be public so it can be seen from outside the class
  int voltage_acum  = 0;
  int flag_charger = 0;

  int flag_current = 0;


  Current_MOD(INT32U ID,int _C_MAX);
  int query(int time, char *buffer);
  void info(char *buffer);
};
#endif
