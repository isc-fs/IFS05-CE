// -----------------------------------------------------------------------------
// Author       :   Luis de la Barba
// Date         :   17/04/2020
// Name         :   module_state_machine.h
// Description  :
// * This file is for the finite state machine initialization

// -----------------------------------------------------------------------------

#ifndef MODULE_STATE_MACHINE_H
#define MODULE_STATE_MACHINE_H

  #include "Arduino.h"
  #include "module_can.h"
  #include "class_temp0.h"

#define SENDID  0x400+40
#define RECVID  0x400+40+19

  // FUNCTIONS
  void select_state();
  void parse_state(CANMsg data);

#endif
