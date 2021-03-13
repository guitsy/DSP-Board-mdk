/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "usb_device.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "ssd1306.h"
#include "tlv320aic.h"
#include "dsp_board_bsp.h"
#include "dsp_processing.h"

#include "usbd_core.h"
#include "usbd_desc.h"
#include "usbd_audio.h"
#include "usbd_audio_if.h"


/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
// #define ENABLE_PRINTF
#define HAL_PWR_MODULE_ENABLED
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;

I2C_HandleTypeDef hi2c1;
I2C_HandleTypeDef hi2c3;

I2S_HandleTypeDef hi2s2;
DMA_HandleTypeDef hdma_i2s2_ext_rx;
DMA_HandleTypeDef hdma_spi2_tx;

RNG_HandleTypeDef hrng;

TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim4;

UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */
//SAI_HandleTypeDef haudio_out_sai;
USBD_HandleTypeDef USBD_Device;
//AUDIO_STATUS_TypeDef audio_status;

SSD1306_t holedR;
SSD1306_t holedL;

uint16_t pTxData[DSP_BUFFERSIZE_DOUBLE];
uint16_t pRxData[DSP_BUFFERSIZE_DOUBLE];
uint16_t sinWave[256];

volatile uint8_t btnLeftPressed = 0;
volatile uint8_t btnRightPressed = 0;
volatile uint8_t encLeftPressed = 0;
volatile uint8_t encRightPressed = 0;
volatile uint8_t dmaTransferComplete = 0;

int16_t encoder_change=0;
char lcd_buf[64];
uint8_t level=0; //akku level state
parameter *para0,*para1,*para2,*para3;
statemachine state_crrnt=IN;
statemachine state_nxt=IN;
int *volume_line;
int *volume_hp;
effect In;

extern volatile uint16_t dsp_mode;
extern volatile uint32_t buffer_offset;

/* Define external variables*/

USBD_AUDIO_ItfTypeDef audio_class_interface;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_I2C1_Init(void);
static void MX_I2C3_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_I2S2_Init(void);
static void MX_TIM3_Init(void);
static void MX_TIM4_Init(void);
static void MX_RNG_Init(void);
static void MX_ADC1_Init(void);
/* USER CODE BEGIN PFP */
void showFX(effect *fx);
void updateAkku(void);
void clearFX(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
/* PRINTF REDIRECT to UART BEGIN */
/** @see    http://www.keil.com/forum/60531/ */
/** @see    https://stackoverflow.com/questions/45535126/stm32-printf-redirect */

struct __FILE{
  int handle;
  /* Whatever you require here. If the only file you are using is */
  /* standard output using printf() for debugging, no file handling */
  /* is required. */
};

FILE __stdout;

int fputc(int ch, FILE *f){
	HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 0xFFFF);
  return ch;
}

//int	 ferror(FILE *){
//  /* Your implementation of ferror(). */
//  return 0;
//}
/* PRINTF REDIRECT to UART END */

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
	/* Init Device Library */
	USBD_Init(&USBD_Device, &FS_Desc, 0);

	/* Add Supported Class */
	USBD_RegisterClass(&USBD_Device, USBD_AUDIO_CLASS);

	/* Add Interface callbacks for AUDIO Class */
	USBD_AUDIO_RegisterInterface(&USBD_Device, &audio_class_interface);

	/* Start Device Process */
	USBD_Start(&USBD_Device);

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_I2C1_Init();
  MX_I2C3_Init();
  MX_USART1_UART_Init();
  MX_I2S2_Init();
  MX_TIM3_Init();
  MX_TIM4_Init();
  MX_RNG_Init();
  MX_ADC1_Init();
  MX_USB_DEVICE_Init();
  /* USER CODE BEGIN 2 */

   /* Set up SSD1306 OLED Displays on both I2C Busses */
 	holedR.hi2cx = &hi2c3;
 	holedL.hi2cx = &hi2c1;
 	ssd1306_Init(&holedL);
 	ssd1306_Fill(&holedL, Black);
 	ssd1306_DrawHLine(&holedL,0,128,13,White);
 	ssd1306_DrawHLine(&holedL,0,128,48,White);
 	ssd1306_UpdateScreen(&holedL);
 	ssd1306_Init(&holedR);
 	ssd1306_Fill(&holedR, Black);
 	ssd1306_DrawHLine(&holedR,0,128,13,White);
 	ssd1306_DrawHLine(&holedR,0,128,48,White);
 	ssd1306_UpdateScreen(&holedR);

   /* If User Button is pressed on Startup, enter DFU Firmware Upgrade Mode */
 	if(HAL_GPIO_ReadPin(SW2_GPIO_Port, SW2_Pin) == GPIO_PIN_SET){
 		HAL_GPIO_WritePin(SET_BOOT0_GPIO_Port, SET_BOOT0_Pin, GPIO_PIN_SET);  // pull BOOT0 = 1
 		ssd1306_SetCursor(&holedL, 48, 23);
 		ssd1306_WriteString(&holedL, "DFU", Font_11x18, White);
 		ssd1306_UpdateScreen(&holedL);
 		ssd1306_SetCursor(&holedR, 42, 23);
 		ssd1306_WriteString(&holedR, "MODE", Font_11x18, White);
 		ssd1306_UpdateScreen(&holedR);
 		HAL_Delay(500);      // wait for Capacitor to charge to ~3.3V
 		NVIC_SystemReset();  // Reset the MCU
 	}else{
 		ssd1306_SetCursor(&holedL, 48, 23);
 		ssd1306_WriteString(&holedL, "DSP", Font_11x18, White);
 		ssd1306_UpdateScreen(&holedL);
 		ssd1306_SetCursor(&holedR, 37, 23);
 		ssd1306_WriteString(&holedR, "BOARD", Font_11x18, White);
 		ssd1306_UpdateScreen(&holedR);
 	}

 	/* Start ADC for Battery Voltage */
 	HAL_ADC_Start(&hadc1);
 	if(HAL_ADC_PollForConversion(&hadc1, 5) == HAL_OK){
 		HAL_ADC_GetValue(&hadc1);
 	}

 	/* Start both Timers in Encoder Mode for Rotary Encoders */
 	HAL_TIM_Encoder_Start(&htim3, TIM_CHANNEL_1); // start encoder mode
 	HAL_TIM_Encoder_Start(&htim4, TIM_CHANNEL_1);
 	TIM3->CNT = 0; // initialize zero
 	TIM4->CNT = 0;

 	/* define the input state */
 	strcpy(In.title,"Input");
 	strcpy(In.par0.name,"Line Vol");
 	In.par0.value=17; //Line Vol, init value
 	In.par0.max=22; //Line Vol, maximum
 	In.par0.min=0; //Line Vol, minimum
 	In.par0.change=1; //Line Vol, step size
 	strcpy(In.par1.name,"Source");
 	In.par1.value=3;
 	In.par1.max=3;
 	In.par1.min=0;
 	In.par1.change=1;
 	strcpy(In.par2.name,"HP Vol");
 	In.par2.value=70; //HP Vol, init value
 	In.par2.max=78; //HP Vol, maximum
 	In.par2.min=0; //HP Vol, minimum
 	In.par2.change=1; //HP Vol, step size

 	input_source=&In.par1.value;
 	volume_line=&In.par0.value;
 	volume_hp=&In.par2.value;

 	/* Init TLV320 Audio Codec */
 	TLV320_Init(&hi2c1);
 	BSP_SelectAudioIn(*input_source);
 	TLV320_SetLineInVol(*volume_line);
 	TLV320_SetHeadphoneVol(*volume_hp);

   /* Signal Processing */
 	dsp_mode = DSP_MODE_PASSTHROUGH;

 	/* Clear Audio Rx and Tx buffer for DMA */
 	for(uint16_t i=0; i<DSP_BUFFERSIZE_DOUBLE; i++){
 		pRxData[i] = 0;
 		pTxData[i] = 0;
 	}

 	/* Generate a 1kHz Sine Wave */
 	//uint16_t nDataPoints = BSP_SineWave(48000.0f, 1000.0f, 1000, sinWave, sizeof(sinWave)/sizeof(uint16_t));
 	uint8_t update_counter = 0;

	/* Start automatic DMA Transmission (Full Duplex) */
	/* Double buffer length, Interrupt on Half-Full */
	HAL_I2SEx_TransmitReceive_DMA(&hi2s2, pTxData, pRxData, DSP_BUFFERSIZE_DOUBLE);

	/* show startup screen for 1s (and then clear it) */
	HAL_Delay(1000);
	ssd1306_Fill(&holedR, Black);
	ssd1306_Fill(&holedL, Black);
	ssd1306_DrawHLine(&holedL,0,128,13,White);
	ssd1306_DrawHLine(&holedL,0,128,48,White);
	ssd1306_UpdateScreen(&holedL);
	ssd1306_DrawHLine(&holedR,0,128,13,White);
	ssd1306_DrawHLine(&holedR,0,128,48,White);
	ssd1306_UpdateScreen(&holedR);
	updateAkku();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
//	  if (audio_status.playing) {
//		  HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET);
//	      } else {
//	    	  HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_RESET);
//	      }

	    if(state_nxt!=state_crrnt){
			clearFX();
			}

			state_crrnt = state_nxt;
			update_counter ++;

	    /* STATE MACHINE */
			switch (state_crrnt){
				case IN:
					showFX(&In);
					BSP_SelectAudioIn(*input_source);
					break;

	      /* State Machine Includes */



				default:
					break;
			}



			encoder_change = BSP_ReadEncoder_Difference(ENCODER_LEFT);
			if(encoder_change){    // only execute when something changed
				(*para0).value+=encoder_change*(*para0).change;
				if((*para0).value<(*para0).min)
					(*para0).value=(*para0).min;
				if((*para0).value>(*para0).max)
					(*para0).value=(*para0).max;
			}

			encoder_change = BSP_ReadEncoder_Difference(ENCODER_RIGHT);
			if(encoder_change){    // only execute when something changed
				(*para2).value+=encoder_change*(*para2).change;
				if((*para2).value<(*para2).min)
					(*para2).value=(*para2).min;
				if((*para2).value>(*para2).max)
					(*para2).value=(*para2).max;
			}

			/* LEFT USER BUTTON */
			if(btnLeftPressed){
				btnLeftPressed= 0;
				if(state_crrnt==IN){
					if((*para1).value-1<(*para1).min)
						(*para1).value=(*para1).max;
					else
						(*para1).value-=1;
				}else{
				(*para1).value=!(*para1).value;
				}
			}

			/* RIGHT USER BUTTON */
			if(btnRightPressed){
				btnRightPressed= 0;
				if(state_crrnt==IN){
					if((*para1).value+1>(*para1).max)
						(*para1).value=(*para1).min;
					else
						(*para1).value+=1;
				}else{
					(*para3).value=!(*para3).value;
				}
			}

			/* LEFT ENCODER BUTTON */
			if(encLeftPressed){
				encLeftPressed = 0;
				/*switch to the previous state (limit first state)*/
				if(state_nxt-1<0){
				state_nxt=0;
				}else{
				state_nxt--;
				}
			}

			/* RIGHT ENCODER BUTTON */
			if(encRightPressed){
				encRightPressed = 0;
				/*switch to the next state (limit last state)*/
				if(state_nxt+1>CNT-1){
				state_nxt=CNT-1;
				}else{
				state_nxt++;
				}
			}

	    //limit line vol
			if(*volume_line>22)
				*volume_line=22;
			else if(*volume_line<0)
				*volume_line=0;
			//limit HP vol
			if(*volume_hp>78)
				*volume_hp=78;
			else if(*volume_hp<0)
				*volume_hp=0;
			TLV320_SetLineInVol(*volume_line);
			TLV320_SetHeadphoneVol(*volume_hp);

			/* update the battery level (every 50 counts) */
			if(update_counter >= 50){
				update_counter = 0;
				updateAkku();
			}

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
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 72;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 3;
  RCC_OscInitStruct.PLL.PLLR = 2;
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
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_I2S_APB1|RCC_PERIPHCLK_CLK48;
  PeriphClkInitStruct.Clk48ClockSelection = RCC_CLK48CLKSOURCE_PLLQ;
  PeriphClkInitStruct.I2sApb1ClockSelection = RCC_I2SAPB1CLKSOURCE_EXT;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */
  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.ScanConvMode = DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  hadc1.Init.DMAContinuousRequests = DISABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_9;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

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
  * @brief I2C3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C3_Init(void)
{

  /* USER CODE BEGIN I2C3_Init 0 */

  /* USER CODE END I2C3_Init 0 */

  /* USER CODE BEGIN I2C3_Init 1 */

  /* USER CODE END I2C3_Init 1 */
  hi2c3.Instance = I2C3;
  hi2c3.Init.ClockSpeed = 100000;
  hi2c3.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c3.Init.OwnAddress1 = 0;
  hi2c3.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c3.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c3.Init.OwnAddress2 = 0;
  hi2c3.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c3.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C3_Init 2 */

  /* USER CODE END I2C3_Init 2 */

}

/**
  * @brief I2S2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2S2_Init(void)
{

  /* USER CODE BEGIN I2S2_Init 0 */

  /* USER CODE END I2S2_Init 0 */

  /* USER CODE BEGIN I2S2_Init 1 */

  /* USER CODE END I2S2_Init 1 */
  hi2s2.Instance = SPI2;
  hi2s2.Init.Mode = I2S_MODE_SLAVE_TX;
  hi2s2.Init.Standard = I2S_STANDARD_MSB;
  hi2s2.Init.DataFormat = I2S_DATAFORMAT_16B_EXTENDED;
  hi2s2.Init.MCLKOutput = I2S_MCLKOUTPUT_DISABLE;
  hi2s2.Init.AudioFreq = I2S_AUDIOFREQ_48K;
  hi2s2.Init.CPOL = I2S_CPOL_LOW;
  hi2s2.Init.ClockSource = I2S_CLOCK_EXTERNAL;
  hi2s2.Init.FullDuplexMode = I2S_FULLDUPLEXMODE_ENABLE;
  if (HAL_I2S_Init(&hi2s2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2S2_Init 2 */

  /* USER CODE END I2S2_Init 2 */

}

/**
  * @brief RNG Initialization Function
  * @param None
  * @retval None
  */
static void MX_RNG_Init(void)
{

  /* USER CODE BEGIN RNG_Init 0 */

  /* USER CODE END RNG_Init 0 */

  /* USER CODE BEGIN RNG_Init 1 */

  /* USER CODE END RNG_Init 1 */
  hrng.Instance = RNG;
  if (HAL_RNG_Init(&hrng) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RNG_Init 2 */

  /* USER CODE END RNG_Init 2 */

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

  TIM_Encoder_InitTypeDef sConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 0;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 65535;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  sConfig.EncoderMode = TIM_ENCODERMODE_TI1;
  sConfig.IC1Polarity = TIM_ICPOLARITY_RISING;
  sConfig.IC1Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC1Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC1Filter = 10;
  sConfig.IC2Polarity = TIM_ICPOLARITY_RISING;
  sConfig.IC2Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC2Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC2Filter = 10;
  if (HAL_TIM_Encoder_Init(&htim3, &sConfig) != HAL_OK)
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
  * @brief TIM4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM4_Init(void)
{

  /* USER CODE BEGIN TIM4_Init 0 */

  /* USER CODE END TIM4_Init 0 */

  TIM_Encoder_InitTypeDef sConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM4_Init 1 */

  /* USER CODE END TIM4_Init 1 */
  htim4.Instance = TIM4;
  htim4.Init.Prescaler = 0;
  htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim4.Init.Period = 65535;
  htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  sConfig.EncoderMode = TIM_ENCODERMODE_TI1;
  sConfig.IC1Polarity = TIM_ICPOLARITY_RISING;
  sConfig.IC1Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC1Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC1Filter = 10;
  sConfig.IC2Polarity = TIM_ICPOLARITY_RISING;
  sConfig.IC2Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC2Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC2Filter = 10;
  if (HAL_TIM_Encoder_Init(&htim4, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM4_Init 2 */

  /* USER CODE END TIM4_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Stream3_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream3_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream3_IRQn);
  /* DMA1_Stream4_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream4_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream4_IRQn);

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
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, SET_LIN_Pin|LED1_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, SET_I_LIM_Pin|LED2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(SET_BOOT0_GPIO_Port, SET_BOOT0_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : SET_LIN_Pin LED1_Pin */
  GPIO_InitStruct.Pin = SET_LIN_Pin|LED1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : DTC_LIN_Pin DTC_MIC_Pin */
  GPIO_InitStruct.Pin = DTC_LIN_Pin|DTC_MIC_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : DTC_HP_Pin DTC_LOUT_Pin */
  GPIO_InitStruct.Pin = DTC_HP_Pin|DTC_LOUT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : SW1_Pin SW2_Pin */
  GPIO_InitStruct.Pin = SW1_Pin|SW2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PA2 */
  GPIO_InitStruct.Pin = GPIO_PIN_2;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : BOOT1_Pin */
  GPIO_InitStruct.Pin = BOOT1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(BOOT1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : BTN_ENC2_Pin */
  GPIO_InitStruct.Pin = BTN_ENC2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(BTN_ENC2_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : SET_I_LIM_Pin LED2_Pin */
  GPIO_InitStruct.Pin = SET_I_LIM_Pin|LED2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : BTN_ENC1_Pin */
  GPIO_InitStruct.Pin = BTN_ENC1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(BTN_ENC1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : SET_BOOT0_Pin */
  GPIO_InitStruct.Pin = SET_BOOT0_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(SET_BOOT0_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI0_IRQn);

  HAL_NVIC_SetPriority(EXTI1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI1_IRQn);

  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

}

/* USER CODE BEGIN 4 */
/**
  * @brief Display the current state effect struct
  * @retval None
	*	@param fx: pointer to the effect's struct
  */
void showFX(effect *fx){
	/*direct the pointers to the currently displayed effect struct*/
	para0=&(*fx).par0;
	para1=&(*fx).par1;
	para2=&(*fx).par2;
	para3=&(*fx).par3;

	/****************************************
	 * Draw the left display
	 ***************************************/

	if(state_crrnt!=0){
    // draw the left arrow
		ssd1306_SetCursor(&holedL, 2, 23);
		ssd1306_WriteString(&holedL, "<", Font_11x18, White);
		ssd1306_DrawHLine(&holedL,0,13,23,White);
		ssd1306_DrawVLine(&holedL,13,23,40,White);
		ssd1306_DrawHLine(&holedL,0,13,40,White);
		ssd1306_DrawVLine(&holedL,0,23,40,White);
	}else{
    // remove the left arrow
		ssd1306_SetCursor(&holedL, 0, 23);
		ssd1306_WriteString(&holedL, "  ", Font_11x18, White);
	}
	ssd1306_SetCursor(&holedL, 10, 0);
	if((*fx).par0.name[0]!= '\0'){
		sprintf(lcd_buf, "%s %5d %s",(*fx).par0.name, (*fx).par0.value, (*fx).par0.unit);
		ssd1306_WriteString(&holedL, lcd_buf, Font_7x10, White);
	}

	ssd1306_SetCursor(&holedL, 10, 53);
	if((*fx).par1.name[0]!= '\0'){
		if(state_crrnt==IN){
			char tmp[5];
			//check input state
			switch((*fx).par1.value){
				case 0:
					strcpy(tmp,"EXT");
					break;
				case 1:
					strcpy(tmp,"LINE");
					break;
				case 2:
					strcpy(tmp,"MIC");
					break;
				case 3:
					strcpy(tmp,"USB");
					break;
				default:
					break;
			}
			sprintf(lcd_buf, "%s %s %s",(*fx).par1.name, tmp, (*fx).par1.unit);
		} else{
			sprintf(lcd_buf, "%s %1d %s",(*fx).par1.name, (*fx).par1.value, (*fx).par1.unit);
		}
		ssd1306_WriteString(&holedL, lcd_buf, Font_7x10, White);
	}
	ssd1306_SetCursor(&holedL, 30, 23);
	ssd1306_WriteString(&holedL, (*fx).title, Font_11x18, White);
	ssd1306_UpdateScreen(&holedL);


	/****************************************
	 * Draw the right display
	 ***************************************/

	if(state_crrnt!=CNT-1){
		//draw the right arrow
		ssd1306_SetCursor(&holedR, 116, 23);
		ssd1306_WriteString(&holedR, ">", Font_11x18, White);
		ssd1306_DrawHLine(&holedR,114,127,23,White);
		ssd1306_DrawVLine(&holedR,114,23,40,White);
		ssd1306_DrawHLine(&holedR,114,127,40,White);
		ssd1306_DrawVLine(&holedR,127,23,40,White);
	}else{
		//remove the right arrow
		ssd1306_SetCursor(&holedR, 114, 23);
		ssd1306_WriteString(&holedR, "  ", Font_11x18, White);
		ssd1306_DrawVLine(&holedR,127,23,40,Black);
		ssd1306_DrawVLine(&holedR,126,23,40,Black);
		ssd1306_DrawVLine(&holedR,125,23,40,Black);
	}
	ssd1306_SetCursor(&holedR, 10, 0);
	if((*fx).par2.name[0]!= '\0'){
		sprintf(lcd_buf, "%s %5d %s",(*fx).par2.name, (*fx).par2.value, (*fx).par2.unit);
		ssd1306_WriteString(&holedR, lcd_buf, Font_7x10, White);
	}
	ssd1306_SetCursor(&holedR, 10, 53);
	if((*fx).par3.name[0]!= '\0'){
		sprintf(lcd_buf, "%s %1d %s",(*fx).par3.name, (*fx).par3.value, (*fx).par3.unit);
		ssd1306_WriteString(&holedR, lcd_buf, Font_7x10, White);
	}
	ssd1306_UpdateScreen(&holedR);
}

void clearFX(void){
		//clear the Strings & Parameters
		sprintf(lcd_buf, "                  ");
		//remove the parameters
		ssd1306_SetCursor(&holedL, 10, 0);
		ssd1306_WriteString(&holedL, lcd_buf, Font_7x10, White);
		ssd1306_SetCursor(&holedL, 10, 53);
		ssd1306_WriteString(&holedL, lcd_buf, Font_7x10, White);
		ssd1306_SetCursor(&holedR, 10, 0);
		ssd1306_WriteString(&holedR, lcd_buf, Font_7x10, White);
		ssd1306_SetCursor(&holedR, 10, 53);
		ssd1306_WriteString(&holedR, lcd_buf, Font_7x10, White);
		//remove the title
		ssd1306_SetCursor(&holedL, 30, 23);
		ssd1306_WriteString(&holedL, lcd_buf, Font_11x18, White);
		//update
		ssd1306_UpdateScreen(&holedL);
		ssd1306_UpdateScreen(&holedR);
}


/**
  * @brief Update the state of the akku
  * @retval None
  * @param None
*/

void updateAkku(void){
    float volt = BSP_ReadBatteryVoltage(10);

    //shut the uC down
    if(volt<=UNDER_VOLTAGE){
            ssd1306_Fill(&holedL, Black);
            ssd1306_Fill(&holedR, Black);
            sprintf(lcd_buf, "Going to sleep...");
            ssd1306_SetCursor(&holedL, 10, 20);
            ssd1306_WriteString(&holedL, lcd_buf, Font_7x10, White);
            ssd1306_UpdateScreen(&holedL);
            sprintf(lcd_buf, "Charge battery!");
            ssd1306_SetCursor(&holedR, 10, 20);
            ssd1306_WriteString(&holedR, lcd_buf, Font_7x10, White);
            ssd1306_UpdateScreen(&holedR);
            TLV320_PowerDown();
            HAL_PWR_EnterSTANDBYMode();
    }

    uint8_t akku_state =(volt-UNDER_VOLTAGE)/(MAX_VOLTAGE-UNDER_VOLTAGE)*100;
    /* different levels of the batterie symbol */
    if(akku_state>100)
            akku_state=100;
    if (akku_state>=84){
    level=5;
    } else if (akku_state<84 && akku_state>=68){
            level=4;
    }	else if (akku_state<68 && akku_state>=52){
            level=3;
    }	else if (akku_state<52 && akku_state>=36){
            level=2;
    }	else if (akku_state<36 && akku_state>=20){
            level=1;
    }	else if (akku_state<20){
            level=0;
    }

    ssd1306_SetCursor(&holedR, 59, 27);
    sprintf(lcd_buf, "%3.0d%%", akku_state);
    ssd1306_WriteString(&holedR, lcd_buf, Font_7x10, White);
    ssd1306_SetCursor(&holedR, 89, 27);
    ssd1306_DrawBat(&holedR,level, Bat_16x9, White);
    ssd1306_UpdateScreen(&holedR);
}

/* USER CODE END 4 */

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
