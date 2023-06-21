#include <class_temp0.h>
#include <stdio.h>

OneWire OurWire(3);
DallasTemperature sensors(&OurWire);    //Internal function of Library

/*Cada modulo está formado por dos PCBs (coinciden, es decir la 1.1 va con la 1.2, cada PCB está marcada con su número), 
y los sensores se enumeran de izquierda a derecha y de arriba a abajo, es decir el de arriba a la izquierda es el sensor 1,
el de su derecha es el 2, el de su derecha es el 3...*/
DeviceAddress address[] = {
  {0x28, 0xFF, 0x26, 0xD9, 0x3A, 0x21, 0x03, 0xED}, //Sensor 1 - PCB 3.1
  {0x28, 0xFF, 0xC8, 0xB1, 0x3A, 0x21, 0x03, 0x19}, //Sensor 2 - PCB 3.1
  {0x28, 0xFF, 0x97, 0x51, 0x3A, 0x21, 0x03, 0xAA}, //Sensor 3 - PCB 3.1
  {0x28, 0xFF, 0xD9, 0xAA, 0x3A, 0x21, 0x03, 0x04}, //Sensor 4 - PCB 3.1
  {0x28, 0xFF, 0x8D, 0x0A, 0x3A, 0x21, 0x03, 0x27}, //Sensor 5 - PCB 3.1
  {0x28, 0xFF, 0x5B, 0xCF, 0x3A, 0x21, 0x03, 0x5D}, //Sensor 6 - PCB 3.1
  {0x28, 0x22, 0x81, 0x2C, 0x0C, 0x00, 0x00, 0x19}, //Sensor 7 - PCB 3.1
  {0x28, 0xFF, 0xD2, 0xA1, 0x3A, 0x21, 0x03, 0xE0}, //Sensor 8 - PCB 3.1
  {0x28, 0xFF, 0x1A, 0xC5, 0x3A, 0x21, 0x03, 0x62}, //Sensor 9 - PCB 3.1

  {0x28, 0xFF, 0xB7, 0xCD, 0x3A, 0x21, 0x03, 0xA1}, //Sensor 1 - PCB 3.2
  {0x28, 0xFF, 0x56, 0xA0, 0x3A, 0x21, 0x03, 0xBB}, //Sensor 2 - PCB 3.2
  {0x28, 0xFF, 0x06, 0x93, 0x3A, 0x21, 0x03, 0xEE}, //Sensor 3 - PCB 3.2
  {0x28, 0xFF, 0xF9, 0x8E, 0x3A, 0x21, 0x03, 0x82}, //Sensor 4 - PCB 3.2
  {0x28, 0xFF, 0x20, 0x97, 0x3A, 0x21, 0x03, 0x84}, //Sensor 5 - PCB 3.2
  {0x28, 0xFF, 0x7B, 0xE0, 0x3A, 0x21, 0x03, 0x4F}, //Sensor 6 - PCB 3.2
  {0x28, 0xFF, 0x43, 0xA1, 0x3A, 0x21, 0x03, 0x9A}, //Sensor 7 - PCB 3.2
  {0x28, 0xFF, 0x3E, 0xBF, 0x3A, 0x21, 0x03, 0x36}, //Sensor 8 - PCB 3.2
  {0x28, 0xFF, 0x78, 0xCD, 0x3A, 0x21, 0x03, 0xEC}, //Sensor 9 - PCB 3.2
  {0x28, 0xFF, 0x8C, 0x47, 0x3A, 0x21, 0x03, 0x97} //Sensor 10 - PCB 3.2
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
