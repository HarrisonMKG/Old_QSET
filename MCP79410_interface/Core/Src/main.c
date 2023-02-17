/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <rtc.h>
#include <fonts.h>
#include <ssd1306.h>
#include "bno055_stm32.h"
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

/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 128 * 4
};
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
void StartDefaultTask(void *argument);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

struct registerData
	{
	  uint8_t registerAddress;
	  uint8_t secondsData;
	  uint8_t *pRegisterAddress;
	};

const uint8_t slaveAddressRTC = 0xDE;


typedef struct registerData registerData;

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
  MX_I2C1_Init();
  /* USER CODE BEGIN 2 */


  /*uint8_t startTimer = 0x80;
  uint8_t *pStartTimer;
  pStartTimer = &startTimer;*/

  uint8_t RTCWKDAYReg = 0x03; //Also has the battery control register at bit 3 and PWRFAIL
  uint8_t turnOnExternalBattery = 0x08;//(0x03 will set battery)
  uint8_t *pExtBatt;
  pExtBatt = &turnOnExternalBattery;

  uint8_t secondsData = 0;
  uint8_t secondsReg = 0x00;
  uint8_t *pSeconds;
  pSeconds=&secondsData;


  uint8_t minData = 0x80;
  uint8_t minReg = 0x01;
  uint8_t *pMin;
  pMin=&minData;

  uint8_t hrsData = 0;
  uint8_t hrsReg = 0x02;
  uint8_t *pHrs;
  pHrs=&hrsData;


  uint8_t alarmControlData = 0b00010000;
  uint8_t alarmControlReg = 0x07;
  uint8_t *pAlarmControl;
  pAlarmControl=&alarmControlData;

  uint8_t alarmFlagData = 0b10000000;
  uint8_t alarmFlagReg = 0x0D;
  uint8_t *pAlarmFlag;
  pAlarmFlag=&alarmFlagData;


  uint8_t alarmSecondsData = 0x07;
  uint8_t alarmSecondsReg = 0x0A;
  uint8_t *pAlarmSeconds;
  pAlarmSeconds=&alarmSecondsData;


  uint8_t alarmMinsData = 0;
  uint8_t alarmMinsReg = 0x0B;
  uint8_t *pAlarmMins;
  pAlarmMins=&alarmMinsData;

  uint8_t alarmHrsData = 0;
  uint8_t alarmHrsReg = 0x0C;
  uint8_t *pAlarmHrs;
  pAlarmHrs=&alarmHrsData;


  while(HAL_I2C_Init(&hi2c1)!=HAL_OK)
  {
	if(HAL_I2C_Init(&hi2c1)==HAL_OK) break;
  }

  uint8_t secondsOnes =0;
   uint8_t secondsTens = 0;

   uint8_t minOnes =0;
   uint8_t minTens = 0;

   uint8_t hrsOnes =0;
   uint8_t hrsTens = 0;

  /* USER CODE END 2 */

  /* Init scheduler */
  osKernelInitialize();

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
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  	  HAL_I2C_Mem_Read(&hi2c1,slaveAddressRTC,secondsReg,1,pSeconds,1,50);
  	  secondsData = secondsData | 0x80; //Starts the timer
  	  HAL_I2C_Mem_Write(&hi2c1,slaveAddressRTC,secondsReg,1,pSeconds,1,50);
  	  HAL_I2C_Mem_Read(&hi2c1,slaveAddressRTC,hrsReg,1,pHrs,1,50);
  	  hrsData = hrsData & 0b10111111;
  	  HAL_I2C_Mem_Write(&hi2c1,slaveAddressRTC,hrsReg,1,pHrs,1,50);

  	 HAL_I2C_Mem_Write(&hi2c1,slaveAddressRTC,alarmControlReg,1,pAlarmControl,1,50); //Set Alarm On
  	 HAL_I2C_Mem_Write(&hi2c1,slaveAddressRTC,alarmSecondsReg,1,pAlarmSeconds,1,50); //Set Alarm Timer
  	 HAL_I2C_Mem_Write(&hi2c1,slaveAddressRTC,alarmFlagReg,1,pAlarmFlag,1,50); //Clear Flag and set MFP HIGH when triggered
	  HAL_I2C_Mem_Read(&hi2c1,slaveAddressRTC,alarmSecondsReg,1,&alarmSecondsData,1,50);
 	  HAL_I2C_Mem_Read(&hi2c1,slaveAddressRTC,secondsReg,1,pAlarmSeconds,1,50);
 	  *pAlarmSeconds = (*pAlarmSeconds%60)+10; //Trigger alarm every 10 seconds
	  HAL_I2C_Mem_Write(&hi2c1,slaveAddressRTC,alarmSecondsReg,1,pAlarmSeconds,1,50);




  	  HAL_I2C_Mem_Write(&hi2c1,slaveAddressRTC,RTCWKDAYReg,1,pExtBatt,1,50);
		ssd1306_Init(&hi2c1);
		ssd1306_Fill(0);
		ssd1306_UpdateScreen (&hi2c1);
		char snum[5];

		bno055_assignI2C(&hi2c1);
		bno055_setup();
		bno055_setOperationModeNDOF();


  while (1)
  {
	  bno055_vector_t v = bno055_getVectorEuler();



	  HAL_I2C_Mem_Read(&hi2c1,slaveAddressRTC,alarmFlagReg,1,pAlarmFlag,1,50);

	  HAL_I2C_Mem_Read(&hi2c1,slaveAddressRTC,secondsReg,1,&secondsData,1,50);
	  secondsOnes = secondsData & 0xF;
	  secondsTens = (secondsData>>4) & 0x7;

	  HAL_Delay(100);

	  HAL_I2C_Mem_Read(&hi2c1,slaveAddressRTC,minReg,1,&minData,1,50);
	  minOnes = minData & 0xF;
	  minTens = (minData>>4) & 0x7;

	  HAL_Delay(100);


	  HAL_I2C_Mem_Read(&hi2c1,slaveAddressRTC,hrsReg,1,&hrsData,1,50);
	  hrsOnes = hrsData & 0xF;
	  hrsTens = (hrsData>>4) & 0b11;

	  HAL_Delay(100);



	  itoa( (int) v.x, snum, 10);
	  ssd1306_SetCursor(1, 1);
	  ssd1306_WriteString("Heading :  ", Font_7x10,1);
	  ssd1306_WriteString(snum, Font_7x10,1);
	  ssd1306_SetCursor(1, 15);
	  itoa( (int) v.y, snum, 10);
	  ssd1306_WriteString("Roll :  ", Font_7x10,1);
	  ssd1306_WriteString(snum, Font_7x10,1);
	  ssd1306_SetCursor(1, 30);
	  itoa( (int) v.z, snum, 10);
	  ssd1306_WriteString("Pitch :  ", Font_7x10,1);
	  ssd1306_WriteString(snum, Font_7x10,1);


	  ssd1306_SetCursor(1, 45);


	  //Hours
	  itoa(hrsTens, snum, 10);
	  ssd1306_SetCursor(1, 45);
	  ssd1306_WriteString("Time: ", Font_7x10,1);
	  ssd1306_WriteString(snum, Font_7x10,1);
	  itoa(hrsOnes, snum, 10);
	  ssd1306_WriteString(snum, Font_7x10,1);

	  //Minutes
	  itoa(minTens, snum, 10);
	  ssd1306_WriteString(" : ", Font_7x10,1);
	  ssd1306_WriteString(snum, Font_7x10,1);
	  itoa(minOnes, snum, 10);
	  ssd1306_WriteString(snum, Font_7x10,1);


	  //Seconds
	  itoa(secondsTens, snum, 10);
	  ssd1306_WriteString(" : ", Font_7x10,1);
	  ssd1306_WriteString(snum, Font_7x10,1);
	  itoa(secondsOnes, snum, 10);
	  ssd1306_WriteString(snum, Font_7x10,1);



	  if(HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_13))
	  {
          HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_0);
    	  HAL_Delay(2000);
      	  HAL_I2C_Mem_Read(&hi2c1,slaveAddressRTC,secondsReg,1,pAlarmSeconds,1,50);
      	  *pAlarmSeconds = (*pAlarmSeconds%60)+10; //Trigger alarm every 10 seconds
    	  HAL_I2C_Mem_Write(&hi2c1,slaveAddressRTC,alarmSecondsReg,1,pAlarmSeconds,1,50);
    	  HAL_I2C_Mem_Write(&hi2c1,slaveAddressRTC,alarmFlagReg,1,pAlarmFlag,1,50); //Clear Flag and set MFP HIGH when triggered
          HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_0);
	  }

	  ssd1306_UpdateScreen (&hi2c1);
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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_I2C1;
  PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_HSI;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
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
  hi2c1.Init.Timing = 0x2000090E;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_RESET);

  /*Configure GPIO pin : PC13 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : PC0 */
  GPIO_InitStruct.Pin = GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN 5 */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END 5 */
}

 /**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM1 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM1) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

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
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
