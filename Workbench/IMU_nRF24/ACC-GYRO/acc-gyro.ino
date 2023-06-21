
// -----------------------------------------------------------------------------
// Author       :   Raúl Morán
// Created      :   10/10/2022
// Modified     :   18/10/2022
// Name         :   acc-gyro.ino
// Description  :
// * This file is for the MP6050 IMU
// -----------------------------------------------------------------------------

#include "mcp_can.h"
#include <SPI.h>
#include <Wire.h>

#include "imu_ecu.h"

//--------------ECU IMU MPU6050 (GYRO + ACC)--------------

MCP_CAN CAN(CAN_CS); // CAN Telemetría

INT8U ext = 0; // CAN_EXTID：Extended data frame, based on CAN 2.0B standard. ID range: 0 ~ 0x1FFFFFFF
INT8U len;     // Asignar el número de bytes (8 bits) que se transmiten.

INT8U rxBuf[8] = {0, 0, 0, 0, 0, 0, 0, 0}; // Único buffer para recibir
INT8U txBuf[8] = {0, 0, 0, 0, 0, 0, 0, 0}; // Buffer para transmitir por el CAN

INT32U rxID; // ID para recibir (único para todas las recepciones)

long TIMER_IMU, TIMER_CAN, RUN_TIME, current_time;

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
    MPU6050_start();

    for (cal_int = 0; cal_int < 3000; cal_int++) // Se toman 3000 muestras y se calcula el valor medio para eliminar el offset
    {
        MPU6050_read(); // Leer sensor MPU6050
        gyro_X_cal += gx;
        gyro_Y_cal += gy;
        gyro_Z_cal += gz;
        acc_X_cal += ax;
        acc_Y_cal += ay;
        acc_Z_cal += az;
        delayMicroseconds(50);
    }

    gyro_X_cal = gyro_X_cal / 3000;
    gyro_Y_cal = gyro_Y_cal / 3000;
    gyro_Z_cal = gyro_Z_cal / 3000;
    acc_X_cal = acc_X_cal / 3000;
    acc_Y_cal = acc_Y_cal / 3000;
    acc_Z_cal = acc_Z_cal / 3000;
    accCalibOK = true;

    if(accCalibOK){
        Serial.println("IMU calibrada correctamente");
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

    TIMER_IMU = micros();
    TIMER_CAN = millis();
}

void loop()
{

    // Se miden aceleraciones y angulos cada 5ms y se envían por el CAN de telemetría cada 50 ms

    if (micros() - TIMER_IMU >= DELAY_IMU)
    {

        TIMER_IMU = micros();

        MPU6050_read();
        MPU6050_process();
    }

    // Enviar telemetria angulos y aceleraciones

    if (millis() - TIMER_CAN >= SEND_DATA_PERIOD)
    {
        Serial.print("Pitch: ");
        Serial.println(angulo_pitch);
        len = 2;
        txBuf[0] = ((int)angulo_pitch) >> 8;
        txBuf[1] = ((int)angulo_pitch);
        CAN.sendMsgBuf(ID_GYRO_PITCH, ext, len, txBuf);

        delay(DELAY_CAN_SEND);

        Serial.print("Roll: ");
        Serial.println(angulo_roll);
        txBuf[0] = ((int)angulo_roll) >> 8;
        txBuf[1] = ((int)angulo_roll);
        CAN.sendMsgBuf(ID_GYRO_ROLL, ext, len, txBuf);

        delay(DELAY_CAN_SEND);

        txBuf[0] = ((int)ax) >> 8;
        txBuf[1] = ((int)ax);
        CAN.sendMsgBuf(ID_ACC_X, ext, len, txBuf);

        delay(DELAY_CAN_SEND);

        txBuf[0] = ((int)ay) >> 8;
        txBuf[1] = ((int)ay);
        CAN.sendMsgBuf(ID_ACC_Y, ext, len, txBuf);

        delay(DELAY_CAN_SEND);

        txBuf[0] = ((int)az) >> 8;
        txBuf[1] = ((int)az);
        CAN.sendMsgBuf(ID_ACC_Z, ext, len, txBuf);
    }
}

void MPU6050_start()
{
    Wire.beginTransmission(MPU6050_adress);
    Wire.write(0x6B); // Registro 6B hex)
    Wire.write(0x00); // 00000000 para activar giroscopio
    Wire.endTransmission();
    Wire.beginTransmission(MPU6050_adress);
    Wire.write(0x1B); // Register 1B hex
    Wire.write(0x08); // Giroscopio a 500dps (full scale)
    Wire.endTransmission();
    Wire.beginTransmission(MPU6050_adress);
    Wire.write(0x1C); // Register (1A hex)
    Wire.write(0x10); // Acelerómetro a  +/- 8g (full scale range)
    Wire.endTransmission();

    Wire.beginTransmission(MPU6050_adress);
    Wire.write(0x1B);
    Wire.endTransmission();
    Wire.requestFrom(MPU6050_adress, 1);
    while (Wire.available() < 1)
        ;

    // Activar y configurar filtro pasa bajos LPF que incorpora el sensor
    Wire.beginTransmission(MPU6050_adress);
    Wire.write(0x1A);
    Wire.write(0x04);
    Wire.endTransmission();
}

// Leer sensor MPU6050
void MPU6050_read()
{
    // Los datos del giroscopio y el acelerómetro se encuentran de la dirección 3B a la 14
    Wire.beginTransmission(MPU6050_adress); // Empezamos comunicación
    Wire.write(0x3B);                       // Pedir el registro 0x3B (AcX)
    Wire.endTransmission();
    Wire.requestFrom(MPU6050_adress, 14); // Solicitar un total de 14 registros
    while (Wire.available() < 14)
        ; // Esperamos hasta recibir los 14 bytes

    ax = Wire.read() << 8 | Wire.read();          // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)
    ay = Wire.read() << 8 | Wire.read();          // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
    az = Wire.read() << 8 | Wire.read();          // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
    temperature = Wire.read() << 8 | Wire.read(); // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)
    gx = Wire.read() << 8 | Wire.read();          // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
    gy = Wire.read() << 8 | Wire.read();          // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
    gz = Wire.read() << 8 | Wire.read();          // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)
}

// Cálculo de velocidad angular (º/s) y ángulo (º)
void MPU6050_process()
{

    // Restar valores de calibración del acelerómetro
    ax -= acc_X_cal;
    ay -= acc_Y_cal;
    az -= acc_Z_cal;
    az = az + 4096;

    // Restar valores de calibración del giroscopio y calcular
    // velocidad angular en º/s. Leer 65.5 en raw equivale a 1º/s
    gyro_X = (gx - gyro_X_cal) / 65.5;
    gyro_Y = (gy - gyro_Y_cal) / 65.5;
    gyro_Z = (gz - gyro_Z_cal) / 65.5;

    // Calcular ángulo de inclinación con datos del giroscopio
    // 0.000000266 = RUN_TIME / 1000 / 65.5 * PI / 180
    angulo_pitch += gyro_X * RUN_TIME / 1000;
    angulo_roll += gyro_Y * RUN_TIME / 1000;
    angulo_pitch += angulo_roll * sin((gz - gyro_Z_cal) * RUN_TIME * 0.000000266);
    angulo_roll -= angulo_pitch * sin((gz - gyro_Z_cal) * RUN_TIME * 0.000000266);

    // Calcular vector de aceleración
    // 57.2958 = Conversion de radianes a grados 180/PI
    acc_total_vector = sqrt(pow(ay, 2) + pow(ax, 2) + pow(az, 2));
    angulo_pitch_acc = asin((float)ay / acc_total_vector) * 57.2958;
    angulo_roll_acc = asin((float)ax / acc_total_vector) * -57.2958;

    // Filtro complementario
    if (set_gyro_angles)
    {
        angulo_pitch = angulo_pitch * 0.99 + angulo_pitch_acc * 0.01;
        angulo_roll = angulo_roll * 0.99 + angulo_roll_acc * 0.01;
    }
    else
    {
        angulo_pitch = angulo_pitch_acc;
        angulo_roll = angulo_roll_acc;
        set_gyro_angles = true;
    }
}