#include <DallasTemperature.h>
#include <OneWire.h>
#include <module_can.h>
#include <string.h>

class Temperatures {
  private:
    INT32U CANID = 0x400+40;
    float Tempt[19];
    int Tempe[19];
    int tam;
    int MAX_T;
  public:
    int Clas;
    INT8U Enviar[8]={0,0,0,0,0,0,0,0};

    Temperatures(float *Tempt, int *tempe);
    void Measure_Temp();
    void Send();
};
