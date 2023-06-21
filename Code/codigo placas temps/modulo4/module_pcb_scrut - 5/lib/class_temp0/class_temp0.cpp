#include <class_temp0.h>
#include <stdio.h>

OneWire OurWire(3);
DallasTemperature sensors(&OurWire);    //Internal function of Library

/*Cada modulo está formado por dos PCBs (coinciden, es decir la 1.1 va con la 1.2, cada PCB está marcada con su número), 
y los sensores se enumeran de izquierda a derecha y de arriba a abajo, es decir el de arriba a la izquierda es el sensor 1,
el de su derecha es el 2, el de su derecha es el 3...*/
DeviceAddress address[] = {
  {0x28, 0xFF, 0xC3, 0x9C, 0x3A, 0x21, 0x03, 0x84}, //Sensor 1 - PCB 4.1
  {0x28, 0xFF, 0x3D, 0x8A, 0x3A, 0x21, 0x03, 0xB1}, //Sensor 2 - PCB 4.1
  {0x28, 0xFF, 0x25, 0xD2, 0x3A, 0x21, 0x03, 0x37}, //Sensor 3 - PCB 4.1
  {0x28, 0xFF, 0x03, 0xB6, 0x3A, 0x21, 0x03, 0xCD}, //Sensor 4 - PCB 4.1
  {0x28, 0xFF, 0xA8, 0xBB, 0x3A, 0x21, 0x03, 0x13}, //Sensor 5 - PCB 4.1
  {0x28, 0xFF, 0xC1, 0xB5, 0x3A, 0x21, 0x03, 0xE4}, //Sensor 6 - PCB 4.1
  {0x28, 0xFF, 0x23, 0xB9, 0x3A, 0x21, 0x03, 0xAF}, //Sensor 7 - PCB 4.1
  {0x28, 0xFF, 0x90, 0x97, 0x3A, 0x21, 0x03, 0xCB}, //Sensor 8 - PCB 4.1
  {0x28, 0xFF, 0xB9, 0xA8, 0x3A, 0x21, 0x03, 0x12}, //Sensor 9 - PCB 4.1

  {0x28, 0xFF, 0x1E, 0xCC, 0x3A, 0x21, 0x03, 0xEE}, //Sensor 1 - PCB 4.2
  {0x28, 0xFF, 0xCB, 0xB8, 0x3A, 0x21, 0x03, 0xC4}, //Sensor 2 - PCB 4.2
  {0x28, 0xFF, 0x5A, 0x9A, 0x3A, 0x21, 0x03, 0xC9}, //Sensor 3 - PCB 4.2
  {0x28, 0xFF, 0x27, 0xF5, 0x39, 0x21, 0x03, 0xA6}, //Sensor 4 - PCB 4.2
  {0x28, 0xFF, 0x7F, 0xC6, 0x3A, 0x21, 0x03, 0x29}, //Sensor 5 - PCB 4.2
  {0x28, 0xFF, 0xD1, 0xBC, 0x3A, 0x21, 0x03, 0x0B}, //Sensor 6 - PCB 4.2
  {0x28, 0xFF, 0x27, 0xC9, 0x3A, 0x21, 0x03, 0x18}, //Sensor 7 - PCB 4.2
  {0x28, 0xFF, 0x0A, 0xD7, 0x3A, 0x21, 0x03, 0x21}, //Sensor 8 - PCB 4.2
  {0x28, 0xFF, 0xBD, 0xDC, 0x3A, 0x21, 0x03, 0xAB}, //Sensor 9 - PCB 4.2
  {0x28, 0xFF, 0xAF, 0xE6, 0x3A, 0x21, 0x03, 0x07} //Sensor 10 - PCB 4.2    
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
