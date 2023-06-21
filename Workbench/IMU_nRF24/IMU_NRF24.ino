
#include <Wire.h>
#include "I2Cdev.h"
  #include "MPU6050_6Axis_MotionApps20.h"
#include <SPI.h>
#include "RF24.h"

// MPU6050 with DMP
MPU6050 mpu;
uint8_t mpuIntStatus;
uint16_t packetSize;
uint16_t fifoCount;
uint8_t fifoBuffer[64];
Quaternion q;
VectorInt16 aa;
VectorInt16 aaReal;
VectorInt16 aaWorld;
VectorFloat gravity;
float yaw, pitch, roll;

// nRF24L01
RF24 radio(9,10); // CE, CSN
const uint64_t pipe = 0xE8E8F0F0E1LL;
const uint8_t payloadSize = 24;

volatile bool mpuInterrupt = false;
void dmpDataReady() {
  mpuInterrupt = true;
}

void setup() {
  Wire.begin();
  Wire.setClock(400000); // 400 kHz I2C clock
  Serial.begin(115200);

  // Initialize MPU6050 and DMP
  mpu.initialize();
  if (!mpu.testConnection()) {
    Serial.println(F("MPU6050 not connected"));
    while (1);
  }
  mpuIntStatus = mpu.dmpInitialize();

  if (mpuIntStatus == 0) {
    mpu.setDMPEnabled(true);
    attachInterrupt(digitalPinToInterrupt(2), dmpDataReady, RISING);
    mpuIntStatus = mpu.getIntStatus();
    packetSize = mpu.dmpGetFIFOPacketSize();
  } else {
    Serial.print(F("DMP Initialization failed (code "));
    Serial.print(mpuIntStatus);
    Serial.println(F(")"));
    while (1);
  }

  // Initialize nRF24L01
  radio.begin();
  radio.setChannel(76);
  radio.setPALevel(RF24_PA_MIN);
  radio.setDataRate(RF24_1MBPS);
  radio.setAutoAck(0, 1);
  radio.setCRCLength(RF24_CRC_8);
  radio.openWritingPipe(pipe);
  radio.stopListening();
  radio.setPayloadSize(payloadSize);
}

void loop() {
  if (!mpuInterrupt && fifoCount < packetSize) {
    return;
  }

  mpuInterrupt = false;
  mpuIntStatus = mpu.getIntStatus();
  fifoCount = mpu.getFIFOCount();

  if ((mpuIntStatus & 0x10) || fifoCount == 1024) {
    mpu.resetFIFO();
  } else if (mpuIntStatus & 0x02) {
    while (fifoCount < packetSize) {
      fifoCount = mpu.getFIFOCount();
    }

    mpu.getFIFOBytes(fifoBuffer, packetSize);
    fifoCount -= packetSize;

    mpu.dmpGetQuaternion(&q, fifoBuffer);
    mpu.dmpGetAccel(&aa, fifoBuffer);
    mpu.dmpGetGravity(&gravity, &q);
    mpu.dmpGetLinearAccel(&aaReal, &aa, &gravity);
    mpu.dmpGetLinearAccelInWorld(&aaWorld, &aaReal, &q);
    mpu.dmpGetYawPitchRoll(&yaw, &pitch, &roll, &q, &gravity);

    // Prepare payload
    uint8_t payload[payloadSize];
    memcpy(payload, &aaWorld.x, 2);
    memcpy(payload + 2, &aaWorld.y, 2);
        memcpy(payload + 4, &aaWorld.z, 2);
    memcpy(payload + 6, &yaw, 4);
    memcpy(payload + 10, &pitch, 4);
    memcpy(payload + 14, &roll, 4);

    // Send data through nRF24L01
    radio.write(payload, payloadSize);

    // Print values
    Serial.print("AX: "); Serial.print(aaWorld.x);
    Serial.print(" AY: "); Serial.print(aaWorld.y);
    Serial.print(" AZ: "); Serial.print(aaWorld.z);
    Serial.print(" Yaw: "); Serial.print(yaw * 180/M_PI);
    Serial.print(" Pitch: "); Serial.print(pitch * 180/M_PI);
    Serial.print(" Roll: "); Serial.println(roll * 180/M_PI);

    delay(100); // Adjust this delay according to your requirements
}
