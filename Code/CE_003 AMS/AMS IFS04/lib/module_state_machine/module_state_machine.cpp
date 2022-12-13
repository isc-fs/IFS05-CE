// -----------------------------------------------------------------------------
// Author       :   Luis de la Barba & Javier R. Juliani
// Date         :   17/04/2020
// Name         :   module_state_machine.cpp
// Description  :
// * This file is for the finite states machine
// -----------------------------------------------------------------------------

#include "module_state_machine.h"

// INITIALIZE VARIABLES
BMS_MOD BMS[] = { ///////////////////////////////////Here all the BMS get declare, the directions come from Zeva and the LAG of 50, 100 etc is for not asking them voltages at once
  BMS_MOD(BMS_ID+00, BMS_MAXV, BMS_MINV, BMS_MAXT,9,BMS_SHUNT),   // 3+3+3+3
  BMS_MOD(BMS_ID+10, BMS_MAXV, BMS_MINV, BMS_MAXT,10,BMS_SHUNT,50),    // 3+5
  BMS_MOD(BMS_ID+20, BMS_MAXV, BMS_MINV, BMS_MAXT,9,BMS_SHUNT,100),   // 5+5
  BMS_MOD(BMS_ID+30, BMS_MAXV, BMS_MINV, BMS_MAXT,10,BMS_SHUNT,150),   // 5+5
  BMS_MOD(BMS_ID+40, BMS_MAXV, BMS_MINV, BMS_MAXT,9,BMS_SHUNT,200),   // 5+5
  BMS_MOD(BMS_ID+50, BMS_MAXV, BMS_MINV, BMS_MAXT,10,BMS_SHUNT,250),   // 5+5
  BMS_MOD(BMS_ID+60, BMS_MAXV, BMS_MINV, BMS_MAXT,9,BMS_SHUNT,300),   // 5+5
  BMS_MOD(BMS_ID+70, BMS_MAXV, BMS_MINV, BMS_MAXT,10,BMS_SHUNT,350),   // 5+5
  BMS_MOD(BMS_ID+80, BMS_MAXV, BMS_MINV, BMS_MAXT,9,BMS_SHUNT,400),   // 5+5
  BMS_MOD(BMS_ID+90, BMS_MAXV, BMS_MINV, BMS_MAXT,10,BMS_SHUNT,450),   // 5+5
};
int BMS_N = 10;
int MIN_V = 4200;
INT8U message_MINV[2] = {0,0};      /////////////Here I'll get the minimun voltages for sending them for telemetry
int time_sending = 0;////////////////////////////For checking the interval I send the messages


Temperatures_MOD Tempt[] = {
  Temperatures_MOD(Temp_ID, T_MAX, 0),      ///////////This could not be a vector, but same as BMS also
};
int Temp_N = 1;


CPU_MOD CPU(CPU_ID_send,CPU_ID_recv,500); ////////////Same with CPU, rest of vehicle

int flag_charger = 0;                     /////////////For knowing wether I am charging or in the car


//SEV_MOD SEVCON(SEV_ID,SEV_ID+1,SEV_ID+2,SEV_ID+3,SEV_ID+4); //////////////////////////Do not worry about anything of these
//CHARGER_MOD CHARGER(CHARGER_ID_SEND, CHARGER_ID_RECV, CHARGER_MAXV, CHARGER_MAXI, CHARGER_MINI);

Current_MOD current (Current_ID, Current_max); ///////////////////Class for current measurement


STATE state = start;    ////////////////Variable on the state machine

char buffer[80];

// FUNCTION
/*********************************************************************************************************
** Function name:           setup_state_machine
** Descriptions:            Initializes the state machine library
*********************************************************************************************************/
void setup_state_machine() ///////////////Declaring the outputs of the AIR, 1 is AIR -, 2 is AIR + and precharge... precharge
{
  pinMode(RELAY_AIR_1, OUTPUT);    // sets the digital pin as output
  pinMode(RELAY_AIR_2, OUTPUT);    // sets the digital pin as output
  pinMode(RELAY_PRECHARGE, OUTPUT);    // sets the digital pin as output
}

/*********************************************************************************************************
** Function name:           select_state
** Descriptions:            check what should they do on the state machine
*********************************************************************************************************/
void select_state()
{
  int    state_air_1 = 0;  // 0 means open, 1 closed     // Turn on SEVCON
  int    state_air_2  = 0;  // 0 means open, 1 closed     // Energize HV relay
  int    state_precharge = 0; // 0 means open, 1 closed
  int   flag_cpu = CPU_ERROR_COMMUNICATION;
  int   flag_current = Current_ERROR_Comunication;
  //INT32U time_start;
  INT32U time = millis();  //////////////Reading the time that has been the micro since it has power
  int time_s;
  time_s = millis();


  CPU.voltage_acum=0; //For precharge


  MIN_V = 4200;     /// I reset the number each cycle cause if the voltages goes up again I wanna has it risen again on telemetry
  for (int i=0;i<BMS_N;i++)
  {
      BMS[i].voltage_acum = 0; //For precharge
      if (BMS[i].query(time,buffer) != BMS_OK) ////////////////I ask the BMS about voltages and cheking their states
      {
        state = error;
      }
      CPU.voltage_acum += BMS[i].voltage_acum; // For precharge
      if(BMS[i].MIN_V < MIN_V)
        MIN_V = BMS[i].MIN_V; //////////////////////////////Checking the minimun voltage of cell in the whole battery
  }
  if(time_s > time_sending+500)
  {
    message_MINV[1] = MIN_V & 0xFF;
    message_MINV[0] = (MIN_V>>8) & 0xFF;
    if(BMS[0].flag_charger != 1)
    {
      if(module_send_message_CAN1(BMS_ID, 0, 2, message_MINV)!=CAN_OK) Serial.println("Error al enviar tension minima"); ///////////////////Sending the message through telemtry each 500 ms
    }
    time_sending=time_s;

  }
  for (int i=0; i<BMS_N; i++)
  {

    BMS[i].BALANCING_V = MIN_V; ////////////////////////Here I say I wanna balance all the cells in the battery to the minimun

  }

  if(time>1000 && time<2000) MIN_V = 4200; /////////////Do not really worry about this, I dunno remember if this was really necesary
  for (int i=0; i<Temp_N; i++)
  {
      if(Tempt[i]. query(time, buffer) != Temperatures_OK) state = error; //////////////////Asking how temperatures are
  }


  flag_cpu = CPU.query(time,buffer); ///////////////////Asking the rest of the car how is it


  flag_current = current.query(time, buffer); ///////////////asking current how is it

  switch (state)
  {
    case start:
      state_air_1     = 0;
      state_air_2     = 0;
      state_precharge = 0;
      CPU.updateState(CPU_DICONNECTED);
      if(flag_cpu != CPU_ERROR_COMMUNICATION) state = precharge; ////////////////////If I do comunicate with the rest of the car, I go to precharge
      break;
    case precharge:
      state_air_1     = 1;
      state_air_2     = 0;
      state_precharge = 1;
      CPU.updateState(CPU_PRECHARGE);
      if (flag_cpu == CPU_OK)
      {
        state = transition;
      }
      //else if(flag_cpu == CPU_ERROR_COMMUNICATION) state = error;
      //else if(flag_current != Current_OK) state = error; //////////////I take this out cause in precharge current can be very high, but probably can be uncommented,
      break;
    case transition:
    state_air_1     = 1;
      state_air_2     = 0;
      state_precharge = 1;
      CPU.updateState(CPU_PRECHARGE);
      if(CPU.voltage_acum*0.9<CPU.DC_BUS) state = run; ///////////////If DC_BUS voltage is higher than 90% of battery voltage, precharge finish
      //else if((flag_cpu == CPU_ERROR_COMMUNICATION)&&(flag_charger == 1)) state = error;
      //else if(flag_current != Current_OK) state = error;
      break;
    case run:
      state_air_1     = 1;
      state_air_2     = 1;
      state_precharge = 1;
      CPU.updateState(CPU_POWER);
      if((flag_cpu == CPU_ERROR_COMMUNICATION)&&(flag_charger == 1)) state = error; /////////////////If I disconnect the charger, error
      if(flag_current != Current_OK) state = error; /////////////////If current is too high, error
      break;
    case error:
      state_air_1 = 0;  ///////////////All relés closed
      state_air_2 = 0;
      state_precharge = 0;
      CPU.updateState(CPU_ERROR);
      break;
  }

/*  Serial.println(state_air_1);
  Serial.print("Relee Prec:");
  Serial.println(state_precharge);*/

  digitalWrite(RELAY_AIR_1, state_air_1);
  digitalWrite(RELAY_AIR_2, state_air_2);
  digitalWrite(RELAY_PRECHARGE, state_precharge);
}

/*********************************************************************************************************
** Function name:           parse_state
** Descriptions:            Function for analysing the data from the CAN bus
*********************************************************************************************************/
void parse_state(CANMsg data)
{

  INT32U time = millis();
  boolean flag = false;

  for (int i=0;i<BMS_N;i++)
  {
      flag = BMS[i].parse(data.id,&data.buf[0],time); /////////////Checking if the message received is for  BMS
      if (flag) i = BMS_N;
  }
  if (!flag){
      if(CPU.parse(data.id,&data.buf[0],time)); //////////////Cheking if message is for CPU
      if(data.id==419385575)/////////////////////////////////If message from this direction received, it is because the charger is connected and the accu is for charging
      {
        for(int i=0; i<BMS_N;i++)
        {
          BMS[i].flag_charger = 1;

        }
        current.flag_charger = 1;
        flag_charger = 1;
      }
      else{
        //Serial.print("ID: ");
        //Serial.println(data.id);
      }
  }

}


/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
