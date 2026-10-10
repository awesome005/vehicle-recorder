/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
#include "my_can.h"
#include "dht22.h"
#include "mpu6050.h"
#include "sensor.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
/* Definitions for MPUTask */
osThreadId_t MPUTaskHandle;
const osThreadAttr_t MPUTask_attributes = {
  .name = "MPUTask",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityHigh,
};
/* Definitions for CANTask */
osThreadId_t CANTaskHandle;
const osThreadAttr_t CANTask_attributes = {
  .name = "CANTask",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityAboveNormal,
};
/* Definitions for DHTTask */
osThreadId_t DHTTaskHandle;
const osThreadAttr_t DHTTask_attributes = {
  .name = "DHTTask",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for DEBUGTask */
osThreadId_t DEBUGTaskHandle;
const osThreadAttr_t DEBUGTask_attributes = {
  .name = "DEBUGTask",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartMPUTask(void *argument);
void StartCANTask(void *argument);
void StartDHTTask(void *argument);
void StartDEBUGTask(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of MPUTask */
  MPUTaskHandle = osThreadNew(StartMPUTask, NULL, &MPUTask_attributes);

  /* creation of CANTask */
  CANTaskHandle = osThreadNew(StartCANTask, NULL, &CANTask_attributes);

  /* creation of DHTTask */
  DHTTaskHandle = osThreadNew(StartDHTTask, NULL, &DHTTask_attributes);

  /* creation of DEBUGTask */
  DEBUGTaskHandle = osThreadNew(StartDEBUGTask, NULL, &DEBUGTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartMPUTask */
/**
  * @brief  Function implementing the MPUTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartMPUTask */
void StartMPUTask(void *argument)
{
  /* USER CODE BEGIN StartMPUTask */
  /* Infinite loop */
  for(;;)
  {
      MPU6050_Read_Data();
      MPU6050_Update_Angle(0.01f);
      Vehicle_State_Update();
      osDelay(10);
  }
  /* USER CODE END StartMPUTask */
}

/* USER CODE BEGIN Header_StartCANTask */
/**
* @brief Function implementing the CANTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartCANTask */
void StartCANTask(void *argument)
{
  /* USER CODE BEGIN StartCANTask */
  uint32_t cnt = 0;
  /* Infinite loop */
  for(;;)
  {
      MY_CAN_Send_Attitude();    // 0x100
      MY_CAN_Send_Raw();         // 0x101

      cnt++;
      if (cnt >= 5)              // 5 ¡Á 20ms = 100ms
      {
          cnt = 0;
          MY_CAN_Send_State();   // 0x300
      }

      osDelay(20);
  }
  /* USER CODE END StartCANTask */
}

/* USER CODE BEGIN Header_StartDHTTask */
/**
* @brief Function implementing the DHTTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartDHTTask */
void StartDHTTask(void *argument)
{
  /* USER CODE BEGIN StartDHTTask */
  /* Infinite loop */
  for(;;)
  {
      DHT22_Task();
      MY_CAN_Send_Env();         // 0x200
      osDelay(5000);
  }
  /* USER CODE END StartDHTTask */
}

/* USER CODE BEGIN Header_StartDEBUGTask */
/**
* @brief Function implementing the DEBUGTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartDEBUGTask */
void StartDEBUGTask(void *argument)
{
  /* USER CODE BEGIN StartDEBUGTask */
  /* Infinite loop */
  for(;;)
  {
      Sensor_Send();
      osDelay(1000);
  }
  /* USER CODE END StartDEBUGTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

