#include "sensor.h"
#include "dht22.h"
#include "usart.h"
#include "mpu6050.h"
#include <stdio.h>
#include <string.h>

extern float g_temp;
extern float g_humi;

static char buf[128];

void Sensor_Send(void)
{
    int len = 0;

    len += snprintf(buf + len, sizeof(buf) - len,
                    "Temperature:%.1f,Humidity:%.1f,",
                    g_temp, g_humi);
    
    len += snprintf(buf + len, sizeof(buf) - len,
                    "P:%.2f,R:%.2f,Y:%.2f,",
                    mpu_data.Pitch, mpu_data.Roll, mpu_data.Yaw);
    
    len += snprintf(buf + len, sizeof(buf) - len, "\r\n");

    HAL_UART_Transmit(&huart1, (uint8_t *)buf, len, 100);
}

