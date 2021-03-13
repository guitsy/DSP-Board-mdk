/*******************************************************************************
 * @file        dsp_processing.h
 * @brief       C Library for processing the incomming datastream
 * @details     
 * @version     1.0
 * @author      Simon Burkhardt
 * @author      Mischa Studer
 * @date        2019.12.05
 * @copyright   (c) 2019 Fachhochschule Nordwestschweiz FHNW
 *              all rights reserved
 * @note        EIT Projekt 5 - HS19 - "DSP Board", Betreuer: Markus Hufschmid
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __DSP_PROCESSING_H
#define __DSP_PROCESSING_H

#ifdef	__cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"
//#include <arm_math.h>

/* Private defines -----------------------------------------------------------*/
#define DSP_BUFFERSIZE 96
#define DSP_BUFFERSIZE_HALF DSP_BUFFERSIZE/2
#define DSP_BUFFERSIZE_DOUBLE DSP_BUFFERSIZE*2
 #define DEBUG_DSP_LATENCY   // enable to toggle LED to measure latency

/* Private Typedefs ----------------------------------------------------------*/
typedef struct {
	uint16_t pTxData[DSP_BUFFERSIZE];
	uint16_t pRxData[DSP_BUFFERSIZE];
} DSPBuffer_t;

enum {
	DSP_MODE_PASSTHROUGH,
	DSP_MODE_STANDARD
};



/* Private Function Prototypes -----------------------------------------------*/
void DSP_Process_Data(uint16_t *sourceBuffer, uint16_t *targetBuffer);
void DSP_Update_Adaptive_FIR(float fg);


#ifdef	__cplusplus
}
#endif

#endif // __DSP_PROCESSING_H


