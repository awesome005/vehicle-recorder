#ifndef __MY_CAN_H
#define __MY_CAN_H

#include "main.h"
#include "can.h"

#define CAN_ID_ATTITUDE   0x100
#define CAN_ID_ACCEL      0x101
#define CAN_ID_ENV        0x200
#define CAN_ID_STATE      0x300

/* 接收数据缓存 */
extern float can_pitch;
extern float can_roll;
extern float can_yaw;
extern float can_temp;
extern float can_humi;
extern float can_ax;
extern float can_ay;
extern float can_az;
extern uint8_t can_state;


void MY_CAN_Init(void);
void MY_CAN_Poll(void);

#endif


