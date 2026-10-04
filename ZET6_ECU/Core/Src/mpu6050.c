#include "mpu6050.h"
#include <math.h>
#include <stdio.h>


MPU6050_Data_t mpu_data;
static float gyro_offset_x = 0, gyro_offset_y = 0, gyro_offset_z = 0;

static uint8_t MPU_WriteReg(uint8_t reg, uint8_t data)
{
    return HAL_I2C_Mem_Write(&hi2c2, MPU6050_ADDR, reg,
                             I2C_MEMADD_SIZE_8BIT, &data, 1, 100);
}

static uint8_t MPU_ReadReg(uint8_t reg, uint8_t *buf, uint16_t len)
{
    return HAL_I2C_Mem_Read(&hi2c2, MPU6050_ADDR, reg,
                            I2C_MEMADD_SIZE_8BIT, buf, len, 100);
}

uint8_t MPU6050_Init(void)
{
    uint8_t check;

    MPU_ReadReg(MPU6050_WHO_AM_I, &check, 1);
    if (check != 0x68) return 1;

    MPU_WriteReg(MPU6050_PWR_MGMT_1, 0x80);
    HAL_Delay(100);
    MPU_WriteReg(MPU6050_PWR_MGMT_1, 0x01);
    HAL_Delay(10);

    MPU_WriteReg(MPU6050_SMPLRT_DIV, 0x07);   // 125Hz
    MPU_WriteReg(MPU6050_CONFIG, 0x06);       // DLPF 5Hz
    MPU_WriteReg(MPU6050_GYRO_CONFIG, 0x18);  // ¡À2000¡ã/s
    MPU_WriteReg(MPU6050_ACCEL_CONFIG, 0x00); // ¡À2g

    MPU6050_Calibrate_Gyro();
    return 0;
}

void MPU6050_Read_Data(void)
{
    uint8_t buf[14];
    MPU_ReadReg(MPU6050_ACCEL_XOUT_H, buf, 14);

    mpu_data.Accel_X = (int16_t)((buf[0]  << 8) | buf[1]);
    mpu_data.Accel_Y = (int16_t)((buf[2]  << 8) | buf[3]);
    mpu_data.Accel_Z = (int16_t)((buf[4]  << 8) | buf[5]);
    mpu_data.Temp    = (int16_t)((buf[6]  << 8) | buf[7]);
    mpu_data.Gyro_X  = (int16_t)((buf[8]  << 8) | buf[9]);
    mpu_data.Gyro_Y  = (int16_t)((buf[10] << 8) | buf[11]);
    mpu_data.Gyro_Z  = (int16_t)((buf[12] << 8) | buf[13]);
}

void MPU6050_Calibrate_Gyro(void)
{
    int32_t sx = 0, sy = 0, sz = 0;
    uint16_t i;
    for (i = 0; i < 200; i++) {
        MPU6050_Read_Data();
        sx += mpu_data.Gyro_X;
        sy += mpu_data.Gyro_Y;
        sz += mpu_data.Gyro_Z;
        HAL_Delay(5);
    }
    gyro_offset_x = sx / 200.0f;
    gyro_offset_y = sy / 200.0f;
    gyro_offset_z = sz / 200.0f;
}

void MPU6050_Update_Angle(float dt)
{
    float ax = mpu_data.Accel_X / 16384.0f;
    float ay = mpu_data.Accel_Y / 16384.0f;
    float az = mpu_data.Accel_Z / 16384.0f;

    float gx = (mpu_data.Gyro_X - gyro_offset_x) / 16.4f;
    float gy = (mpu_data.Gyro_Y - gyro_offset_y) / 16.4f;
    float gz = (mpu_data.Gyro_Z - gyro_offset_z) / 16.4f;

    float accel_pitch = atan2f(-ax, sqrtf(ay*ay + az*az)) * 57.29578f;
    float accel_roll  = atan2f(ay, az) * 57.29578f;

    mpu_data.Pitch = 0.98f * (mpu_data.Pitch + gy * dt) + 0.02f * accel_pitch;
    mpu_data.Roll  = 0.98f * (mpu_data.Roll  + gx * dt) + 0.02f * accel_roll;
    mpu_data.Yaw  += gz * dt;
}


