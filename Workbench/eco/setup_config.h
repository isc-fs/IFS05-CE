// CAN config
#define CAN0Speed         CAN_500KBPS
#define CAN0IntPin_Mega   49
#define CAN0IntPin_NANO   4             //REVISAR!!
#define CAN0CS_Mega       53
#define CAN0CS_NANO       2             //REVISAR!!

#define CAN1Speed         CAN_250KBPS
//#define CAN1Speed         CAN_125KBPS
#define CAN1IntPin        48
#define CAN1CS            46 

INT8U ext = 0; // CAN_EXTID = 0：Standard data frame, based on CAN 2.0A standard. ID range: 0 ~ 0x7FF

// Señales de Control
#define START_BUTTON_ON                 2       //Boton de arranque del coche (activo a nivel alto). Hay que pulsarlo a la vez que el freno.
#define READY_TO_DRIVE_SOUND_PIN        3       //El pin con el que pasando de un 0 a un 1 suena el RTDS
#define READY_TO_DRIVE_SOUND_DURATION   3000     //Tiempo en milisegundos durante el cual se activa el RTDS
#define RESET_PIN                       5

// Sensor Parameters Mega Delantero
#define S1_THROTTLE_PIN       A5
#define S2_THROTTLE_PIN       A4
#define S1_SUSPENSION_PIN     A2
#define S2_SUSPENSION_PIN     A3
#define S_BRAKE_PIN           A1
#define S_CURRENT_PIN         A5

// Sensor Parameters NANO Trasero
#define S3_SUSPENSION_PIN     A0
#define S4_SUSPENSION_PIN     A1
#define S1_TEMPERATURE_PIN    A2
#define S2_TEMPERATURE_PIN    A3
#define S3_TEMPERATURE_PIN    A4
 
//Calibration Brake Sensor
#define S_BRAKE_ON_LIMIT      700    //Cuando se lea un valor mayor que este el programa entiende que se está pisando el freno. S_BRAKE_ON_LIMIT=Vsensor*1000/5(V)
#define S_BRAKE_LOW_LIMIT     200    //Si se lee un valor por debajo de este programa entiende que el sensor esta desconectado o roto
#define S_BRAKE_HIGH_LIMIT    1200    //Si se lee un valor por encima de este programa entiende que el sensor esta desconectado o roto

//Calibration Torque Parameters
//torque1 = (100/(S1_THROTTLE_MAX-S1_THROTTLE_MIN)*s1_throttle-(100*S1_THROTTLE_MIN)/(S1_THROTTLE_MAX-S1_THROTTLE_MIN))/TORQUE_ATTENUATION;
#define S1_THROTTLE_MAX       960
#define S1_THROTTLE_MIN       300
#define S2_THROTTLE_MAX       710
#define S2_THROTTLE_MIN       30
#define TORQUE_ATTENUATION    1
int torque1;
int torque2;

//EV2.3 APPS / Brake Pedal Plausibility Check
#define APPS_active_torque    25    //Porcentaje de torque a traves del cual se considera que se esta acelerando lo suficiente, tiene que ser equivalente a entregar 5kW al motor
int Plausibility_Check_ON=0;     //Vale uno cuando se esta comprobando que se cumple que condicion del EV2.3 durante mas de 500ms.
int Plausibility_APPS_happened=0;   //Vale uno cuando ha sucedido la condicion del EV2.3 durante mas de 500ms y por lo tanto torque=0 cuando valga uno.
int startMillisAPPS = 0;
int currentMillisAPPS = 0;
int periodoAPPS = 500;

//Variables de control
int   RTD = 1;           //cuando vale 1 significa que el inversor esta listo para mover el motor.s
int   RTT = 0;           //cuando vale 1 significa que el arduino puede enviar datos de telemetría. Vale 1 cuando se recive el ACK(RTS) del Master ECU.
int   RTS = 0;           //
int   RTDS= 0;           //uno si se quiere que la bocina suene
int   INV_REDY_FLAG=1;   //poner a 1 para no comprobar estado del inversor
int   PRE_CHARGE_ON=0;   //uno cuando esta la resistencia de precarga, cuando se encuentra en este estado el MEGA reencvia la tensión enviada leida por el inversor al BMS.


//Definición de IDs CAN Power Train
INT32U rxId;                                                 // la variable se usa para leer por referencia los ID de los mensajes que lee el modulo can
INT32U rxIdInverter = 0x201;                                 // escrito en hexadecimal!! 
INT32U txIdInverter = 0x181;                                 // NO es de 32bits es de 11bits, supongo que internamente en las funciones de la librería lo trunca


//Definición de IDs CAN Telemetry
INT32U Id_OkBMS= 0x20;                                      // Control
INT32U Id_OkMaster= 0x40;
INT32U Id_OkSensorECU= 0x30;
INT32U Id_RTS = 0x51;
INT32U Id_ACK_RTS = 0x41;
INT32U Id_ACK_TEL = 0x52;
INT32U Id_RTDMaster = 0x42;

INT32U ID_DC_BUS=0x100;                                     // Inversor
INT32U ID_Tmotor=0x101;
INT32U ID_Tigbt=0x102;
INT32U ID_Tair=0x103;
INT32U ID_RPM=0x104;

INT32U ID_Trefri_in=0x110;                                  // Sensores 
INT32U ID_Trefri_out=0x111;
INT32U ID_P_Aero_refri=0x112;
INT32U ID_Accel_x=0x130;
INT32U ID_Accel_y=0x131;
INT32U ID_Accel_z=0x132;
INT32U ID_Par_accel=0x133;
INT32U ID_P_frenos=0x134;
INT32U ID_TBat1=0x140;
INT32U ID_TBat2=0x141;
INT32U ID_TBat3=0x142;
INT32U ID_TBat4=0x143;
INT32U ID_TBat5=0x144;


//CONF. INVERTER TELEMETRY
INT32U Inv_DC_BUS_reg=0xEB;
INT32U Inv_Tmotor_reg=0x49;
INT32U Inv_Tigbt_reg=0x4a;
INT32U Inv_Tair_reg=0x4b;
INT32U Inv_SPEED_ACTUAL_reg=0x30;

INT32U CALIBRATION_DC_BUS=55;
INT32U CALIBRATION_Tmotor=1;
INT32U CALIBRATION_Tigbt=1;
INT32U CALIBRATION_Tair=1;
INT32U CALIBRATION_SPEED_ACTUAL=1;

//Variables donde se almacena los valores de telemetría enviados por el inversor
INT8U MOTOR_SPEED[8] = {0,0,0,0,0,0,0,0};                         
INT8U VOLTAGE_DC[8] = {0,0,0,0,0,0,0,0};                   

//Bufers CAN
INT8U rxBuf[8];                                              // Un único bufer para transmitir
INT8U txBuf0[8] = {0,0,0,0,0,0,0,0};                         // Bufer para transmitir por el modulo can de telematría
INT8U txBuf1[8] = {0,0,0,0,0,0,0,0};                         // Bufer para transmitir por el modulo can del Power Train
