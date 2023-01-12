/*


*/

#include <Arduino.h>
#include "mainECU.h"
#include "FlexCAN_T4.h"

// ---------- MODOS DEBUG ----------
#define DEBUG true

// ---------- VARIABLES DEL CAN ----------

// Mantener dos buses CAN
// Añadir tercer bus CAN => 1 bus para real-time telemetry 1 bus para analytics
// Crear una libreria de CAN propia

FlexCAN_T4<CAN1> CAN_INV;    // CAN Inversor
FlexCAN_T4<CAN2> CAN_TEL_RT; // CAN Telemetría RT
FlexCAN_T4<CAN3> CAN_TEL_AN; // CAN Telemetría AN

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

CAN_message_t msg_rx;

// Datos a recibir del inversor
INT32U datos_inversor[N_DATOS_INV] = {T_MOTOR, T_IGBT, T_AIR, N_ACTUAL, I_ACTUAL};

// Detector de flanco botón de arranque
int start_button_act;
int start_button_ant = 1;

// ---------- VARIABLES DE LECTURA DE SENSORES ----------

// Inversor
int inv_dc_bus_voltage; // Lectura de DC_BUS_VOLTAGE
int inv_t_motor;        // Lectura de motor temperature
int inv_t_igbt;         // Lectura de power stage temperature
int inv_t_air;          // Lectura de air temperature
int inv_n_actual;       // Lectura de speed actual value

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
float v_celda_min = 3600; // Contiene el ultimo valor de tension minima de una celda enviada por el AMS.

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

void setup()
{
#if DEBUG
  Serial.begin(115200);
#endif

  // Configuración de pines -- revisar pines teensy
  pinMode(S1_ACELERACION_PIN, INPUT);
  pinMode(S2_ACELERACION_PIN, INPUT);

  pinMode(S_FRENO_PIN, INPUT);

  pinMode(SDD_SUSPENSION_PIN, INPUT);
  pinMode(SDI_SUSPENSION_PIN, INPUT);

  pinMode(CAN_TEL_INT, INPUT);
  pinMode(CAN_INV_INT, INPUT);

  pinMode(RTDS_PIN, OUTPUT);
  pinMode(START_BUTTON_PIN, INPUT);
  digitalWrite(RTDS_PIN, LOW);

  // Inicializarción de los buses CAN

  CAN_INV.begin();
  CAN_TEL_RT.begin();
  CAN_TEL_AN.begin();

  // ---------- SECUENCIA DE ARRANQUE ----------

  // Configuración para recibir el registro DC_BUS_VOLTAGE del inversor cada 100ms

  while (config_inv_lectura_v == 0)
  {
    msg_inv.id = rxID_inversor;
    msg_inv.len = 3;
    msg_inv.buf[0] = READ;
    msg_inv.buf[1] = DC_BUS_VOLTAGE;
    msg_inv.buf[2] = INV_DATA_PERIOD;

    CAN_INV.write(msg_inv);
    delay(DELAY_CONFIG);

#if DEBUG
    Serial.println("Solicitar tensión");
#endif

    // Esperar ACK inversor

    if (CAN_INV.read(msg_rx))
    {
      if (msg_rx.id == txID_inversor && msg_rx.len == 4 && msg_rx.buf[0] == DC_BUS_VOLTAGE)
      {
#if DEBUG
        Serial.println("CAN_INV: Configurado lectura de DC_BUS_VOLTAGE correctamente.");
#endif
        config_inv_lectura_v = 1; // Salimos del bucle
      }
    }
  }
#if DEBUG
  Serial.println("Pre-carga");
#endif

  // Pre-carga
  while (precarga_inv == 0)
  {

    // Leemos DC_BUS_VOLTAGE del CAN_INV
    if (CAN_INV.read(msg_rx))
    {
      if (msg_rx.id == txID_inversor && msg_rx.len == 4 && msg_rx.buf[0] == DC_BUS_VOLTAGE)
      {
        inv_dc_bus_voltage = ((int)msg_rx.buf[2] << 8 | (int)msg_rx.buf[1]) / CONV_DC_BUS_VOLTAGE;
      }
#if DEBUG
      Serial.print("DC_BUS_VOLTAGE (V): ");
      Serial.println(inv_dc_bus_voltage);
#endif

      // Reenviamos DC_BUS_VOLTAGE al AMS por CAN_TEL_AN
      msg_tel_an.id = ID_dc_bus_voltage;
      msg_tel_an.len = 2;
      msg_tel_an.buf[0] = inv_dc_bus_voltage >> 8 & 0xFF;
      msg_tel_an.buf[1] = inv_dc_bus_voltage & 0xFF;
      CAN_TEL_AN.write(msg_tel_an);
#if DEBUG
      Serial.println("CAN_TEL: DC_BUS_VOLTAGE enviado a AMS");
#endif
    }

    // Esperamos a recibir el Ok Pre-Carga del AMS
    if (CAN_TEL_AN.read(msg_rx))
    {
      if (msg_rx.id == ID_ack_precarga && msg_rx.len == 1 && msg_rx.buf[0] == 0)
      {
        precarga_inv = 1; // Precarga lista
#if DEBUG
        Serial.println("CAN_TEL_AN: Precarga correcta. OK del AMS");
#endif
      }
    }
  }

  // Comprobación ECU Telemería ON
  while (ecu_telemetria == 0)
  {
    msg_tel_an.id = ID_ok_telemetria;
    msg_tel_an.len = 1;
    msg_tel_an.buf[0] = 1;
    CAN_TEL_AN.write(msg_tel_an);

    if (CAN_TEL_AN.read(msg_rx))
    {
      if (msg_rx.id == ID_ack_telemetria && msg_rx.len == 1 && msg_rx.buf[0] == 1)
      {
        ecu_telemetria = 1; // ECU Telemetría ON
#if DEBUG
        Serial.println("CAN_TEL: ACK ECU Telemetria");
#endif
      }
    }
    delay(DELAY_CONFIG);
  }

  // Comprobación ECU Datalogger
  while (ecu_datalogger == 0)
  {
    msg_tel_an.id = ID_ok_caja_negra;
    msg_tel_an.len = 1;
    msg_tel_an.buf[0] = 1;
    CAN_TEL_AN.write(msg_tel_an);

    if (CAN_TEL_AN.read(msg_rx))
    {
      if (msg_rx.id == ID_ack_caja_negra && msg_rx.len == 1 && msg_rx.buf[0] == 1)
      {
        ecu_datalogger = 1; // ECU Caja Negra ON
#if DEBUG
        Serial.println("CAN_TEL: ACK ECU Datalogger");
#endif
      }
    }
    delay(DELAY_CONFIG);
  }

  // Comprobación si el inversor está READY-TO-START (RTS) (RFE y RUN pulsados)
  while (RTS_inv == 0)
  {
    // Pedimos el registro de RFE y RUN para comprobar si se han pulsado
    msg_inv.id = rxID_inversor;
    msg_inv.len = 3;
    msg_inv.buf[0] = READ;
    msg_inv.buf[1] = RFE_RUN;
    msg_inv.buf[2] = 0x00; // Time interval (transmitting once)
    CAN_INV.write(msg_inv);

    if (CAN_INV.read(msg_rx))
    {
      if (msg_rx.id == txID_inversor && msg_rx.len == 4 /*COMPROBAR*/ && msg_rx.buf[0] == RFE_RUN && (msg_rx.buf[1] & RFE_RUN_EN) == RFE_RUN_EN)
      {
        RTS_inv = 1;
#if DEBUG
        Serial.println("CAN_INV: Inversor RTS");
#endif
      }
    }
    delay(DELAY_CONFIG);
  }

  // Comprobar BTB Inversor (preparar para controlar por torque)
  if (BTB_todo == 0)
  {
    while (BTB_inv_1 == 0)
    {
      // Transmitting transmission request BTB
      msg_inv.id = rxID_inversor;
      msg_inv.len = 3;
      msg_inv.buf[0] = READ;
      msg_inv.buf[1] = BTB;
      msg_inv.buf[2] = 0x00;
      CAN_INV.write(msg_inv);

      // Receiving BTB 0xE2
      if (CAN_INV.read(msg_rx))
      {
        if (msg_rx.id == txID_inversor && msg_rx.len == 4 && msg_rx.buf[0] == BTB && msg_rx.buf[2] == 0x00 && msg_rx.buf[3] == 0x00)
        {
          if (msg_rx.buf[1] == 0x01)
          {
            BTB_inv_1 = 1;
#if DEBUG
            Serial.println("CAN_INV: BTB correcto, listo para arrancar.");
#endif
          }
          else
          {
#if DEBUG
            Serial.println("CAN_INV: BTB erróneo. No se puede arrancar.");
#endif
          }
        }
      }
      delay(DELAY_CONFIG);
    }

    // Transmitting disable
    msg_inv.id = rxID_inversor;
    msg_inv.len = 3;
    msg_inv.buf[0] = MODE;
    msg_inv.buf[1] = 0x04;
    msg_inv.buf[2] = 0x00;
    CAN_INV.write(msg_inv);

    // Transmitting transmission request enable (hardware)
    msg_inv.id = rxID_inversor;
    msg_inv.len = 3;
    msg_inv.buf[0] = READ;
    msg_inv.buf[1] = I_FRG;
    msg_inv.buf[2] = 0x00;
    CAN_INV.write(msg_inv);

    while (BTB_inv_2 == 0)
    {
      // Receiving enable 0xE8
      CAN_INV.read(msg_rx);
      if (msg_rx.id == txID_inversor && msg_rx.len == 4 && msg_rx.buf[0] == I_FRG && msg_rx.buf[1] == 0x01 && msg_rx.buf[2] == 0x00 && msg_rx.buf[3] == 0x00)
      {
        BTB_inv_2 = 1;
#if DEBUG
        Serial.println("CAN_INV: enable 0xE8 recibido correctamente");
#endif
      }
      delay(DELAY_CONFIG);
    }
  }

  // Transmitting no disable (enable)
  msg_inv.id = rxID_inversor;
  msg_inv.len = 3;
  msg_inv.buf[0] = MODE;
  msg_inv.buf[1] = 0x00;
  msg_inv.buf[2] = 0x00;
  CAN_INV.write(msg_inv);

// Encender LED del botón de arranque para avisar al piloto que está todo listo

// Peticiones de envío de datos de telemetría a inversor
#if DEBUG
  Serial.print("Peticiones inversor");
#endif
  for (size_t i = 0; i < N_DATOS_INV; i++)
  {
    msg_inv.id = rxID_inversor;
    msg_inv.len = 3;
    msg_inv.buf[0] = READ;
    msg_inv.buf[1] = datos_inversor[i];
    msg_inv.buf[2] = 0xFA; // Delay: 1 - 254 (0xEF) ms --> 0xFF para parar
    CAN_INV.write(msg_inv);
    delay(15);
  }
#if DEBUG
  Serial.println("RTS");
#endif

  // Esperar a que se pulse el botón de arranque mientras se pisa el freno
  while (boton_arranque == 0)
  {
    start_button_act = digitalRead(START_BUTTON_PIN);
    if (start_button_act == 0 && start_button_ant == 1)
    {
      s_freno = analogRead(S_FRENO_PIN);
#if DEBUG
      Serial.print("Freno: ");
      Serial.print(s_freno);
#endif
      if (s_freno > UMBRAL_FRENO)
      {
        boton_arranque = 1;
#if DEBUG
        Serial.println("Coche arrancado correctamente.");
#endif
      }
      else
      {
#if DEBUG
        Serial.println("Pulsar freno para arrancar");
#endif
      }
    }
  }

  // Activar READY-TO-DRIVE-SOUND (RTDS) durante un segundo y medio

#if DEBUG
  Serial.println("RTDS sonando...");
#endif
  digitalWrite(RTDS_PIN, HIGH);
  delay(2000);
  digitalWrite(RTDS_PIN, LOW);
#if DEBUG
  Serial.println("RTDS apagado");
#endif
  // Avisar a resto de ECUs de que pueden comenzar ya a mandar datos al CAN (RTD_all)
  msg_tel_an.id = ID_RTD_all;
  msg_tel_an.len = 1;
  msg_tel_an.buf[0] = 1;
  CAN_TEL_AN.write(msg_tel_an);
  delay(DELAY_CAN_SEND);
}

void loop()
{
  // put your main code here, to run repeatedly:
}