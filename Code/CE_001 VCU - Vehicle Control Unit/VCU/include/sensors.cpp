#include "sensors.h"
#include "mainECU.h"

void read_throtle_pedal(float* throtle_sensor_1, float* throtle_sensor_2){

    throtle_sensor_1 = analogRead(S1_ACELERACION_PIN);
    throtle_sensor_2 = analogRead(S2_ACELERACION_PIN);

}