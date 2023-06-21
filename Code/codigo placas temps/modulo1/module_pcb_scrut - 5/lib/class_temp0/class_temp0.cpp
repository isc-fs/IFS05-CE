#include <class_temp0.h>
#include <stdio.h>

OneWire OurWire(3);
DallasTemperature sensors(&OurWire);    //Internal function of Library

/*Cada modulo está formado por dos PCBs (coinciden, es decir la 1.1 va con la 1.2, cada PCB está marcada con su número), 
y los sensores se enumeran de izquierda a derecha y de arriba a abajo, es decir el de arriba a la izquierda es el sensor 1,
el de su derecha es el 2, el de su derecha es el 3...*/
DeviceAddress address[] = {
  {0x28, 0xFF, 0x67, 0xA3, 0x3A, 0x21, 0x03, 0x7A}, //Sensor 1 - PCB 1.1
  {0x28, 0xFF, 0x19, 0xD3, 0x3A, 0x21, 0x03, 0x1D}, //Sensor 2 - PCB 1.1
  {0x28, 0xFF, 0x0F, 0xD1, 0x3A, 0x21, 0x03, 0xFA}, //Sensor 3 - PCB 1.1
  {0x28, 0xFF, 0x1A, 0xE0, 0x3A, 0x21, 0x03, 0x93}, //Sensor 4 - PCB 1.1
  {0x28, 0xFF, 0x9C, 0x90, 0x3A, 0x21, 0x03, 0x6C}, //Sensor 5 - PCB 1.1
  {0x28, 0xFF, 0x0E, 0xA6, 0x3A, 0x21, 0x03, 0x19}, //Sensor 6 - PCB 1.1
  {0x28, 0xFF, 0xA2, 0xDB, 0x3A, 0x21, 0x03, 0x3E}, //Sensor 7 - PCB 1.1
  {0x28, 0xFF, 0xF3, 0xC8, 0x3A, 0x21, 0x03, 0xD6}, //Sensor 8 - PCB 1.1
  {0x28, 0xFF, 0x09, 0x96, 0x3A, 0x21, 0x03, 0x00}, //Sensor 9 - PCB 1.1

  {0x28, 0xFF, 0x1A, 0xC7, 0x3A, 0x21, 0x03, 0x65}, //Sensor 1 - PCB 1.2
  {0x28, 0xFF, 0xAF, 0xD1, 0x3A, 0x21, 0x03, 0xC9}, //Sensor 2 - PCB 1.2
  {0x28, 0xFF, 0x11, 0xC6, 0x3A, 0x21, 0x03, 0x9A}, //Sensor 3 - PCB 1.2
  {0x28, 0xFF, 0x5B, 0xD8, 0x3A, 0x21, 0x03, 0xE3}, //Sensor 4 - PCB 1.2
  {0x28, 0xFF, 0xCC, 0xCC, 0x3A, 0x21, 0x03, 0x33}, //Sensor 5 - PCB 1.2
  {0x28, 0xFF, 0x65, 0xDF, 0x3A, 0x21, 0x03, 0x43}, //Sensor 6 - PCB 1.2
  {0x28, 0xFF, 0x22, 0xA6, 0x3A, 0x21, 0x03, 0xC0}, //Sensor 7 - PCB 1.2
  {0x28, 0xFF, 0x85, 0xB8, 0x3A, 0x21, 0x03, 0x8F}, //Sensor 8 - PCB 1.2
  {0x28, 0xFF, 0x87, 0xAF, 0x3A, 0x21, 0x03, 0xB2}, //Sensor 9 - PCB 1.2
  {0x28, 0xFF, 0xDA, 0xC7, 0x3A, 0x21, 0x03, 0x47} //Sensor 10 - PCB 1.2
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
  Serial.println("Inicio");
  sensors.requestTemperatures();
  for (int i=0; i<tam; i++)
  {
    Tempt[i]=sensors.getTempC(address[i]);  //Intenal function of library
    Serial.println(Tempt[i]);
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
      if(module_send_message_CAN0(CANID+j, 1, 8, Enviar));
    }
}
