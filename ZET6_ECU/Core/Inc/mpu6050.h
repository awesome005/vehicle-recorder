#ifndef __MPU6050_H
#define __MPU6050_H

#include "main.h"
#include "i2c.h"

#define MPU6050_ADDR         0xD0
#define MPU6050_SMPLRT_DIV   0x19
#define MPU6050_CONFIG       0x1A
#define MPU6050_GYRO_CONFIG  0x1B
#define MPU6050_ACCEL_CONFIG 0x1C
#define MPU6050_ACCEL_XOUT_H 0x3B
#define MPU6050_PWR_MGMT_1   0x6B
#define MPU6050_WHO_AM_I     0x75

typedef struct {
    int16_t Accel_X, Accel_Y, Accel_Z;
    int16_t Gyro_X, Gyro_Y, Gyro_Z;
    int16_t Temp;
    float   Pitch, Roll, Yaw;
} MPU6050_Data_t;

extern MPU6050_Data_t mpu_data;

uint8_t MPU6050_Init(void);
void    MPU6050_Read_Data(void);
void    MPU6050_Calibrate_Gyro(void);
void    MPU6050_Update_Angle(float dt);

#endif


