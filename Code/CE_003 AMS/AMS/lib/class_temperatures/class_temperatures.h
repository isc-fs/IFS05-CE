// -----------------------------------------------------------------------------
// Author       :   Javier R. Julianii
// Date         :   01/08/2020
// Name         :   class_cpu.h
// Description  :
// * This file is for defining the class of the connection protocols with putside
// -----------------------------------------------------------------------------

#ifndef CLASS_Temperatures_H
#define CLASS_Temperatures_H

#include "Arduino.h"
#include "module_can.h"
#include <DallasTemperature.h>
#include <OneWire.h>

#define Temperatures_OK                    0
#define Temperatures_ERROR_COMMUNICATION   1
#define Temperatures_ERROR_MAXIMUN_T       2


class Temperatures_MOD {
private:

  int error             = Temperatures_OK;                     // Variable for error handling
  int T_MAX             = 60;          //Maximum value of temperatures
  int T_MIN             = 0;
  INT32U  ID     = 0;          //ID of Receive Can device
  int j = 0;


  int Minimun_Temperature;
  int Maximun_Temperature;
  int MAX_T;


  INT8U message[2] = {0,0};


private:
  INT32U TIME_LIM_PLOT      = 0;   // Interval of time for ploting BMS info in ms
  INT32U TIME_LIM_SEND      = 5000;   // Interval to send mesage in ms
  INT32U TIME_LIM_MEASURE   = 5000;   // Limit time for communication in ms
  INT32U time_lim_plotted   = TIME_LIM_PLOT; // Dont plot in the initil time because the value is null
  INT32U time_lim_sended    = 0;      // When has been last sended
  INT32U time_lim_measure  = TIME_LIM_MEASURE;   // When have been last received // Initialized to 1000 to wait one minute to receive the data
  INT32U time_sending       = 0;

  int Tempe[95];
  float Tempt[95];

public:


  Temperatures_MOD(INT32U _ID,int _T_MAX, int _LAG=0);
  void info(char *buffer);
  int query(INT32U time, char *buffer);
  void Measure_Temp();
};
#endif
