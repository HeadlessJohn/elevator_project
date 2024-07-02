#ifndef SRC_STEP_MOTOR_STEPPER_H_
#define SRC_STEP_MOTOR_STEPPER_H_

#include "main.h"
#include "stm32f4xx_hal.h"
#include "cmsis_os.h"

#define STEP_PER_REVOLUTION 4096	// 4096 Steps per revolution
#define DIR_CW              0		// Clockwise
#define DIR_CCW             1		// Counter Clockwise

// #define IN1_GPIO_PORT 		GPIOC
// #define IN1_PIN				GPIO_PIN_0
// #define IN2_GPIO_PORT 		GPIOC
// #define IN2_PIN				GPIO_PIN_1
// #define IN3_GPIO_PORT 		GPIOB
// #define IN3_PIN				GPIO_PIN_0
// #define IN4_GPIO_PORT 		GPIOA
// #define IN4_PIN				GPIO_PIN_4

static const uint8_t HALF_STEP_SEQ[9][4] = {
	{1, 0, 0, 0},
	{1, 1, 0, 0},
	{0, 1, 0, 0},
	{0, 1, 1, 0},
	{0, 0, 1, 0},
	{0, 0, 1, 1},
	{0, 0, 0, 1},
	{1, 0, 0, 1},
	{0, 0, 0, 0} // Stop
};

void step_motor(uint8_t step);
void rotate_step(uint16_t step, uint8_t dir, uint8_t enable);
void rotate_degree(uint16_t degree, uint8_t dir, uint8_t enable);
void rotate_infinite(uint8_t dir, uint8_t enable);

#endif /* SRC_STEP_MOTOR_STEPPER_H_ */