#ifndef __MY_CAN_H
#define __MY_CAN_H

#include "main.h"
#include "can.h"

#define CAN_ID_ATTITUDE   0x100
#define CAN_ID_ACCEL      0x101
#define CAN_ID_ENV        0x200
#define CAN_ID_STATE      0x300



extern uint8_t vehicle_state;


void MY_CAN_Init(void);
void MY_CAN_Send_Attitude(void);
void MY_CAN_Send_Env(void);
void MY_CAN_Send_Raw(void);
void MY_CAN_Send_State(void);
void Vehicle_State_Update(void);


#endif


