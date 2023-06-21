// -----------------------------------------------------------------------------
// Author       :   Javier R. Juliani
// Date         :   11/04/2022
// Name         :   module_state_machine.cpp
// Description  :
// * This file is for the finite states machine
// -----------------------------------------------------------------------------

#include "module_state_machine.h"

char buffer[80];
Temperatures temperaturas(0, 0);

/*********************************************************************************************************
** Function name:           select_state
** Descriptions:            check what should they do on the state machine
*********************************************************************************************************/
void select_state()
{
  temperaturas.Measure_Temp();
}

/*********************************************************************************************************
** Function name:           parse_state
** Descriptions:            Function for analysing the data from the CAN bus
*********************************************************************************************************/
void parse_state(CANMsg data)
{
  Serial.println("HOLA");
  if(data.id==RECVID)
  {
    //Serial.println("HEY");
    temperaturas.Send();
  }
  else{
    //Serial.print("data:");
    //Serial.println(data.id);
  }
}


/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
