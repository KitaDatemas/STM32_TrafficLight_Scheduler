/*
 * Traffic_Light_FSM.h
 *
 *  Created on: Oct 7, 2024
 *      Author: ASUS
 */

#ifndef INC_TRAFFIC_LIGHT_FSM_H_
#define INC_TRAFFIC_LIGHT_FSM_H_
#include "main.h"
#include "button.h"
#define TIMER 1

#define BUTTON1 1
#define BUTTON2 2
#define BUTTON3 3

extern int scanCount, 
		   blinkCount;

enum FSM_Mode {RED, AMBER, GREEN, NORMAL, SLOWDOWN, MANUAL};

extern GPIO_TypeDef * LIGHT1_Port [2];
extern GPIO_TypeDef * LIGHT2_Port [2];
extern int LIGHT1_Pin [2];
extern int LIGHT2_Pin [2];

extern TIM_HandleTypeDef * tim;

extern int count;
extern int LED_Buffer [4];
extern int RED_Timer,
		   AMBER_Timer,
		   GREEN_Timer;

extern int settingBuffer[3];
extern int setFlag;

extern int redLightTimer;

extern int isLight1Manual;
extern enum FSM_Mode Mode;
extern int changeModeFlag;

enum lightState {Red = 5, Yellow = 2, Green = 3};
extern int second, startup;

struct lightStatus {
	uint8_t 			isLight1;
	enum lightState 	currentState;
};

extern struct lightStatus lightController;
void controllerInit (void);
void fetchNextState (void);
void lightOn (void);
void display_Light ();
void turnOffLight();

void Traffic_Light_FSM_Init (GPIO_TypeDef * Button1_Port, int Button1_Pin,
							 GPIO_TypeDef * Button2_Port, int Button2_Pin,
							 GPIO_TypeDef * Button3_Port, int Button3_Pin,
							 GPIO_TypeDef * LIGHT1_Sig1_Port, int LIGHT1_Sig1_Pin,
							 GPIO_TypeDef * LIGHT1_Sig2_Port, int LIGHT1_Sig2_Pin,
							 GPIO_TypeDef * LIGHT2_Sig1_Port, int LIGHT2_Sig1_Pin,
							 GPIO_TypeDef * LIGHT2_Sig2_Port, int LIGHT2_Sig2_Pin);

void Traffic_Light_FSM_Run ();

void displayMode (enum FSM_Mode mode);

void saveState ();
void updateDisplaySecond (int second);
void resetSettingBuffer ();
void modePreset (enum FSM_Mode Mode);
void manualModeInit();
void switchGreenLight ();
void switchRoute ();
void checkButton2LongPressed ();
void checkLongPressed();
void increaseTimer (int * buffer);
void traficLightFSM ();
#endif /* INC_TRAFFIC_LIGHT_FSM_H_ */
