#include "dht22.h"

float g_humi = 0;
float g_temp = 0;

void DHT22_Init(void)
{
    HAL_GPIO_WritePin(DHT22_PORT, DHT22_PIN, GPIO_PIN_SET);
}

void DHT22_Task(void)
  {
    uint8_t data[5];
    uint8_t ret = DHT22_ReadData(data);

    if (ret != 0)
    {
        HAL_Delay(100);
        ret = DHT22_ReadData(data);
    }
        
    if (ret == 0)
    {
        g_humi = (data[0] << 8 | data[1]) / 10.0f;
        g_temp = (data[2] << 8 | data[3]) / 10.0f;
    }
    else
    {
        printf("DHT22 fail, err = %d\r\n", ret);
    }
  }
  

static uint8_t DHT22_Start(void)
{
    uint32_t timeout;

    HAL_GPIO_WritePin(DHT22_PORT, DHT22_PIN, GPIO_PIN_RESET);
    delay_us(1200);
    HAL_GPIO_WritePin(DHT22_PORT, DHT22_PIN, GPIO_PIN_SET);
    delay_us(30);

    timeout = 0;
    while (HAL_GPIO_ReadPin(DHT22_PORT, DHT22_PIN) == GPIO_PIN_SET)
    {
        if (++timeout > 200) return 1;
        delay_us(1);
    }

    timeout = 0;
    while (HAL_GPIO_ReadPin(DHT22_PORT, DHT22_PIN) == GPIO_PIN_RESET)
    {
        if (++timeout > 200) return 2;
        delay_us(1);
    }

    timeout = 0;
    while (HAL_GPIO_ReadPin(DHT22_PORT, DHT22_PIN) == GPIO_PIN_SET)
    {
        if (++timeout > 200) return 3;
        delay_us(1);
    }

    return 0;
}

uint8_t DHT22_ReadData(uint8_t *data)
{
    uint8_t i, j;
    uint8_t ret;
    uint32_t timeout;

    ret = DHT22_Start();
    if (ret != 0)
    {
        return ret;
    }

    for (i = 0; i < 5; i++)
    {
        data[i] = 0;
        for (j = 0; j < 8; j++)
        {
            timeout = 0;
            while (HAL_GPIO_ReadPin(DHT22_PORT, DHT22_PIN) == GPIO_PIN_RESET)
            {
                if (++timeout > 100) return 4;
                delay_us(1);
            }

            delay_us(40);

            if (HAL_GPIO_ReadPin(DHT22_PORT, DHT22_PIN) == GPIO_PIN_SET)
            {
                data[i] |= (1 << (7 - j));
            }

            timeout = 0;
            while (HAL_GPIO_ReadPin(DHT22_PORT, DHT22_PIN) == GPIO_PIN_SET)
            {
                if (++timeout > 100) return 5;
                delay_us(1);
            }
        }
    }

    if (data[4] != (uint8_t)(data[0] + data[1] + data[2] + data[3]))
    {
        return 6;
    }

    return 0;
}

