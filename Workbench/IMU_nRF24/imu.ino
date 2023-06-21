
// -----------------------------------------------------------------------------
// Author       :   Ana Urquidi & Raúl Morán
// Created      :   10/10/2022
// Modified     :   18/10/2022
// Name         :   acc-gyro.ino
// Description  :
// * This file is for the MP6050 IMU
// -----------------------------------------------------------------------------

#include "imu_ecu.h"

#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
#include "Wire.h"
#endif

MPU6050 mpu;

//--------------ECU IMU MPU6050 (GYRO + ACC)--------------

MCP_CAN CAN(CAN_CS); // CAN Telemetría

INT8U ext = 0; // CAN_EXTID：Extended data frame, based on CAN 2.0B standard. ID range: 0 ~ 0x1FFFFFFF
INT8U len;     // Asignar el número de bytes (8 bits) que se transmiten.

INT8U rxBuf[8] = {0, 0, 0, 0, 0, 0, 0, 0}; // Único buffer para recibir
INT8U txBuf[8] = {0, 0, 0, 0, 0, 0, 0, 0}; // Buffer para transmitir por el CAN

INT32U rxID; // ID para recibir (único para todas las recepciones)

long TIMER_IMU, TIMER_CAN, RUN_TIME, current_time;

volatile bool mpuInterrupt = false; // indicates whether MPU interrupt pin has gone high
void dmpDataReady()
{
    mpuInterrupt = true;
}

void setup()
{

    Serial.begin(115200);
    Wire.begin();
    //------------INICIALIZACIÓN DEL CAN DE TELEMETRÍA------------

    pinMode(CAN_INT, INPUT);

    if (CAN.begin(MCP_ANY, CAN_KBPS, MCP_8MHZ) == CAN_OK)
    {
        Serial.println("CAN_TEL: Inicializacion correcta!");
        CAN.setMode(MCP_NORMAL);
    }
    else
    {
        Serial.println("CAN_TEL: Fallo al incializar, reinicie por favor");
        while (1)
            ;
    }

    // SPI.setClockDivider(SPI_CLOCK_DIV2); // Set SPI to run at 8MHz (16MHz / 2 = 8 MHz)

    // ---------- SECUENCIA DE ARRANQUE ----------

    // Calibrar sensores

    // Iniciar sensor MPU6050

    // initialize device
    Serial.println(F("Initializing I2C devices..."));
    mpu.initialize();
    pinMode(MPU_INT, INPUT);

    // verify connection
    Serial.println(F("Testing device connections..."));
    Serial.println(mpu.testConnection() ? F("MPU6050: Inicializacion correcta!") : F("MPU6050: Fallo al incializar, reinicie por favor"));

    delay(1000);

    // load and configure the DMP
    Serial.println(F("Initializing DMP..."));
    devStatus = mpu.dmpInitialize();

    if (devStatus == 0)
    {
        // Calibration Time: generate offsets and calibrate our MPU6050
        mpu.CalibrateAccel(6);
        mpu.CalibrateGyro(6);
        mpu.PrintActiveOffsets();
        // turn on the DMP, now that it's ready
        Serial.println(F("Enabling DMP..."));
        mpu.setDMPEnabled(true);

        // enable Arduino interrupt detection
        Serial.print(F("Enabling interrupt detection (Arduino external interrupt "));
        Serial.print(digitalPinToInterrupt(MPU_INT));
        Serial.println(F(")..."));
        attachInterrupt(digitalPinToInterrupt(MPU_INT), dmpDataReady, RISING);
        mpuIntStatus = mpu.getIntStatus();

        // set our DMP Ready flag so the main loop() function knows it's okay to use it
        Serial.println(F("DMP ready! Waiting for first interrupt..."));
        dmpReady = true;

        // get expected DMP packet size for later comparison
        packetSize = mpu.dmpGetFIFOPacketSize();
    }
    else
    {
        // ERROR!
        // 1 = initial memory load failed
        // 2 = DMP configuration updates failed
        // (if it's going to break, usually the code will be 1)
        Serial.print(F("DMP Initialization failed (code "));
        Serial.print(devStatus);
        Serial.println(F(")"));
    }

    // Esperamos a que el Master ECU nos pregunte si estamos listos
    while (ecu_master == 0)
    {
        if (!digitalRead(CAN_INT))
        {
            CAN.readMsgBuf(&rxID, &len, rxBuf);
            if (rxID == ID_ok_telemetria && len == 1 && rxBuf[0] == 1)
            {
                len = 1;
                txBuf[0] = 1;
                CAN.sendMsgBuf(ID_ack_telemetria, ext, len, txBuf);
                ecu_master = 1; // Telemetría arrancada
                Serial.println("Telemetria arrancada correctamente.");
            }
        }
    }

    // Esperamos hasta que el Master ECU nos mande la señal de READY-TO-DRIVE
    while (RTD_all == 0)
    {
        if (!digitalRead(CAN_INT))
        {
            CAN.readMsgBuf(&rxID, &len, rxBuf);
            if (rxID == ID_RTD_all && len == 1 && rxBuf[0] == 1)
            {
                RTD_all = 1; // Telemetría lista
                Serial.println("Telemetria RTD, enviando datos.");
            }
        }
    }

    TIMER_IMU = millis();
    TIMER_CAN = millis();
}

void loop()
{

    // Se miden aceleraciones y angulos cada 5ms y se envían por el CAN de telemetría cada 50 ms

    if (millis() - TIMER_IMU >= DELAY_IMU)
    {
        TIMER_IMU = micros();

        if (!dmpReady)
            return;
        // read a packet from FIFO
        if (mpu.dmpGetCurrentFIFOPacket(fifoBuffer))
        { // Get the Latest packet

            // display Euler angles in degrees
            mpu.dmpGetQuaternion(&q, fifoBuffer);
            mpu.dmpGetGravity(&gravity, &q);
            mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);
            Serial.print("ypr\t");
            Serial.print(ypr[0] * 180 / M_PI);
            Serial.print("\t");
            Serial.print(ypr[1] * 180 / M_PI);
            Serial.print("\t");
            Serial.println(ypr[2] * 180 / M_PI);

            // display initial world-frame acceleration, adjusted to remove gravity
            // and rotated based on known orientation from quaternion
            mpu.dmpGetQuaternion(&q, fifoBuffer);
            mpu.dmpGetAccel(&aa, fifoBuffer);
            mpu.dmpGetGravity(&gravity, &q);
            mpu.dmpGetLinearAccel(&aaReal, &aa, &gravity);
            mpu.dmpGetLinearAccelInWorld(&aaWorld, &aaReal, &q);
            // Serial.print("aworld\t");
            // Serial.print(aaWorld.x);
            // Serial.print("\t");
            // Serial.print(aaWorld.y);
            // Serial.print("\t");
            // Serial.println(aaWorld.z);
        }

        // Enviar telemetria angulos y aceleraciones

        if (millis() - TIMER_CAN >= SEND_DATA_PERIOD)
        {

            yaw = ypr[0];
            len = 2;
            txBuf[0] = ((int)yaw) >> 8;
            txBuf[1] = ((int)yaw);
            CAN.sendMsgBuf(ID_GYRO_YAW, ext, len, txBuf);

            delay(DELAY_CAN_SEND);

            pitch = ypr[1];
            len = 2;
            txBuf[0] = ((int)pitch) >> 8;
            txBuf[1] = ((int)pitch);
            CAN.sendMsgBuf(ID_GYRO_PITCH, ext, len, txBuf);

            delay(DELAY_CAN_SEND);

            roll = ypr[2];
            txBuf[0] = ((int)roll) >> 8;
            txBuf[1] = ((int)roll);
            CAN.sendMsgBuf(ID_GYRO_ROLL, ext, len, txBuf);

            delay(DELAY_CAN_SEND);

            ax = aaWorld.x;
            txBuf[0] = ((int)ax) >> 8;
            txBuf[1] = ((int)ax);
            CAN.sendMsgBuf(ID_ACC_X, ext, len, txBuf);

            delay(DELAY_CAN_SEND);

            ay = aaWorld.y;
            txBuf[0] = ((int)ay) >> 8;
            txBuf[1] = ((int)ay);
            CAN.sendMsgBuf(ID_ACC_Y, ext, len, txBuf);

            delay(DELAY_CAN_SEND);

            az = aaWorld.z;
            txBuf[0] = ((int)az) >> 8;
            txBuf[1] = ((int)az);
            CAN.sendMsgBuf(ID_ACC_Z, ext, len, txBuf);
        }
    }
}
