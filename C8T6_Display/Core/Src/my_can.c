#include "my_can.h"
#include <stdio.h>
#include "oled.h"

/* CAN 接收数据缓存 */
float can_pitch = 0;
float can_roll  = 0;
float can_yaw   = 0;
float can_temp   = 0;
float can_humi   = 0;
float can_ax    = 0;
float can_ay    = 0;
float can_az    = 0;
uint8_t can_state = 0;


extern CAN_HandleTypeDef hcan;


void MY_CAN_Init(void)
{
    CAN_FilterTypeDef filter;

    filter.FilterBank = 0;
    filter.FilterMode = CAN_FILTERMODE_IDMASK;
    filter.FilterScale = CAN_FILTERSCALE_32BIT;
    filter.FilterIdHigh = 0x0000;
    filter.FilterIdLow = 0x0000;
    filter.FilterMaskIdHigh = 0x0000;
    filter.FilterMaskIdLow = 0x0000;
    filter.FilterFIFOAssignment = CAN_RX_FIFO0;
    filter.FilterActivation = ENABLE;
    filter.SlaveStartFilterBank = 14;

    if (HAL_CAN_ConfigFilter(&hcan, &filter) != HAL_OK)
        {
        printf("CAN filter config failed\r\n");
        return;
        }
    if (HAL_CAN_Start(&hcan) != HAL_OK)
        {
        printf("CAN start failed\r\n");
        return;
        }
    printf("C8T6 CAN init OK\r\n");
}

void MY_CAN_Poll(void)
{
    CAN_RxHeaderTypeDef rx;
    uint8_t buf[8];

    if (HAL_CAN_GetRxFifoFillLevel(&hcan, CAN_RX_FIFO0) == 0)
        return;

    if (HAL_CAN_GetRxMessage(&hcan, CAN_RX_FIFO0, &rx, buf) != HAL_OK)
        return;

    /* 0x100：姿态 Roll/Pitch/Yaw */
    if (rx.StdId == CAN_ID_ATTITUDE && rx.DLC == 8)
    {
        int16_t pitch = (int16_t)((buf[0] << 8) | buf[1]);
        int16_t roll  = (int16_t)((buf[2] << 8) | buf[3]);
        int16_t yaw   = (int16_t)((buf[4] << 8) | buf[5]);

        can_pitch = pitch / 100.0f;
        can_roll  = roll  / 100.0f;
        can_yaw   = yaw   / 100.0f;
        
        printf("RX: P=%.2f R=%.2f Y=%.2f\r\n",
               can_pitch, can_roll, can_yaw);
    }
    /* 0x101：原始加速度 Ax/Ay/Az */
    else if (rx.StdId == CAN_ID_ACCEL && rx.DLC == 8)      // ← 加速度
    {
        int16_t ax = (int16_t)((buf[0] << 8) | buf[1]);
        int16_t ay = (int16_t)((buf[2] << 8) | buf[3]);
        int16_t az = (int16_t)((buf[4] << 8) | buf[5]);

        can_ax = ax / 16384.0f;
        can_ay = ay / 16384.0f;
        can_az = az / 16384.0f;
    }
    /* 0x200：温湿度 */
    else if (rx.StdId == CAN_ID_ENV && rx.DLC == 8)
    {
        int16_t temp = (int16_t)((buf[0] << 8) | buf[1]);
        int16_t humi = (int16_t)((buf[2] << 8) | buf[3]);

        can_temp = temp / 10.0f;
        can_humi = humi / 10.0f;

        printf("RX: T=%.1f H=%.1f\r\n", can_temp, can_humi);
    }
    /* 0x300：车辆状态 */
    else if (rx.StdId == CAN_ID_STATE && rx.DLC == 8)
    {
        can_state = buf[0];
    }
}


