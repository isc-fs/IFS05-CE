#include <class_temp0.h>

OneWire OurWire(3);
DallasTemperature sensors(&OurWire);    //Internal function of Library
DeviceAddress address[] = {
        {0x28, 0xFF, 0x64, 0x01, 0xBC, 0x60, 0xEB, 0x5D},    //  1
        {0x28, 0xFF, 0x64, 0x01, 0xBC, 0x18, 0x51, 0xD1},    //  2
        {0x28, 0xFF, 0x64, 0x01, 0xBC, 0x14, 0x01, 0x47},    //  3
        {0x28, 0xFF, 0x64, 0x01, 0xBC, 0x6A, 0xED, 0x67},    //  4
        {0x28, 0xFF, 0x64, 0x01, 0xBC, 0x61, 0xEA, 0xC7},    //  5
        {0x28, 0xFF, 0x64, 0x01, 0xBC, 0x63, 0xDF, 0xD7},    //  6
        {0x28, 0xFF, 0x64, 0x01, 0xBC, 0x6B, 0x4B, 0xD1},    //  7
        {0x28, 0xFF, 0x64, 0x01, 0xBC, 0x1B, 0xBD, 0xCE},    //  8
        {0x28, 0xFF, 0x64, 0x01, 0xBE, 0x0B, 0x64, 0xA6},    //  9
        {0x28, 0xFF, 0x64, 0x01, 0xB9, 0x60, 0xB4, 0xF2},    //  10
        {0x28, 0xFF, 0x64, 0x01, 0xB9, 0x7E, 0x62, 0x48},    //  11
        {0x28, 0xFF, 0x64, 0x01, 0xB9, 0x75, 0x3C, 0xAF},    //  12
        {0x28, 0xFF, 0x64, 0x01, 0xB9, 0x6D, 0x44, 0x0F},    //  13
        {0x28, 0xFF, 0x64, 0x01, 0xB9, 0x7F, 0xF3, 0xC3},    //  14
        {0x28, 0xFF, 0x64, 0x01, 0xBD, 0xA4, 0x69, 0x49},    //  15
        {0x28, 0xFF, 0x64, 0x01, 0xBD, 0x86, 0x23, 0x21},    //  16
        {0x28, 0xFF, 0x64, 0x01, 0xBD, 0xA7, 0x25, 0xF9},    //  17
        {0x28, 0xFF, 0x64, 0x01, 0xBD, 0xAF, 0x3E, 0x32},    //  18
        {0x28, 0xFF, 0x64, 0x01, 0xBF, 0x8A, 0xAF, 0x0C},    //  19
};





/********************************************************************************************************
**  Function name:          temperatures
**  Desciptions:            Initialitates values of class
*********************************************************************************************************/
Temperatures::Temperatures(float *temp,int *tempe)
{
  *Tempt=*temp;
  *Tempe=*tempe;

  tam = sizeof(address)/8;
  sensors.begin();              //Internal function of library
}

/*********************************************************************************************************
** Function name:           Measure_Temp
** Descriptions:            Reads Temperatures from temperatures sensors
*********************************************************************************************************/
void Temperatures::Measure_Temp()
{
  sensors.requestTemperatures();
  for (int i=0; i<tam; i++)
  {
    Tempt[i]=sensors.getTempC(address[i]);  //Intenal function of library
    //Serial.println(Tempt[i]);
    Tempe[i]=int(Tempt[i]);
    //Serial.println(Tempe[i]);
  }
  MAX_T = Tempe[0];
  for (int j=0; j<tam; j++)
  {
      if(Tempe[j]>MAX_T)
      {
        MAX_T = Tempe[j];
      }
  }
}

/*********************************************************************************************************
** Function name:           Choose_array
** Descriptions:            Selects which of all the temperature sensors wants to get
*********************************************************************************************************/
void Temperatures::Send()
{
    Enviar[0] = MAX_T;


    for(int j=0; j<3; j++){
      for(int x =0; x<8; x++){
        if((x+8*j)<tam){
          Enviar[x] = Tempe[x+8*j];
        }else{
          Enviar[x] = 0;
        }
      }
    }
  if(module_send_message_CAN0(CANID, 1, 8, Enviar));
}
