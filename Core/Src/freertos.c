/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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
#include <stdio.h>
#include "stepper.h"
#include "tim.h"
#include "usart.h"
#include "ssd1306.h"
#include "ssd1306_fonts.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

#define STATE_IDLE    0
#define STATE_FLOOR1  1
#define STATE_FLOOR2  2
#define STATE_FLOOR3  3
#define STATE_UP      4
#define STATE_DOWN    5
#define STATE_ARRIVED 6

#define DIR_UP    DIR_CW
#define DIR_DOWN  DIR_CCW

#ifdef __GNUC__
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

uint8_t state = STATE_IDLE;
uint8_t next_state = STATE_IDLE;
uint8_t prev_state = STATE_IDLE;
uint8_t target_floor = 1;
uint8_t current_floor = 1;

/* USER CODE END Variables */
/* Definitions for heartbeat */
osThreadId_t heartbeatHandle;
const osThreadAttr_t heartbeat_attributes = {
  .name = "heartbeat",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for task1 */
osThreadId_t task1Handle;
const osThreadAttr_t task1_attributes = {
  .name = "task1",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for task2 */
osThreadId_t task2Handle;
const osThreadAttr_t task2_attributes = {
  .name = "task2",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for task3 */
osThreadId_t task3Handle;
const osThreadAttr_t task3_attributes = {
  .name = "task3",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for task4 */
osThreadId_t task4Handle;
const osThreadAttr_t task4_attributes = {
  .name = "task4",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

PUTCHAR_PROTOTYPE{
    HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 0xFFFF);
    return ch;
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
  if(GPIO_Pin == PI_F1_Pin){
    current_floor = 1;
  }
  else if(GPIO_Pin == PI_F2_Pin){
    current_floor = 2;
  }
  else if(GPIO_Pin == PI_F3_Pin){
    current_floor = 3;
  }
  else if(GPIO_Pin == BTN_F1_Pin){
    if (target_floor == current_floor) target_floor = 1;
  }
  else if(GPIO_Pin == BTN_F2_Pin){
    if (target_floor == current_floor) target_floor = 2;
  }
  else if(GPIO_Pin == BTN_F3_Pin){
    if (target_floor == current_floor) target_floor = 3;
  }
  else{
    //
  }
  printf("Current Floor: %d, Target Floor: %d\n", current_floor, target_floor);
}

/* USER CODE END FunctionPrototypes */

void heartbeat_task(void *argument);
void thread1(void *argument);
void thread2(void *argument);
void thread3(void *argument);
void thread4(void *argument);

void door_open();

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */
  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
  htim2.Instance->CCR1 = 130;

  ssd1306_Init();
	ssd1306_Fill(Black);
	ssd1306_UpdateScreen();
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
  /* creation of heartbeat */
  heartbeatHandle = osThreadNew(heartbeat_task, NULL, &heartbeat_attributes);

  /* creation of task1 */
  task1Handle = osThreadNew(thread1, NULL, &task1_attributes);

  /* creation of task2 */
  task2Handle = osThreadNew(thread2, NULL, &task2_attributes);

  /* creation of task3 */
  task3Handle = osThreadNew(thread3, NULL, &task3_attributes);

  /* creation of task4 */
  task4Handle = osThreadNew(thread4, NULL, &task4_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  HAL_UART_Transmit(&huart1, (uint8_t *)"RTOS START\n", 11, 100);

  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_heartbeat_task */
/**
  * @brief  Heartbeat Task. Toggle LED every 500ms
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_heartbeat_task */
void heartbeat_task(void *argument)
{
  /* USER CODE BEGIN heartbeat_task */
  /* Infinite loop */
  for(;;) {
    HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
    osDelay(500);
  }
  /* USER CODE END heartbeat_task */
}

/* USER CODE BEGIN Header_thread1 */
/**
* @brief Function implementing the task1 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_thread1 */
void thread1(void *argument)
{
  /* USER CODE BEGIN thread1 */

  /* Infinite loop */
  for(;;) {
    prev_state = state;
    state = next_state;
    osDelay(10);
  }
    
  /* USER CODE END thread1 */
}

/* USER CODE BEGIN Header_thread2 */
/**
* @brief Function implementing the task2 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_thread2 */
void thread2(void *argument)
{
  /* USER CODE BEGIN thread2 */
    uint8_t dir = DIR_UP;
    uint8_t enable = 0;
    int8_t diff;
    /* Infinite loop */
  for(;;) {
    diff = target_floor - current_floor;
    switch (state) {
      case STATE_IDLE:
        enable = 0;
        if      (diff > 0) next_state = STATE_UP;
        else if (diff < 0) next_state = STATE_DOWN;
        else               next_state = STATE_IDLE;
        break; // dir doesn't matter
      case STATE_UP:     
        dir = DIR_UP;   
        enable = 1;
        if (diff == 0) next_state = STATE_ARRIVED;  
        break;
      case STATE_DOWN:   
        dir = DIR_DOWN; 
        enable = 1;  
        if (diff == 0) next_state = STATE_ARRIVED;  
        break;
      case STATE_ARRIVED:                
        enable = 0;
        if (prev_state != state) door_open(); 
        next_state = STATE_IDLE;
        break; // 도착 하면 1초 대기 
      default: break;
    }
    rotate_infinite(dir, enable);
    // osDelay(1);
  }
  /* USER CODE END thread2 */
}

/* USER CODE BEGIN Header_thread3 */
/**
* @brief Function implementing the task3 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_thread3 */
void thread3(void *argument)
{
  /* USER CODE BEGIN thread3 */
  uint8_t temp[3] = {0,};
  for(;;) {
    switch (current_floor) {
      case 1:  temp[0] = 1; temp[1] = 0; temp[2] = 0; break;
      case 2:  temp[0] = 0; temp[1] = 1; temp[2] = 0; break;
      case 3:  temp[0] = 0; temp[1] = 0; temp[2] = 1; break;
      default : break;
    }
    HAL_GPIO_WritePin(LED_F1_GPIO_Port, LED_F1_Pin, temp[0]);
    HAL_GPIO_WritePin(LED_F2_GPIO_Port, LED_F2_Pin, temp[1]);
    HAL_GPIO_WritePin(LED_F3_GPIO_Port, LED_F3_Pin, temp[2]);
    osDelay(100);
  }
  /* USER CODE END thread3 */
}

/* USER CODE BEGIN Header_thread4 */
/**
* @brief Function implementing the task4 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_thread4 */
void thread4(void *argument)
{
  /* USER CODE BEGIN thread4 */
  uint8_t temp[50];
  /* Infinite loop */
  for(;;)  {
    switch (state) {
      case STATE_IDLE:    sprintf((char*)temp, "  IDLE  ");     break;
      case STATE_UP:      sprintf((char*)temp, "   UP   ");     break;
      case STATE_DOWN:    sprintf((char*)temp, "  DOWN  ");     break;
      case STATE_ARRIVED: sprintf((char*)temp, "ARRIVED ");     break;
      default:            sprintf((char*)temp, "  IDLE  ");     break;
    }
    ssd1306_SetCursor(20, 0);
    ssd1306_WriteString((char*)temp, Font_11x18, White);
    ssd1306_SetCursor(0, 30);
    sprintf((char*)temp, "Current Floor: %d", current_floor);
    ssd1306_WriteString((char*)temp, Font_7x10, White);
    ssd1306_SetCursor(0, 50);
    sprintf((char*)temp, "Target Floor: %d", target_floor);
    ssd1306_WriteString((char*)temp, Font_7x10, White);
    ssd1306_UpdateScreen();
    osDelay(200);
  }
  /* USER CODE END thread4 */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
void door_open(){
  osDelay(1000);
  htim2.Instance->CCR1 = 80;
  osDelay(1000);
  htim2.Instance->CCR1 = 130;
  osDelay(500);
  return;
}
/* USER CODE END Application */

