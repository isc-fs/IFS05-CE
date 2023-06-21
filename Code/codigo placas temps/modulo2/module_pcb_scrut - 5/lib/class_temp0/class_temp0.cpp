#include <class_temp0.h>
#include <stdio.h>

OneWire OurWire(3);
DallasTemperature sensors(&OurWire);    //Internal function of Library

/*Cada modulo está formado por dos PCBs (coinciden, es decir la 1.1 va con la 1.2, cada PCB está marcada con su número), 
y los sensores se enumeran de izquierda a derecha y de arriba a abajo, es decir el de arriba a la izquierda es el sensor 1,
el de su derecha es el 2, el de su derecha es el 3...*/
DeviceAddress address[] = {
  {0x28, 0xFF, 0x65, 0x87, 0x3A, 0x21, 0x03, 0x87}, //Sensor 1 - PCB 2.1
  {0x28, 0xFF, 0xE9, 0x87, 0x3A, 0x21, 0x03, 0x6D}, //Sensor 2 - PCB 2.1
  {0x28, 0xFF, 0xE2, 0xBA, 0x3A, 0x21, 0x03, 0xC8}, //Sensor 3 - PCB 2.1
  {0x28, 0xFF, 0x17, 0xD5, 0x3A, 0x21, 0x03, 0xB6}, //Sensor 4 - PCB 2.1
  {0x28, 0xFF, 0xB3, 0xE5, 0x3A, 0x21, 0x03, 0xD2}, //Sensor 5 - PCB 2.1
  {0x28, 0xFF, 0xC3, 0xB3, 0x3A, 0x21, 0x03, 0x6E}, //Sensor 6 - PCB 2.1
  {0x28, 0xC5, 0x47, 0x2C, 0x0C, 0x00, 0x00, 0x0B}, //Sensor 7 - PCB 2.1 
  {0x28, 0xFF, 0xA4, 0x22, 0x3A, 0x21, 0x03, 0x40}, //Sensor 8 - PCB 2.1
  {0x28, 0xFF, 0xC6, 0xBE, 0x3A, 0x21, 0x03, 0x21}, //Sensor 9 - PCB 2.1

  {0x28, 0xFF, 0x77, 0xB5, 0x3A, 0x21, 0x03, 0x37}, //Sensor 1 - PCB 2.2
  {0x28, 0xFF, 0x26, 0x92, 0x3A, 0x21, 0x03, 0x99}, //Sensor 2 - PCB 2.2
  {0x28, 0xFF, 0xA0, 0xB3, 0x3A, 0x21, 0x03, 0x31}, //Sensor 3 - PCB 2.2
  {0x28, 0xFF, 0x67, 0xC5, 0x3A, 0x21, 0x03, 0xE3}, //Sensor 4 - PCB 2.2
  {0x28, 0xFF, 0x41, 0xD4, 0x3A, 0x21, 0x03, 0x30}, //Sensor 5 - PCB 2.2
  {0x28, 0x54, 0x57, 0x2C, 0x0C, 0x00, 0x00, 0xF1}, //Sensor 6 - PCB 2.2
  {0x28, 0x45, 0xF2, 0x2B, 0x0C, 0x00, 0x00, 0xFA}, //Sensor 7 - PCB 2.2
  {0x28, 0x57, 0x38, 0x2C, 0x0C, 0x00, 0x00, 0xD6}, //Sensor 8 - PCB 2.2
  {0x28, 0xFF, 0xAF, 0xDD, 0x3A, 0x21, 0x03, 0xDB}, //Sensor 9 - PCB 2.2
  {0x28, 0xFF, 0x48, 0x88, 0x3A, 0x21, 0x03, 0x09} //Sensor 10 - PCB 2.2
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
