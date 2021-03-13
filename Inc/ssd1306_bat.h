#include <stdint.h>

#ifndef __SSD1306_BAT_H__
#define __SSD1306_BAT_H__

typedef struct {
	const uint8_t SignWidth;    /*!< Sign width in pixels */
	uint8_t SignHeight;   /*!< Sign height in pixels */
	const uint16_t *data; /*!< Pointer to data sign data array */
} SignDef;


extern SignDef Bat_16x9;


#endif // __SSD1306_BAT_H__
