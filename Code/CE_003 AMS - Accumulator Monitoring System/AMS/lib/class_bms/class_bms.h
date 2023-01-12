// -----------------------------------------------------------------------------
// Author       :   Luis de la Barba & Javier R. Juliani
// Date         :   21/04/2020
// Name         :   class_bms.h
// Description  :
// * This file is for defining the BMS class
// -----------------------------------------------------------------------------

#ifndef CLASS_BMS_H
#define CLASS_BMS_H
#include "Arduino.h"
#include "module_can.h"

#define BMS_OK                    0
#define BMS_ERROR_COMMUNICATION   1
#define BMS_ERROR_VOLTS           2
#define BMS_ERROR_TEMP            3

class BMS_MOD {
private:
  INT32U CANID          = 0x12C;                          // The id of the CAN device
  int error             = BMS_OK;                     // Variable for error handling
  int cellVoltagemV[12] = {0,0,0,0,0,0,0,0,0,0,0,0};  // Voltage in mV
  int temperature[2]    = {0,0};                      // Temperatures
  int LIMIT_MAX_V = 0;
  int LIMIT_MIN_V = 0;
  int LIMIT_MAX_T = 0;
  int MAX_V = 0;

private:
  INT32U TIME_LIM_PLOT      = 1000;   // Interval of time for ploting BMS info in ms
  INT32U TIME_LIM_SEND      = 500;   // Interval to send mesage in ms
  INT32U TIME_LIM_RECV      = 3000;   // Limit time for communication in ms
  INT32U time_lim_plotted   = TIME_LIM_PLOT; // Dont plot in the initil time because the value is null
  INT32U time_lim_sended    = 0;      // When has been last sended
  INT32U time_lim_received  = TIME_LIM_RECV;   // When have been last received // Initialized to 1000 to wait one minute to receive the data
  INT8U message_balancing[2] = {0,0}; // Voltage in mV
  INT8U NUM_CELLS           = 3;
  int max_flag              = 3;
  int flag_error_volt[12]   = {0,0,0,0,0,0,0,0,0,0,0,0};
  int flag_error_temp       = 0;

public:
  int voltage_acum          = 0;
  int MIN_V = 0;
  int BALANCING_V = 4200;
  int flag_charger = 0;

  BMS_MOD(INT32U _ID, int _MAXV, int _MINV, int _MAXT, INT8U _NUMCELLS,unsigned int _SHUNT, int _LAG=0);
  void info(char *buffer);
  boolean parse(INT32U id,INT8U *buf,INT32U t);
  int return_error();
  int query(INT32U time, char *buffer);
};

#endif
