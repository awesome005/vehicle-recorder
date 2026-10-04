#ifndef __DHT22_H
#define __DHT22_H

#include "main.h"
#include "dwt.h"
#include "stdio.h"

#define DHT22_PORT  GPIOB
#define DHT22_PIN   GPIO_PIN_0

void DHT22_Init(void);
void DHT22_Task(void);
uint8_t DHT22_ReadData(uint8_t *data);
extern float g_humi;
extern float g_temp;

#endif

