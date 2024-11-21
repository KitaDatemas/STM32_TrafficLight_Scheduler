/*
 * Traffic_Light_FSM.c
 *
 *  Created on: Oct 7, 2024
 *      Author: ASUS
 */
#include "Traffic_Light_FSM.h"

TIM_HandleTypeDef * tim;

int init = 0;
int startup = 0;
int setFlag = 0;
int changeModeFlag = 1;
struct lightStatus lightController;
enum FSM_Mode Mode = NORMAL;
int isLight1Manual = 1;

int count = 0, led_count = 0, amber_blinking_count = 0, long_pressed_count = 0;
int milliSec = 0, second = Green;

int LED_Buffer [4];
int settingBuffer[3] = {5, 2, 3};

int RED_Timer = 5,
    AMBER_Timer = 2,
    GREEN_Timer = 3,
	redLightTimer = 5;
int longPressedID = -1;
int scanCount = 250 / TIMER,
	blinkCount = 250 / TIMER,
	amberBlinkCount = 500 / (250 / TIMER),
	longPressedCount = 250 / TIMER;

GPIO_TypeDef * LIGHT1_Port [2];
GPIO_TypeDef * LIGHT2_Port [2];
int LIGHT1_Pin [2];
int LIGHT2_Pin [2];

void Traffic_Light_FSM_Init (GPIO_TypeDef * Button1_Port, int Button1_Pin,
							 GPIO_TypeDef * Button2_Port, int Button2_Pin,
							 GPIO_TypeDef * Button3_Port, int Button3_Pin,
							 GPIO_TypeDef * LIGHT1_Sig1_Port, int LIGHT1_Sig1_Pin,
							 GPIO_TypeDef * LIGHT1_Sig2_Port, int LIGHT1_Sig2_Pin,
							 GPIO_TypeDef * LIGHT2_Sig1_Port, int LIGHT2_Sig1_Pin,
							 GPIO_TypeDef * LIGHT2_Sig2_Port, int LIGHT2_Sig2_Pin) {
	Button_Init(Button1_Port, Button1_Pin, Button2_Port, Button2_Pin, Button3_Port, Button3_Pin);

	LIGHT1_Port [0] = LIGHT1_Sig1_Port;
	LIGHT1_Port [1] = LIGHT1_Sig2_Port;
	LIGHT1_Pin [0] = LIGHT1_Sig1_Pin;
	LIGHT1_Pin [1] = LIGHT1_Sig2_Pin;

	LIGHT2_Port [0] = LIGHT2_Sig1_Port;
	LIGHT2_Port [1] = LIGHT2_Sig2_Port;
	LIGHT2_Pin [0] = LIGHT2_Sig1_Pin;
	LIGHT2_Pin [1] = LIGHT2_Sig2_Pin;

}

void Traffic_Light_FSM_Run () {
	if (!init) {
		init = 1;
		controllerInit ();
	}
	switch (Mode) {
	case NORMAL:
		if (Is_Button_Pressed (BUTTON1) && !Is_Button_Long_Pressed (BUTTON1)) {
			modePreset (RED);
			changeModeFlag = 1;
			break;
		}
		else if (Is_Button_Pressed (BUTTON2) && !Is_Button_Long_Pressed (BUTTON2)) {
			modePreset (SLOWDOWN);
			changeModeFlag = 1;
			break;
		}
		else if (Is_Button_Pressed (BUTTON3) && !Is_Button_Long_Pressed (BUTTON3)) {
			modePreset (MANUAL);
			changeModeFlag = 1;
			break;
		} else {
			display_Light ();
			return;
		}
		break;
	case RED:
		if ((Is_Button_Pressed (BUTTON1) && !Is_Button_Long_Pressed (BUTTON1)) ||
			(Is_Button_Pressed (BUTTON3) && !Is_Button_Long_Pressed (BUTTON3))) {
			if (RED_Timer != settingBuffer[RED])
				setFlag = 1;
			modePreset (AMBER);
			changeModeFlag = 1;
			break;
		}
		if (Is_Button_Pressed (BUTTON2) && !Is_Button_Long_Pressed (BUTTON2)) {
			increaseTimer(&settingBuffer[RED]);
		}
		if (Is_Button_Long_Pressed (BUTTON2)) {
			if (longPressedID == -1) {
				longPressedID = SCH_Add_Task(checkButton2LongPressed, 1400, 0);
			}
		} else {
			if (longPressedID != -1) {
				Delete_Task (longPressedID);
				longPressedID = -1;
			}
		}
		break;
	case AMBER:
		if ((Is_Button_Pressed (BUTTON1) && !Is_Button_Long_Pressed (BUTTON1)) ||
			(Is_Button_Pressed (BUTTON3) && !Is_Button_Long_Pressed (BUTTON3))) {
			if (AMBER_Timer != settingBuffer[AMBER])
				setFlag = 1;
			modePreset (GREEN);
			changeModeFlag = 1;
			break;
		}
		if (Is_Button_Pressed (BUTTON2) && !Is_Button_Long_Pressed (BUTTON2)) {
			increaseTimer (&settingBuffer[AMBER]);
		}
		if (Is_Button_Long_Pressed (BUTTON2)) {
			if (longPressedID == -1) {
				longPressedID = SCH_Add_Task(checkButton2LongPressed, 1400, 0);
			}
		} else {
			if (longPressedID != -1) {
				Delete_Task (longPressedID);
				longPressedID = -1;
			}
		}
		break;
	case GREEN:
		if (Is_Button_Pressed (BUTTON2) && !Is_Button_Long_Pressed (BUTTON2)) {
			increaseTimer (&settingBuffer[GREEN]);
		}
		if (Is_Button_Long_Pressed (BUTTON2)) {
			if (longPressedID == -1) {
				longPressedID = SCH_Add_Task(checkButton2LongPressed, 1400, 0);
			}
		} else {
			if (longPressedID != -1) {
				Delete_Task (longPressedID);
				longPressedID = -1;
			}
		}
		if ((Is_Button_Pressed (BUTTON1) && !Is_Button_Long_Pressed (BUTTON1)) ||
			(Is_Button_Pressed (BUTTON3) && !Is_Button_Long_Pressed (BUTTON3))) {
			if (GREEN_Timer != settingBuffer[GREEN])
				setFlag = 1;
			if (setFlag) {
				if (settingBuffer[RED] != settingBuffer[AMBER] + settingBuffer[GREEN]) {//Process if the change is invalid
					resetSettingBuffer ();
					modePreset (RED);
					changeModeFlag = 1;
					setFlag = 0;
					break;
				} else {
					saveState ();
					setFlag = 0;
				}
			}
			if (!setFlag) {//If we not make any change or the change is valid then enter mode normal to run
				modePreset (NORMAL);
				changeModeFlag = 1;
			}
		}
		break;
	case SLOWDOWN:
		if (Is_Button_Pressed(BUTTON2) && !Is_Button_Long_Pressed(BUTTON2)) {
			modePreset (NORMAL);
			changeModeFlag = 1;
		}
		break;
	case MANUAL:
		if (Is_Button_Pressed(BUTTON3) && !Is_Button_Long_Pressed(BUTTON3)) {
			modePreset (NORMAL);
			changeModeFlag = 1;
		}
		if (Is_Button_Pressed(BUTTON1) && !Is_Button_Long_Pressed(BUTTON1)) {
			switchRoute();
		}
		break;
	default:
		break;
	}
}

void display_Light () {
	if (milliSec >= 1000) {
		second--;
		redLightTimer--;
		milliSec = 0;

		if (second <= 0) {
		  fetchNextState();
		  switch (lightController.currentState) {
			  case Green:
				  redLightTimer = RED_Timer;
				  break;
			  default:
				  break;
		  }
		}
	}
	lightOn();
}

/*controllerInit used to reload the value of green light time and red light time
for the traffic light before enter normal mode*/
void controllerInit (void) {
	lightController.isLight1 = 1;
	lightController.currentState = Green;
	second = GREEN_Timer;
	redLightTimer = RED_Timer;
}

/*Load next state when the counter gets to zero*/
void fetchNextState (void) {
	switch (lightController.currentState) {
		case Green:
			lightController.currentState = Yellow;
			second = AMBER_Timer;
			return;
		case Yellow:
			lightController.currentState = Green;
			lightController.isLight1 = !lightController.isLight1;
			second = GREEN_Timer;
			return;
		default:
			return;
	}
}

/*Turn on lights on the traffic light*/
void lightOn (void) {
	if (lightController.isLight1) {
		switch (lightController.currentState) {
			case Green:
				HAL_GPIO_WritePin(LIGHT1_Port[0], LIGHT1_Pin[0], GPIO_PIN_SET);
				HAL_GPIO_WritePin(LIGHT1_Port[1], LIGHT1_Pin[1], GPIO_PIN_RESET);
				break;
			case Yellow:
				HAL_GPIO_WritePin(LIGHT1_Port[0], LIGHT1_Pin[0], GPIO_PIN_RESET);
				HAL_GPIO_WritePin(LIGHT1_Port[1], LIGHT1_Pin[1], GPIO_PIN_SET);
				break;
			default:
				break;
		}
		HAL_GPIO_WritePin(LIGHT2_Port[0], LIGHT2_Pin[0], GPIO_PIN_SET);
		HAL_GPIO_WritePin(LIGHT2_Port[1], LIGHT2_Pin[1], GPIO_PIN_SET);
	}
	if (!lightController.isLight1) {
		switch (lightController.currentState) {
		case Green:
				HAL_GPIO_WritePin(LIGHT2_Port[0], LIGHT2_Pin[0], GPIO_PIN_SET);
				HAL_GPIO_WritePin(LIGHT2_Port[1], LIGHT2_Pin[1], GPIO_PIN_RESET);
				break;
			case Yellow:
				HAL_GPIO_WritePin(LIGHT2_Port[0], LIGHT2_Pin[0], GPIO_PIN_RESET);
				HAL_GPIO_WritePin(LIGHT2_Port[1], LIGHT2_Pin[1], GPIO_PIN_SET);
				break;
			default:
				break;
		}
		HAL_GPIO_WritePin(LIGHT1_Port[0], LIGHT1_Pin[0], GPIO_PIN_SET);
		HAL_GPIO_WritePin(LIGHT1_Port[1], LIGHT1_Pin[1], GPIO_PIN_SET);
	}
}

/*Turn off lights on the traffic light*/
void turnOffLight(){
	HAL_GPIO_WritePin(LIGHT1_Port[0], LIGHT1_Pin[0], GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LIGHT1_Port[1], LIGHT1_Pin[1], GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LIGHT2_Port[0], LIGHT2_Pin[0], GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LIGHT2_Port[1], LIGHT2_Pin[1], GPIO_PIN_RESET);
}

/*This function is to display the correct numbers on 4 7-SEGs Leds corresponding to each mode*/
void displayMode (enum FSM_Mode mode) {
	switch (mode) {
	case RED:
		LED_Buffer [0] = 0;
		LED_Buffer [1] = 1;
		LED_Buffer [2] = settingBuffer[RED] / 10;
		LED_Buffer [3] = settingBuffer[RED] % 10;
		break;
	case AMBER:
		LED_Buffer [0] = 0;
		LED_Buffer [1] = 2;
		LED_Buffer [2] = settingBuffer[AMBER] / 10;
		LED_Buffer [3] = settingBuffer[AMBER] % 10;
		break;
	case GREEN:
		LED_Buffer [0] = 0;
		LED_Buffer [1] = 3;
		LED_Buffer [2] = settingBuffer[GREEN] / 10;
		LED_Buffer [3] = settingBuffer[GREEN] % 10;
		break;
	default:
		break;
	}
}

/*To save state when the change is valid*/
void saveState () {
	RED_Timer = settingBuffer[0];
	AMBER_Timer = settingBuffer[1];
	GREEN_Timer = settingBuffer[2];
}

/*This function is to increase and make sure if it valid*/
void increaseTimer (int * buffer) {
	if (++*buffer > 99)		*buffer = 1;
}

/*This function is to change the display second when we increase value*/
void updateDisplaySecond (int second) {
	LED_Buffer [2] = second / 10;
	LED_Buffer [3] = second % 10;
}

/*Reset the buffer if the change we made is invalid*/
void resetSettingBuffer () {
	settingBuffer[RED] = RED_Timer;
	settingBuffer[AMBER] = AMBER_Timer;
	settingBuffer[GREEN] = GREEN_Timer;
}

/*Preparing for the next state when we make state transition of FSM*/
void modePreset (enum FSM_Mode mode) {
	Mode = mode;
	if (Mode != NORMAL) {
		turnOffLight();
		if (Mode != MANUAL)
			displayMode(mode);
		else {
			isLight1Manual = 1;
			manualModeInit();
		}
	} else {
		controllerInit ();
	}
	count = 0;
}
void manualModeInit() {
	HAL_GPIO_WritePin(LIGHT1_Port[0], LIGHT1_Pin[0], GPIO_PIN_SET);
	HAL_GPIO_WritePin(LIGHT1_Port[1], LIGHT1_Pin[1], GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LIGHT2_Port[0], LIGHT2_Pin[0], GPIO_PIN_SET);
	HAL_GPIO_WritePin(LIGHT2_Port[1], LIGHT2_Pin[1], GPIO_PIN_SET);
}
void switchGreenLight () {
	if (isLight1Manual) {
		HAL_GPIO_WritePin(LIGHT1_Port[0], LIGHT1_Pin[0], GPIO_PIN_SET);
		HAL_GPIO_WritePin(LIGHT1_Port[1], LIGHT1_Pin[1], GPIO_PIN_SET);
		HAL_GPIO_WritePin(LIGHT2_Port[0], LIGHT2_Pin[0], GPIO_PIN_SET);
		HAL_GPIO_WritePin(LIGHT2_Port[1], LIGHT2_Pin[1], GPIO_PIN_RESET);
	}else {
		HAL_GPIO_WritePin(LIGHT2_Port[0], LIGHT2_Pin[0], GPIO_PIN_SET);
		HAL_GPIO_WritePin(LIGHT2_Port[1], LIGHT2_Pin[1], GPIO_PIN_SET);
		HAL_GPIO_WritePin(LIGHT1_Port[0], LIGHT1_Pin[0], GPIO_PIN_SET);
		HAL_GPIO_WritePin(LIGHT1_Port[1], LIGHT1_Pin[1], GPIO_PIN_RESET);
	}
	isLight1Manual = !isLight1Manual;
}
void switchRoute () {
	if (isLight1Manual) {
		HAL_GPIO_WritePin(LIGHT1_Port[0], LIGHT1_Pin[0], GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LIGHT1_Port[1], LIGHT1_Pin[1], GPIO_PIN_SET);
	} else {
		HAL_GPIO_WritePin(LIGHT2_Port[0], LIGHT2_Pin[0], GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LIGHT2_Port[1], LIGHT2_Pin[1], GPIO_PIN_SET);
	}
	SCH_Add_Task(switchGreenLight, 2000, 0);
}
void checkButton2LongPressed () {
	if (Is_Button_Long_Pressed(BUTTON2)) {
		longPressedID = SCH_Add_Task(checkLongPressed, 0, 330);
	}
}
void checkLongPressed() {
	switch (Mode) {
	case RED:
		increaseTimer(&settingBuffer[RED]);
		break;
	case AMBER:
		increaseTimer(&settingBuffer[AMBER]);
		break;
	case GREEN:
		increaseTimer(&settingBuffer[GREEN]);
		break;
	default:
		break;
	}
}
void traficLightFSM () {
    if (++count >= blinkCount)	{
        count = 0;
        switch (Mode) {
            case RED:
                HAL_GPIO_TogglePin (LIGHT1_Port[0], LIGHT1_Pin[0]);
                HAL_GPIO_TogglePin (LIGHT1_Port[1], LIGHT1_Pin[1]);
                HAL_GPIO_TogglePin (LIGHT2_Port[0], LIGHT2_Pin[0]);
                HAL_GPIO_TogglePin (LIGHT2_Port[1], LIGHT2_Pin[1]);
                break;
            case AMBER:
                HAL_GPIO_TogglePin (LIGHT1_Port[1], LIGHT1_Pin[1]);
                HAL_GPIO_TogglePin (LIGHT2_Port[1], LIGHT2_Pin[1]);
                break;
            case GREEN:
                HAL_GPIO_TogglePin (LIGHT1_Port[0], LIGHT1_Pin[0]);
                HAL_GPIO_TogglePin (LIGHT2_Port[0], LIGHT2_Pin[0]);
                break;
            case SLOWDOWN:
            	if (++amber_blinking_count >= amberBlinkCount) {
            		amber_blinking_count = 0;
					HAL_GPIO_TogglePin (LIGHT1_Port[1], LIGHT1_Pin[1]);
					HAL_GPIO_TogglePin (LIGHT2_Port[1], LIGHT2_Pin[1]);
            	}
                break;
            default:
                break;
        }
    }
    if (++led_count >= 4*scanCount)
    	led_count = 0;
    if (Mode == NORMAL)		milliSec += TIMER;
}
