#include <class_temp0.h>
#include <stdio.h>

OneWire OurWire(3);
DallasTemperature sensors(&OurWire);    //Internal function of Library

/*Cada modulo está formado por dos PCBs (coinciden, es decir la 1.1 va con la 1.2, cada PCB está marcada con su número), 
y los sensores se enumeran de izquierda a derecha y de arriba a abajo, es decir el de arriba a la izquierda es el sensor 1,
el de su derecha es el 2, el de su derecha es el 3...*/
DeviceAddress address[] = {
  {0x28, 0x4D, 0x81, 0x2C, 0x0C, 0x00, 0x00, 0xFE}, //Sensor 1 - PCB 5.1
  {0x28, 0xFF, 0xB9, 0xB6, 0x3A, 0x21, 0x03, 0x3F}, //Sensor 2 - PCB 5.1
  {0x28, 0xFF, 0x42, 0x1D, 0x3A, 0x21, 0x03, 0xD4}, //Sensor 3 - PCB 5.1
  {0x28, 0xFF, 0xAC, 0xBC, 0x3A, 0x21, 0x03, 0x8A}, //Sensor 4 - PCB 5.1
  {0x28, 0xFF, 0xA5, 0xE1, 0x3A, 0x21, 0x03, 0x3C}, //Sensor 5 - PCB 5.1
  {0x28, 0xFF, 0x48, 0xC7, 0x3A, 0x21, 0x03, 0x73}, //Sensor 6 - PCB 5.1
  {0x28, 0xFF, 0x84, 0xE9, 0x3A, 0x21, 0x03, 0x15}, //Sensor 7 - PCB 5.1 
  {0x28, 0xFF, 0x4E, 0x8D, 0x3A, 0x21, 0x03, 0x14}, //Sensor 8 - PCB 5.1
  {0x28, 0xFF, 0xBC, 0xA7, 0x3A, 0x21, 0x03, 0x5A}, //Sensor 9 - PCB 5.1

  {0x28, 0x29, 0x81, 0x2C, 0x0C, 0x00, 0x00, 0xE1}, //Sensor 1 - PCB 5.2 
  {0x28, 0x48, 0x57, 0x2C, 0x0C, 0x00, 0x00, 0xD7}, //Sensor 2 - PCB 5.2
  {0x28, 0x9E, 0x57, 0x2C, 0x0C, 0x00, 0x00, 0xA1}, //Sensor 3 - PCB 5.2
  {0x28, 0x3D, 0x57, 0x2C, 0x0C, 0x00, 0x00, 0xA4}, //Sensor 4 - PCB 5.2
  {0x28, 0xE2, 0x37, 0x2C, 0x0C, 0x00, 0x00, 0x55}, //Sensor 5 - PCB 5.2
  {0x28, 0x93, 0x57, 0x2C, 0x0C, 0x00, 0x00, 0xEB}, //Sensor 6 - PCB 5.2
  {0x28, 0x87, 0x57, 0x2C, 0x0C, 0x00, 0x00, 0x6C}, //Sensor 7 - PCB 5.2
  {0x28, 0x86, 0x57, 0x2C, 0x0C, 0x00, 0x00, 0x5B}, //Sensor 8 - PCB 5.2
  {0x28, 0xD6, 0xF1, 0x2B, 0x0C, 0x00, 0x00, 0x5C}, //Sensor 9 - PCB 5.2
  {0x28, 0xCE, 0xF1, 0x2B, 0x0C, 0x00, 0x00, 0xA6} //Sensor 10 - PCB 5.2
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
      if(module_send_message_CAN0(CANID + j, 1, 8, Enviar));
    }
}
