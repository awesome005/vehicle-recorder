#include "my_can.h"
#include "mpu6050.h"
#include <stdio.h>
#include <math.h>

#define STATE_NORMAL   0
#define STATE_ACCEL    1
#define STATE_BRAKE    2
#define STATE_TURN     3

#define AX_THRESHOLD   0.15f
#define AY_THRESHOLD   0.15f

extern CAN_HandleTypeDef hcan;

uint8_t vehicle_state = 0;

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

    if (HAL_CAN_ConfigFilter(&hcan, &filter) != HAL_OK) {
        printf("CAN filter config failed\r\n");
        return;
    }

    if (HAL_CAN_Start(&hcan) != HAL_OK) {
        printf("CAN start failed\r\n");
        return;
    }

    printf("MY_CAN init OK\r\n");
}

void MY_CAN_Send_Attitude(void)
{
    CAN_TxHeaderTypeDef tx;
    uint8_t data[8] = {0};
    uint32_t mailbox;

    int16_t pitch = (int16_t)(mpu_data.Pitch * 100);
    int16_t roll  = (int16_t)(mpu_data.Roll  * 100);
    int16_t yaw   = (int16_t)(mpu_data.Yaw   * 100);

    data[0] = (pitch >> 8) & 0xFF;
    data[1] = pitch & 0xFF;
    data[2] = (roll  >> 8) & 0xFF;
    data[3] = roll  & 0xFF;
    data[4] = (yaw   >> 8) & 0xFF;
    data[5] = yaw   & 0xFF;
    data[6] = 0;
    data[7] = 0;

    tx.StdId = CAN_ID_ATTITUDE;
    tx.ExtId = 0;
    tx.IDE = CAN_ID_STD;
    tx.RTR = CAN_RTR_DATA;
    tx.DLC = 8;
    tx.TransmitGlobalTime = DISABLE;

    HAL_CAN_AddTxMessage(&hcan, &tx, data, &mailbox);
}

void MY_CAN_Send_Raw(void)
{
    CAN_TxHeaderTypeDef tx;
    uint8_t data[8] = {0};
    uint32_t mailbox;

    data[0] = (mpu_data.Accel_X >> 8) & 0xFF;
    data[1] = mpu_data.Accel_X & 0xFF;
    data[2] = (mpu_data.Accel_Y >> 8) & 0xFF;
    data[3] = mpu_data.Accel_Y & 0xFF;
    data[4] = (mpu_data.Accel_Z >> 8) & 0xFF;
    data[5] = mpu_data.Accel_Z & 0xFF;

    tx.StdId = CAN_ID_ACCEL;
    tx.ExtId = 0;
    tx.IDE = CAN_ID_STD;
    tx.RTR = CAN_RTR_DATA;
    tx.DLC = 8;
    tx.TransmitGlobalTime = DISABLE;

    HAL_CAN_AddTxMessage(&hcan, &tx, data, &mailbox);
}

void MY_CAN_Send_Env(void)
{
    extern float g_temp;
    extern float g_humi;

    CAN_TxHeaderTypeDef tx;
    uint8_t data[8] = {0};
    uint32_t mailbox;

    int16_t temp = (int16_t)(g_temp * 10);
    int16_t humi = (int16_t)(g_humi * 10);

    data[0] = (temp >> 8) & 0xFF;
    data[1] = temp & 0xFF;
    data[2] = (humi >> 8) & 0xFF;
    data[3] = humi & 0xFF;

    tx.StdId = CAN_ID_ENV;
    tx.ExtId = 0;
    tx.IDE = CAN_ID_STD;
    tx.RTR = CAN_RTR_DATA;
    tx.DLC = 8;
    tx.TransmitGlobalTime = DISABLE;

    HAL_CAN_AddTxMessage(&hcan, &tx, data, &mailbox);
}

void MY_CAN_Send_State(void)
{
    CAN_TxHeaderTypeDef tx;
    uint8_t data[8] = {0};
    uint32_t mailbox;

    data[0] = vehicle_state;

    tx.StdId = CAN_ID_STATE;
    tx.ExtId = 0;
    tx.IDE = CAN_ID_STD;
    tx.RTR = CAN_RTR_DATA;
    tx.DLC = 8;
    tx.TransmitGlobalTime = DISABLE;

    HAL_CAN_AddTxMessage(&hcan, &tx, data, &mailbox);
}

void Vehicle_State_Update(void)
{
    float ax = mpu_data.Accel_X / 16384.0f;
    float ay = mpu_data.Accel_Y / 16384.0f;

    if (ax > AX_THRESHOLD)
        vehicle_state = STATE_ACCEL;
    else if (ax < -AX_THRESHOLD)
        vehicle_state = STATE_BRAKE;
    else if (fabsf(ay) > AY_THRESHOLD)
        vehicle_state = STATE_TURN;
    else
        vehicle_state = STATE_NORMAL;
}




