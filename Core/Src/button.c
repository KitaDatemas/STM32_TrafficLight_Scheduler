/*
 * button.c
 *
 *  Created on: Oct 7, 2024
 *      Author: ASUS
 */
#include "button.h"
#include "stm32f1xx_hal.h"

GPIO_TypeDef * ButtonPort [NO_BUTTONS];
int ButtonPin [NO_BUTTONS],
	input_buffer [NO_BUTTONS][NO_DEBOUNCE_SAMPLE],
	ButtonState [NO_BUTTONS],
	ButtonLongPressedState [NO_BUTTONS],
	sample = 0;

void Button_Init(GPIO_TypeDef * Port1, int Pin1,
				 GPIO_TypeDef * Port2, int Pin2,
				 GPIO_TypeDef * Port3, int Pin3) {
	ButtonPort [0] = Port1;
	ButtonPin [0] = Pin1;
	ButtonPort [1] = Port2;
	ButtonPin [1] = Pin2;
	ButtonPort [2] = Port3;
	ButtonPin [2] = Pin3;

	for (int buttonIdx = 0; buttonIdx < NO_BUTTONS; buttonIdx++) {
		for (int idx = 0; idx < NO_DEBOUNCE_SAMPLE; idx++)
			input_buffer[buttonIdx][idx] = 1;
		ButtonState[buttonIdx] = BUTTON_RELEASED;
		ButtonLongPressedState[buttonIdx] = BUTTON_RELEASED;
	}
}

void Button_Read () {
	sample++;
	for (int buttonIdx = 0; buttonIdx < NO_BUTTONS; buttonIdx++){
		int offstate = 0, buttonstate = 0;

		for (int idx = 1; idx < NO_DEBOUNCE_SAMPLE; idx++) {
			input_buffer [buttonIdx] [idx - 1] = input_buffer [buttonIdx] [idx];
			offstate = (offstate << 1) | 1;
			buttonstate = (buttonstate << 1) | input_buffer [buttonIdx] [idx - 1];
		}

		input_buffer [buttonIdx] [NO_DEBOUNCE_SAMPLE - 1] = HAL_GPIO_ReadPin(ButtonPort[buttonIdx], ButtonPin[buttonIdx]);
		offstate = (offstate << 1) | 1;
		buttonstate = (buttonstate << 1) | input_buffer [buttonIdx] [NO_DEBOUNCE_SAMPLE - 1];

		if (sample >= NO_DEBOUNCE_SAMPLE) {
			if (buttonstate == offstate && ButtonState [buttonIdx] == BUTTON_PRESSED) {
				ButtonLongPressedState [buttonIdx] = BUTTON_RELEASED;
				ButtonState [buttonIdx] = BUTTON_RELEASED;
			}
			if (buttonstate == 0) {
				if (ButtonState [buttonIdx] == BUTTON_PRESSED)
					ButtonLongPressedState [buttonIdx] = BUTTON_PRESSED;
				else
					ButtonState [buttonIdx] = BUTTON_PRESSED;
			}
			if (buttonIdx == NO_BUTTONS - 1)
				sample = NO_DEBOUNCE_SAMPLE - 1;
		}
	}
}

int Is_Button_Pressed (int num) {
	if (num < 1 || num > NO_BUTTONS)		return -1;
	return ButtonState [num - 1];
}

int Is_Button_Long_Pressed (int num) {
	if (num < 1 || num > NO_BUTTONS)		return -1;
	return ButtonLongPressedState [num - 1];
}
