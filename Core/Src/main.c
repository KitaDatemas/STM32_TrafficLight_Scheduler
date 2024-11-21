/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
#include "Scheduler.h"
#include "Traffic_Light_FSM.h"
#include "I2C_LCD.h"
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
I2C_HandleTypeDef hi2c1;

TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
char light1Mode[] = "G:";
char light2Mode[] = "R:";

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_TIM2_Init(void);
static void MX_TIM3_Init(void);
static void MX_I2C1_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

void printLightNotify () {
	int col = 1, row = 1;
	if (changeModeFlag) {
		char mode[] = "TIMER:";
		changeModeFlag = 0;
		clearLCD();
		switch (Mode) {
		case NORMAL:
			char text1[] = "Light 1";
			I2C_Send(&hi2c1, (0x27 << 1), 0x80 | ((row - 1)*64 + (col - 1)), 0);
			for (int i = 0; i < sizeof(text1)/sizeof(text1[0]) - 1; i++)
				I2C_Send(&hi2c1, (0x27 << 1), text1[i], RS);

			char text2[] = "Light 2";
			col = 10; row = 1;
			I2C_Send(&hi2c1, (0x27 << 1), 0x80 | ((row - 1)*64 + (col - 1)), 0);
			for (int i = 0; i < sizeof(text2)/sizeof(text2[0]) - 1; i++)
				I2C_Send(&hi2c1, (0x27 << 1), text2[i], RS);
			break;
		case RED:
			char textRED[] = "RED SETTING";
			I2C_Send(&hi2c1, (0x27 << 1), 0x80 | ((row - 1)*64 + (col - 1)), 0);
			for (int i = 0; i < sizeof(textRED)/sizeof(textRED[0]) - 1; i++)
				I2C_Send(&hi2c1, (0x27 << 1), textRED[i], RS);
			col = 1; row = 2;
			I2C_Send(&hi2c1, (0x27 << 1), 0x80 | ((row - 1)*64 + (col - 1)), 0);
			for (int i = 0; i < sizeof(mode)/sizeof(mode[0]) - 1; i++)
				I2C_Send(&hi2c1, (0x27 << 1), mode[i], RS);
			break;
		case AMBER:
			char textAMBER[] = "AMBER SETTING";
			I2C_Send(&hi2c1, (0x27 << 1), 0x80 | ((row - 1)*64 + (col - 1)), 0);
			for (int i = 0; i < sizeof(textAMBER)/sizeof(textAMBER[0]) - 1; i++)
				I2C_Send(&hi2c1, (0x27 << 1), textAMBER[i], RS);
			col = 1; row = 2;
			I2C_Send(&hi2c1, (0x27 << 1), 0x80 | ((row - 1)*64 + (col - 1)), 0);
			for (int i = 0; i < sizeof(mode)/sizeof(mode[0]) - 1; i++)
				I2C_Send(&hi2c1, (0x27 << 1), mode[i], RS);
			break;
		case GREEN:
			char textGREEN[] = "GREEN SETTING";
			I2C_Send(&hi2c1, (0x27 << 1), 0x80 | ((row - 1)*64 + (col - 1)), 0);
			for (int i = 0; i < sizeof(textGREEN)/sizeof(textGREEN[0]) - 1; i++)
				I2C_Send(&hi2c1, (0x27 << 1), textGREEN[i], RS);
			col = 1; row = 2;
			I2C_Send(&hi2c1, (0x27 << 1), 0x80 | ((row - 1)*64 + (col - 1)), 0);
			for (int i = 0; i < sizeof(mode)/sizeof(mode[0]) - 1; i++)
				I2C_Send(&hi2c1, (0x27 << 1), mode[i], RS);
			break;
		case SLOWDOWN:
			char textSLOWDOWN[] = "SLOWDOWN MODE";
			I2C_Send(&hi2c1, (0x27 << 1), 0x80 | ((row - 1)*64 + (col - 1)), 0);
			for (int i = 0; i < sizeof(textSLOWDOWN)/sizeof(textSLOWDOWN[0]) - 1; i++)
				I2C_Send(&hi2c1, (0x27 << 1), textSLOWDOWN[i], RS);
			break;
		case MANUAL:
			char textMANUAL[] = "MANUAL MODE";
			I2C_Send(&hi2c1, (0x27 << 1), 0x80 | ((row - 1)*64 + (col - 1)), 0);
			for (int i = 0; i < sizeof(textMANUAL)/sizeof(textMANUAL[0]) - 1; i++)
				I2C_Send(&hi2c1, (0x27 << 1), textMANUAL[i], RS);
			break;
		default:
			break;
		}
	}
}

void printLight1Mode () {
	if (Mode == NORMAL) {
		int col = 1, row = 2;
		I2C_Send(&hi2c1, (0x27 << 1), 0x80 | ((row - 1)*64 + (col - 1)), 0);
		for (int i = 0; i < sizeof(light1Mode)/sizeof(light1Mode[0]) - 1; i++)
			I2C_Send(&hi2c1, (0x27 << 1), light1Mode[i], RS);
	}
}
void printLight2Mode () {
	if (Mode == NORMAL) {
		int col = 10, row = 2;
		I2C_Send(&hi2c1, (0x27 << 1), 0x80 | ((row - 1)*64 + (col - 1)), 0);
		for (int i = 0; i < sizeof(light2Mode)/sizeof(light2Mode[0]) - 1; i++)
			I2C_Send(&hi2c1, (0x27 << 1), light2Mode[i], RS);
	}
}
void printLight1Timer () {
	uint8_t timer[] = "00";
	int col = 8, row = 2;
	switch (Mode){
	case NORMAL:
		if (lightController.isLight1){
			timer[0] = '0' + (second / 10);
			timer[1] = '0' + (second % 10);
		} else {
			timer[0] = '0' + (redLightTimer / 10);
			timer[1] = '0' + (redLightTimer % 10);
		}
		col = 4; row = 2;
		I2C_Send(&hi2c1, (0x27 << 1), 0x80 | ((row - 1)*64 + (col - 1)), 0);
		for (int i = 0; i < sizeof(timer)/sizeof(timer[0]) - 1; i++)
			I2C_Send(&hi2c1, (0x27 << 1), timer[i], RS);
		break;
	case RED:
		timer[0] = '0' + (settingBuffer[RED] / 10);
		timer[1] = '0' + (settingBuffer[RED] % 10);

		I2C_Send(&hi2c1, (0x27 << 1), 0x80 | ((row - 1)*64 + (col - 1)), 0);
		for (int i = 0; i < sizeof(timer)/sizeof(timer[0]) - 1; i++)
			I2C_Send(&hi2c1, (0x27 << 1), timer[i], RS);
		break;
	case AMBER:
		timer[0] = '0' + (settingBuffer[AMBER] / 10);
		timer[1] = '0' + (settingBuffer[AMBER] % 10);

		I2C_Send(&hi2c1, (0x27 << 1), 0x80 | ((row - 1)*64 + (col - 1)), 0);
		for (int i = 0; i < sizeof(timer)/sizeof(timer[0]) - 1; i++)
			I2C_Send(&hi2c1, (0x27 << 1), timer[i], RS);
		break;
	case GREEN:
		timer[0] = '0' + (settingBuffer[GREEN] / 10);
		timer[1] = '0' + (settingBuffer[GREEN] % 10);

		I2C_Send(&hi2c1, (0x27 << 1), 0x80 | ((row - 1)*64 + (col - 1)), 0);
		for (int i = 0; i < sizeof(timer)/sizeof(timer[0]) - 1; i++)
			I2C_Send(&hi2c1, (0x27 << 1), timer[i], RS);
		break;
	default:
		break;
	}
}
void printLight2Timer () {
	if (Mode == NORMAL) {
		uint8_t timer[] = "00";
		if (!lightController.isLight1){
			timer[0] = '0' + (second / 10);
			timer[1] = '0' + (second % 10);
		} else {
			timer[0] = '0' + (redLightTimer / 10);
			timer[1] = '0' + (redLightTimer % 10);
		}
		int col = 13, row = 2;
		I2C_Send(&hi2c1, (0x27 << 1), 0x80 | ((row - 1)*64 + (col - 1)), 0);
		for (int i = 0; i < sizeof(timer)/sizeof(timer[0]) - 1; i++)
			I2C_Send(&hi2c1, (0x27 << 1), timer[i], RS);
	}
}
void cursorOff() {
	setCursor(0);
}
void blinkingOff() {
	setBlinking(0);
}
void changeMode( ) {
	switch (lightController.isLight1) {
	case 1:
		if (lightController.currentState == Green)
			light1Mode[0] = 'G';
		else
			light1Mode[0] = 'A';
		light2Mode[0] = 'R';
		break;
	case 0:
		if (lightController.currentState == Green)
			light2Mode[0] = 'G';
		else
			light2Mode[0] = 'A';
		light1Mode[0] = 'R';
		break;
	default:
		break;
	}
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART2_UART_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_I2C1_Init();
  /* USER CODE BEGIN 2 */
  Traffic_Light_FSM_Init(BUTTON_1_GPIO_Port, BUTTON_1_Pin,
		  	  	  	  	 BUTTON_2_GPIO_Port, BUTTON_2_Pin,
						 BUTTON_3_GPIO_Port, BUTTON_3_Pin,
						 LIGHT1_SIG1_GPIO_Port, LIGHT1_SIG1_Pin,
						 LIGHT1_SIG2_GPIO_Port, LIGHT1_SIG2_Pin,
						 LIGHT2_SIG1_GPIO_Port, LIGHT2_SIG1_Pin,
						 LIGHT2_SIG2_GPIO_Port, LIGHT2_SIG2_Pin);
  HAL_TIM_Base_Start_IT(&htim3);
  LCD_Init(&hi2c1);
  SCH_Init();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  SCH_Add_Task(clearLCD, 0, 0);
  SCH_Add_Task(cursorOff, 0, 0);
  SCH_Add_Task(blinkingOff, 0, 0);

  SCH_Add_Task(printLightNotify, 0, 353);
//  SCH_Add_Task(printLight2, 0, 1000);
  SCH_Add_Task(printLight1Timer, 0, 243);
  SCH_Add_Task(printLight2Timer, 0, 333);
  SCH_Add_Task(printLight1Mode, 0, 300);
  SCH_Add_Task(printLight2Mode, 0, 400);

  SCH_Add_Task(changeMode, 0, 2*TIMER_CYCLE);

  SCH_Add_Task(Button_Read, 0, TIMER_CYCLE);
  SCH_Add_Task(Traffic_Light_FSM_Run, 0, TIMER_CYCLE);
  SCH_Add_Task(traficLightFSM, 0, TIMER_CYCLE);
  while (1)
  {
	  SCH_Dispatcher();
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 7999;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 9;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 23999;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 2;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, LD2_Pin|LIGHT2_SIG1_Pin|LIGHT2_SIG2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LIGHT1_SIG1_GPIO_Port, LIGHT1_SIG1_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LIGHT1_SIG2_GPIO_Port, LIGHT1_SIG2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : LD2_Pin LIGHT2_SIG1_Pin LIGHT2_SIG2_Pin */
  GPIO_InitStruct.Pin = LD2_Pin|LIGHT2_SIG1_Pin|LIGHT2_SIG2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : BUTTON_2_Pin BUTTON_1_Pin BUTTON_3_Pin */
  GPIO_InitStruct.Pin = BUTTON_2_Pin|BUTTON_1_Pin|BUTTON_3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : LIGHT1_SIG1_Pin */
  GPIO_InitStruct.Pin = LIGHT1_SIG1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LIGHT1_SIG1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : LIGHT1_SIG2_Pin */
  GPIO_InitStruct.Pin = LIGHT1_SIG2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LIGHT1_SIG2_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
