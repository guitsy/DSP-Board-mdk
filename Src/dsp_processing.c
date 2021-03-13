/*******************************************************************************
 * @file        dsp_processing.c
 * @brief       C Library for processing the incomming datastream
 * @details     
 * @version     1.0
 * @author      Simon Burkhardt
 * @author      Mischa Studer
 * @date        2019.12.05
 * @copyright   (c) 2019 Fachhochschule Nordwestschweiz FHNW
 *              all rights reserved
 * @note        EIT Projekt 5 - HS19 - "DSP Board", Betreuer: Markus Hufschmid
 * @note        <pre>Project Properties > C/C++ > Preprocessor Symbols: ARM_MATH_CM4</pre>
 * @note        <pre>Project Properties > Target > Floating Point Hardware > "Single Precision"</pre>
 *******************************************************************************/
/*
 Project Properties > C/C++ > Preprocessor Symbols: ARM_MATH_CM4
 Project Properties > Target > Floating Point Hardware > "Single Precision"
 */

/* Includes ------------------------------------------------------------------*/
//#include "main.h"
#include "dsp_processing.h"
//#include <arm_math.h>
#include <stdio.h>
//#include "adaptive_fir.h"

/* Define inc */
//#include "fir0.h"
//#include "gain0.h"
//#include "swtch0.h"
//#include "multi0.h"
/* Global Variables ----------------------------------------------------------*/
volatile uint16_t dsp_mode;

//static float32_t rxLeft[DSP_BUFFERSIZE_HALF], rxRight[DSP_BUFFERSIZE_HALF];
//static float32_t txLeft[DSP_BUFFERSIZE_HALF], txRight[DSP_BUFFERSIZE_HALF];

static int16_t rxLeft[DSP_BUFFERSIZE_HALF], rxRight[DSP_BUFFERSIZE_HALF];
static int16_t txLeft[DSP_BUFFERSIZE_HALF], txRight[DSP_BUFFERSIZE_HALF];

/* Define Intermediate Buffer */
//static float32_t EF16Left[DSP_BUFFERSIZE], EF16Right[DSP_BUFFERSIZE];
//static float32_t EF1016Left[DSP_BUFFERSIZE], EF1016Right[DSP_BUFFERSIZE];
//static float32_t EF12Left[DSP_BUFFERSIZE], EF12Right[DSP_BUFFERSIZE];
//static float32_t EF14Left[DSP_BUFFERSIZE], EF14Right[DSP_BUFFERSIZE];
/* Private Functions ---------------------------------------------------------*/

uint16_t index1;
//float32_t* tmp;
//
//	/* define Buffers here */
//	float32_t *P16L = &EF16Left[0];
//	float32_t *P16R = &EF16Right[0];
//	float32_t *P16L2 = &EF16Left[DSP_BUFFERSIZE_HALF];
//	float32_t *P16R2 = &EF16Right[DSP_BUFFERSIZE_HALF];
//	float32_t *P1016L = &EF1016Left[0];
//	float32_t *P1016R = &EF1016Right[0];
//	float32_t *P1016L2 = &EF1016Left[DSP_BUFFERSIZE_HALF];
//	float32_t *P1016R2 = &EF1016Right[DSP_BUFFERSIZE_HALF];
//	float32_t *P12L = &EF12Left[0];
//	float32_t *P12R = &EF12Right[0];
//	float32_t *P12L2 = &EF12Left[DSP_BUFFERSIZE_HALF];
//	float32_t *P12R2 = &EF12Right[DSP_BUFFERSIZE_HALF];
//	float32_t *P14L = &EF14Left[0];
//	float32_t *P14R = &EF14Right[0];
//	float32_t *P14L2 = &EF14Left[DSP_BUFFERSIZE_HALF];
//	float32_t *P14R2 = &EF14Right[DSP_BUFFERSIZE_HALF];

/**
 * @param sourceBuffer Pointer to the Audio Signal Source Buffer (from DMA ISR)
 * @param targetBuffer Pointer to the Audio Signal Destination Buffer (to DMA)
 */
void DSP_Process_Data(uint16_t *sourceBuffer, uint16_t *targetBuffer) {

#ifdef DEBUG_DSP_LATENCY
	/* Measure the Latency of the whole DSP_Process */
	HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET);
#endif

	// copy sourceBuffer to leftSignalBuffer and rightSignalBuffer
	for (index1 = 0; index1 < DSP_BUFFERSIZE_HALF; index1++) {
		rxLeft[index1] = (int16_t) (sourceBuffer[2 * index1]);
		rxRight[index1] = (int16_t) (sourceBuffer[2 * index1 + 1]);
	}

//		/* insert Buffers here */
//		tmp = P16L;
//		P16L = P16L2;
//		P16L2 = tmp;
//
//		tmp = P16R;
//		P16R = P16R2;
//		P16R2 = tmp;
//
//		tmp = P1016L;
//		P1016L = P1016L2;
//		P1016L2 = tmp;
//
//		tmp = P1016R;
//		P1016R = P1016R2;
//		P1016R2 = tmp;
//
//		tmp = P12L;
//		P12L = P12L2;
//		P12L2 = tmp;
//
//		tmp = P12R;
//		P12R = P12R2;
//		P12R2 = tmp;
//
//		tmp = P14L;
//		P14L = P14L2;
//		P14L2 = tmp;
//
//		tmp = P14R;
//		P14R = P14R2;
//		P14R2 = tmp;

	switch (dsp_mode) {
	case DSP_MODE_STANDARD:
		/* insert the functions here */

// 			FIR_Filter_F32_Stereo0(P12L2, P16L, P12R2, P16R);
// 			Gain0_Filter(P14L2, P1016L, P14R2, P1016R);
//
// 			swtch0(P16L2, P16R2, P1016L2, P1016R2, txLeft, txRight);
// 			multi0(rxLeft, rxRight, P12L, P12R, P14L, P14R);
		break;

	case DSP_MODE_PASSTHROUGH:
		//talk through: just copy input (rx) into output (tx)
		for (uint16_t i = 0; i < DSP_BUFFERSIZE_HALF; i++) {
			txLeft[i] = rxLeft[i];
		}
		for (uint16_t i = 0; i < DSP_BUFFERSIZE_HALF; i++) {
			txRight[i] = rxRight[i];
		}
		break;

	default:
		break;
	}

	// copy left and right txBuffer into targetBuffer
	for (index1 = 0; index1 < DSP_BUFFERSIZE_HALF; index1++) {
		targetBuffer[2 * index1] = (int16_t) (txLeft[index1]);
		targetBuffer[2 * index1 + 1] = (int16_t) (txRight[index1]);
	}

#ifdef DEBUG_DSP_LATENCY
	HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_RESET);
#endif
}

