#include "mcp_can.h"
#include <SPI.h>
#include <Wire.h>
#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps20.h"

// CAN Telemetría config
#define CAN_CS 4
#define CAN_INT 5
#define CAN_KBPS CAN_250KBPS

// Delays
#define DELAY_CONFIG 100
#define DELAY_CAN_SEND 10 
#define SEND_DATA_PERIOD 500//50

#define INT32U unsigned long
#define INT8U byte

// Comprobaciones y flags: 1 listo, 0 en proceso todavía
int ecu_master = 1;
int RTD_all = 1;

// ---------- IDs de los buses CAN ----------
// IDs CAN Telemetría Generales
INT32U ID_RTD_all = 0x80;

// IDs CAN Telemetría ECU_Telemetría
INT32U ID_ok_telemetria = 0xA0;
INT32U ID_ack_telemetria = 0x30;

//IDs sensores

INT32U ID_ACC_X = 0x611;
INT32U ID_ACC_Y = 0x612;
INT32U ID_ACC_Z = 0x613;
INT32U ID_GYRO_YAW = 0x614;
INT32U ID_GYRO_PITCH = 0x615;
INT32U ID_GYRO_ROLL = 0x616;



// MPU6050

//#define OUTPUT_READABLE_EULER //Euler angles (in degrees)
#define OUTPUT_READABLE_YAWPITCHROLL //yaw/pitch/roll angles (in degrees)
#define OUTPUT_READABLE_WORLDACCEL 

#define MPU_INT 2

// MPU control/status vars
bool dmpReady = false;  // set true if DMP init was successful
uint8_t mpuIntStatus;   // holds actual interrupt status byte from MPU
uint8_t devStatus;      // return status after each device operation (0 = success, !0 = error)
uint16_t packetSize;    // expected DMP packet size (default is 42 bytes)
uint16_t fifoCount;     // count of all bytes currently in FIFO
uint8_t fifoBuffer[64]; // FIFO storage buffer

// orientation/motion vars
Quaternion q;           // [w, x, y, z]         quaternion container
VectorInt16 aa;         // [x, y, z]            accel sensor measurements
VectorInt16 aaReal;     // [x, y, z]            gravity-free accel sensor measurements
VectorInt16 aaWorld;    // [x, y, z]            world-frame accel sensor measurements
VectorFloat gravity;    // [x, y, z]            gravity vector
float euler[3];         // [psi, theta, phi]    Euler angle container
float ypr[3];           // [yaw, pitch, roll]   yaw/pitch/roll container and gravity vector

float ax, ay, az, yaw, pitch, roll, psi, theta, phi;

#define DELAY_IMU 500  // Ciclo de ejecución de software en microsegundos