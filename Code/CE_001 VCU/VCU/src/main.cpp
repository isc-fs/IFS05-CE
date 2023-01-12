#include <Arduino.h>
#include "mainECU.h"
#include "FlexCAN_T4.h"


// ---------- MODOS DEBUG ----------
#define DEBUG true


// ---------- VARIABLES DEL CAN ----------

// Mantener dos buses CAN
// Añadir tercer bus CAN => 1 bus para real-time telemetry 1 bus para analytics
// Crear una libreria de CAN propia

FlexCAN_T4<CAN1> CAN_INV; //CAN Inversor
FlexCAN_T4<CAN2> CAN_TEL_RT; //CAN Telemetría RT
FlexCAN_T4<CAN3> CAN_TEL_AN; //CAN Telemetría AN

/* 
INT8U ext = 0; // CAN_EXTID = 0：Standard data frame, based on CAN 2.0A standard. ID range: 0 ~ 0x7FF
INT8U len; // DLC: número de bytes que se transmiten en cada mensaje.

INT8U rxBuf[8] = {0,0,0,0,0,0,0,0}; // Único buffer para recibir
INT8U txBuf_tel[8] = {0,0,0,0,0,0,0,0}; // Buffer para transmitir por el CAN_TEL
INT8U txBuf_inv[8] = {0,0,0,0,0,0,0,0}; // Buffer para transmitir por el CAN_INV

INT32U rxID; // ID para recibir (único para todas las recepciones) */

CAN_message_t msg_inv;
CAN_message_t msg_tel_rt;
CAN_message_t msg_tel_an;

// Datos a recibir del inversor
INT32U datos_inversor[N_DATOS_INV] = {T_MOTOR,T_IGBT,T_AIR,N_ACTUAL,I_ACTUAL};

// Detector de flanco botón de arranque
int start_button_act;
int start_button_ant = 1;

// ---------- VARIABLES DE LECTURA DE SENSORES ----------

// Inversor
int inv_dc_bus_voltage; // Lectura de DC_BUS_VOLTAGE
int inv_t_motor; // Lectura de motor temperature
int inv_t_igbt; // Lectura de power stage temperature
int inv_t_air; // Lectura de air temperature
int inv_n_actual; // Lectura de speed actual value

// Coche
int s1_aceleracion; // Lectura del sensor 1 del pedal de aceleración
int s2_aceleracion; // Lectura del sensor 2 del pedal de aceleración
float s1_aceleracion_aux;
float s2_aceleracion_aux;
int s_freno; // Lectura del sensor de freno
float s_freno_aux;
int sdd_suspension; // Lectura del sensor delantero derecho de suspensión
int sdi_suspension; // Lectura del sensor delantero izquierdo de suspensión
int std_suspension; // Lectura del sensor trasero derecho de suspensión
int sti_suspension; // Lectura del sensor trasero izquierdo de suspensión
float aux_velocidad;
float v_celda_min=3600; //Contiene el ultimo valor de tension minima de una celda enviada por el AMS.

// ---------- VARIABLES DE CONTROL DEL INVERSOR ----------
int porcentaje_pedal_acel;
int torque_1;
int torque_2;
int torque_total;
int torque_limitado;
int media_s_acel;

// Revisar normativa
/* int flag_EV_2_3=0;
int flag_T11_8_9=0;
int count_T11_8_9=0;
 */

// ---------- VARIABLES DE CONTROL DEL TIEMPO ----------



//  ---------- PLAUSABILITY CHECKS ----------
/* unsigned long current_time; // Guarda el valor actual de millis()
unsigned long previous_time_inv = 0;
unsigned long previous_time_tel = 0;
int count_accel =0 ; */



void setup() {
  if 
  


}

void loop() {
  // put your main code here, to run repeatedly:
}