/*
 * button.h
 *
 *  Created on: Oct 7, 2024
 *      Author: ASUS
 */

#ifndef INC_BUTTON_H_
#define INC_BUTTON_H_
#include "stm32f1xx_hal.h"

#ifndef BUTTON_PRESSED
#define BUTTON_PRESSED 1
#endif

#ifndef BUTTON_RELEASED
#define BUTTON_RELEASED 0
#endif

#ifndef NO_DEBOUNCE_SAMPLE
#define NO_DEBOUNCE_SAMPLE 4
#endif

#ifndef NO_BUTTONS
#define NO_BUTTONS 3
#endif

extern GPIO_TypeDef * ButtonPort [NO_BUTTONS];
extern int ButtonPin [NO_BUTTONS];
extern int input_buffer [NO_BUTTONS][NO_DEBOUNCE_SAMPLE];
extern int ButtonState [NO_BUTTONS];
extern int ButtonLongPressedState [NO_BUTTONS];
extern int sample;

void Button_Init(GPIO_TypeDef * Port1, int Pin1, GPIO_TypeDef * Port2, int Pin2, GPIO_TypeDef * Port3, int Pin3);
void Button_Read ();
int Is_Button_Pressed (int num);
int Is_Button_Long_Pressed (int num);

#endif /* INC_BUTTON_H_ */
