#line 1 "c:\\Users\\moran\\OneDrive - Universidad Pontificia Comillas\\UNI\\ICAI\\ISC RACING TEAM\\00_2022-2023\\03. Tests octubre 2022\\master_ecu\\master_ecu.h"
// CAN Telemetría config
#define CAN_TEL_CS 44
#define CAN_TEL_INT 46
#define CAN_TEL_KBPS CAN_250KBPS

// CAN Inversor config
#define CAN_INV_CS 4
#define CAN_INV_INT 5
#define CAN_INV_KBPS CAN_500KBPS

// Conversiones inversor
#define CONV_DC_BUS_VOLTAGE 55

// Sensores config
#define S1_ACELERACION_PIN A0
#define S2_ACELERACION_PIN A3
#define S_FRENO_PIN A4

#define SDD_SUSPENSION_PIN A6
#define SDI_SUSPENSION_PIN A8


// Secuencia de arranque config
#define RTDS_PIN 31
#define START_BUTTON_PIN 14

// INVERSOR
#define INV_DATA_PERIOD 0x64 // Send data each 100 ms
#define READ 0x3D // REGID for reading data from the servo and transmission to the CAN
#define RFE_RUN 0xD8 // RUN and RFE REGID
#define RFE_RUN_EN 0x30 // Si RUN (bit 4) y RFE (bit 5) están enable
#define BTB 0xE2 // Device ready
#define MODE 0x51 // Mode State
#define I_FRG 0xE8 // Digital input RUN
#define TORQUE 0x90
// Telemetry data
#define DC_BUS_VOLTAGE 0xEB // DC-Bus voltage
#define I_ACTUAL 0X5F // Filtered actual current
#define T_MOTOR 0x49 // Motor temperature
#define T_IGBT 0x4A // Power stage temperature
#define T_AIR 0x4B // Air temperature
#define N_ACTUAL 0x30 // Speed actual value

// Delays
#define DELAY_CONFIG 100
#define DELAY_CAN_SEND 0

#define UMBRAL_FRENO 210 // Valor léido del ADC a partir del cual se considera que el pedal de freno ha sido pulsado

// Periodo de recogida y envío de datos
unsigned long periodo_inv = 200; // ms
unsigned long periodo_tel = 800; // ms

// Comprobaciones y flags: 1 listo, 0 en proceso todavía
int precarga_inv = 0; //0;
int config_inv_lectura_v = 0; //0;
int ecu_telemetria = 1;
int ecu_caja_negra = 1;
int rpi_pantalla = 1;
int RTS_inv = 0; //0;
int BTB_todo = 1;
int BTB_inv_1 = 0;
int BTB_inv_2 = 0;
int boton_arranque = 0; //0

// ---------- IDs de los buses CAN ----------
// IDs CAN Inversor (definidos desde el punto de vista del inversor)
INT32U rxID_inversor = 0x201;
INT32U txID_inversor = 0x181;

// IDs CAN Telemetría Generales
INT32U ID_RTD_all = 0x80;

// IDs CAN Telemería AMS
INT32U ID_dc_bus_voltage = 0x100; // COMPROBAR CON JULIANO
INT32U ID_ack_precarga = 0x20;
INT32U ID_v_celda_min= 0x12C;

// IDs CAN Telemetría ECU_Telemetría
INT32U ID_ok_telemetria = 0xA0;
INT32U ID_ack_telemetria = 0x30;

// IDs CAN Telemetría ECU_CAJA_NEGRA
INT32U ID_ok_caja_negra = 0xB0;
INT32U ID_ack_caja_negra = 0x40;

// IDs CAN Telemetría RPI_PANTALLA
// INT32U ID_ok_pantalla = 0xE0; // COMPROBAR CON GONZALO
// INT32U ID_ack_pantalla = 0x50;

// IDs Sensores
INT32U ID_s1_aceleracion = 0x101;
INT32U ID_s2_aceleracion = 0x102;
INT32U ID_s_freno = 0x103;
INT32U ID_torque_total = 0x106; //////////////Revisar Fer
INT32U ID_velocidad = 0x104;
INT32U ID_sdi_suspension = 0x105;

// IDs Envío Datos Inversor por el CAN_TEL
INT32U ID_t_motor = 0x301;
INT32U ID_t_igbt = 0x302;
INT32U ID_t_air = 0x303;
INT32U ID_n_actual = 0x304;
