/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define SET_LIN_Pin GPIO_PIN_13
#define SET_LIN_GPIO_Port GPIOC
#define DTC_LIN_Pin GPIO_PIN_14
#define DTC_LIN_GPIO_Port GPIOC
#define DTC_LIN_EXTI_IRQn EXTI15_10_IRQn
#define DTC_MIC_Pin GPIO_PIN_15
#define DTC_MIC_GPIO_Port GPIOC
#define DTC_MIC_EXTI_IRQn EXTI15_10_IRQn
#define DTC_HP_Pin GPIO_PIN_0
#define DTC_HP_GPIO_Port GPIOC
#define DTC_LOUT_Pin GPIO_PIN_1
#define DTC_LOUT_GPIO_Port GPIOC
#define I2S2_DIN_Pin GPIO_PIN_2
#define I2S2_DIN_GPIO_Port GPIOC
#define I2S2_DOUT_Pin GPIO_PIN_3
#define I2S2_DOUT_GPIO_Port GPIOC
#define SW1_Pin GPIO_PIN_0
#define SW1_GPIO_Port GPIOA
#define SW1_EXTI_IRQn EXTI0_IRQn
#define SW2_Pin GPIO_PIN_1
#define SW2_GPIO_Port GPIOA
#define SW2_EXTI_IRQn EXTI1_IRQn
#define ADC_BAT_Pin GPIO_PIN_1
#define ADC_BAT_GPIO_Port GPIOB
#define BOOT1_Pin GPIO_PIN_2
#define BOOT1_GPIO_Port GPIOB
#define BTN_ENC2_Pin GPIO_PIN_13
#define BTN_ENC2_GPIO_Port GPIOB
#define BTN_ENC2_EXTI_IRQn EXTI15_10_IRQn
#define SET_I_LIM_Pin GPIO_PIN_14
#define SET_I_LIM_GPIO_Port GPIOB
#define LED2_Pin GPIO_PIN_15
#define LED2_GPIO_Port GPIOB
#define LED1_Pin GPIO_PIN_6
#define LED1_GPIO_Port GPIOC
#define USB_DM_Pin GPIO_PIN_11
#define USB_DM_GPIO_Port GPIOA
#define USB_DP_Pin GPIO_PIN_12
#define USB_DP_GPIO_Port GPIOA
#define BTN_ENC1_Pin GPIO_PIN_12
#define BTN_ENC1_GPIO_Port GPIOC
#define BTN_ENC1_EXTI_IRQn EXTI15_10_IRQn
#define SET_BOOT0_Pin GPIO_PIN_2
#define SET_BOOT0_GPIO_Port GPIOD
#define ENC2_B_Pin GPIO_PIN_4
#define ENC2_B_GPIO_Port GPIOB
#define ENC2_A_Pin GPIO_PIN_5
#define ENC2_A_GPIO_Port GPIOB
#define ENC1_A_Pin GPIO_PIN_6
#define ENC1_A_GPIO_Port GPIOB
#define ENC1_B_Pin GPIO_PIN_7
#define ENC1_B_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */
#define UNDER_VOLTAGE 3.3f
#define MAX_VOLTAGE 4.19f
/* each parameter has a name, unit and a value */
typedef struct{
	char name[20];
	int value;
	int min;
	int max;
	int change;	//step size
	char unit[20];
}parameter;

/* each effect has a title and several parameters */
typedef struct {
	char title[20];
	parameter par0;
	parameter par1;
	parameter par2;
	parameter par3;
}effect;

/* the statemachine for the displayed effects */
typedef enum {IN, CNT}statemachine;

int *input_source;
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
