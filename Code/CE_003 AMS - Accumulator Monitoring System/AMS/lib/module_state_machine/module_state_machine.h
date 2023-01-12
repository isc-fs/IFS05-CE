// -----------------------------------------------------------------------------
// Author       :   Luis de la Barba & Javier R. Juliani
// Date         :   17/04/2020
// Name         :   module_state_machine.h
// Description  :
// * This file is for the finite state machine initialization

// -----------------------------------------------------------------------------

#ifndef MODULE_STATE_MACHINE_H
#define MODULE_STATE_MACHINE_H

  #include "Arduino.h"
  #include "module_can.h"
  #include "class_bms.h"
  #include "class_cpu.h"
  #include "class_current.h"
  #include "class_temperatures.h"

  // BMS
  #define BMS_ID    0x12C
  #define BMS_MAXV  4200    // mV
  #define BMS_MINV  2800
  #define BMS_MAXT  60      // Celsius º
  #define BMS_SHUNT 4000 // 3750    // mV voltage for balancing

  // CAR
  #define CPU_ID_send 0x020
  #define CPU_ID_recv 0x100

  // Current Class
  #define Current_ID 0x450
  #define Current_max 200

  #define T_MAX 60
  #define Temp_ID 0x400

  // Digital Outputs
  #define RELAY_AIR_1     36 // 4
  #define RELAY_AIR_2     35 // 5
  #define RELAY_PRECHARGE 34 // 5 34

  // Current States available for the Finite State MAchine
  enum STATE{start, precharge, transition, run, error};


  // FUNCTIONS
  void setup_state_machine();
  void select_state();
  void parse_state(CANMsg data);

#endif
