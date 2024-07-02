#include "stepper.h"


void step_motor(uint8_t step){
	HAL_GPIO_WritePin(IN1_GPIO_Port, IN1_Pin, HALF_STEP_SEQ[step][0]);
	HAL_GPIO_WritePin(IN2_GPIO_Port, IN2_Pin, HALF_STEP_SEQ[step][1]);
	HAL_GPIO_WritePin(IN3_GPIO_Port, IN3_Pin, HALF_STEP_SEQ[step][2]);
	HAL_GPIO_WritePin(IN4_GPIO_Port, IN4_Pin, HALF_STEP_SEQ[step][3]);
}

void rotate_step(uint16_t step, uint8_t dir, uint8_t enable){
	uint16_t i;
	for(i = 0; i < step; i++){
		if ( enable == 0 ) break;
		else{
			if(dir == DIR_CW){ // Clockwise
				step_motor( i % 8 ); // 0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, ...
			}
			else { // Counter Clockwise
				step_motor(7 - (i % 8) ); // 7, 6, 5, 4, 3, 2, 1, 0, 7, 6, 5, ...
			}
			osDelay(1);
		}
	}
	return;
}

void rotate_infinite(uint8_t dir, uint8_t enable){
	static uint8_t i;
	if ( enable == 0 ) {
		i = 0;
		step_motor(8); // Stop
		return;
	}
	else {
		if(dir == DIR_CW) { // Clockwise
			step_motor( i % 8 ); // 0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, ...
		}
		else { // Counter Clockwise
			step_motor(7 - (i % 8) ); // 7, 6, 5, 4, 3, 2, 1, 0, 7, 6, 5, ...
		}
		i++;
		osDelay(1);
	}
	return;
}

void rotate_degree(uint16_t degree, uint8_t dir, uint8_t enable){
	// 각도에 해당하는 스텝 수 계산
	uint16_t step = (uint16_t) ( ( (uint32_t)degree * STEP_PER_REVOLUTION) / 360);
	rotate_step(step, dir, enable);
}