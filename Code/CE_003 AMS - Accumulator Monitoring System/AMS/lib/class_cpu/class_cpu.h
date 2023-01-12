// -----------------------------------------------------------------------------
// Author       :   Luis de la Barba & Javier R. Juliani
// Date         :   01/08/2020
// Name         :   class_cpu.h
// Description  :
// * This file is for defining the class of the connection protocols with putside
// -----------------------------------------------------------------------------

#ifndef CLASS_CPU_H
#define CLASS_CPU_H

#include "Arduino.h"
#include "module_can.h"

#define CPU_OK                    0
#define CPU_ERROR_COMMUNICATION   1
#define CPU_BUS_LINE_OK           2

#define CPU_POWER       0
#define CPU_PRECHARGE   1
#define CPU_DICONNECTED 2
#define CPU_ERROR       3

class CPU_MOD {
private:
  INT32U CANID_send     = 0;
  INT32U CANID_recv     = 0;                          // The id of the CAN device
  int error             = CPU_ERROR_COMMUNICATION;                     // Variable for error handling
  INT8U currentState[1]    = {CPU_DICONNECTED};
  int current_state    = CPU_DICONNECTED;
  /*
  int cellVoltagemV[12] = {0,0,0,0,0,0,0,0,0,0,0,0};  // Voltage in mV
  int temperature[2]    = {0,0};                      // Temperatures
  int LIMIT_MAX_V = 0;
  int LIMIT_MIN_V = 0;
  int LIMIT_MAX_T = 0;
  int MAX_V = 0;
  int MIN_V = 0;
  */
private:
  INT32U TIME_LIM_PLOT      = 0;   // Interval of time for ploting BMS info in ms
  INT32U TIME_LIM_SEND      = 450;   // Interval to send mesage in ms
  INT32U TIME_LIM_RECV      = 1000000;   // Limit time for communication in ms
  INT32U time_lim_plotted   = TIME_LIM_PLOT; // Dont plot in the initil time because the value is null
  INT32U time_lim_sended    = 0;      // When has been last sended
  INT32U time_lim_received  = TIME_LIM_RECV;   // When have been last received // Initialized to 1000 to wait one minute to receive the data

public:
  int voltage_acum          = 0;
  int DC_BUS                 = 0; // Voltage in V of the bus outside the accumulator

  CPU_MOD(INT32U _ID_send,INT32U _ID_recv, int _LAG=0);
  void info(char *buffer);
  int return_error();
  int query(INT32U time, char *buffer);
  boolean parse(INT32U id,INT8U *buf,INT32U t);
  void updateState(int s);
};
#endif
