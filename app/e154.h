#ifndef __E154_H__
#define __E154_H__

#include "stm8s.h"

#define xDot 200
#define yDot 200

#define E154_MOSI_0	        GPIO_WriteLow(GPIOC, GPIO_PIN_2)
#define E154_MOSI_1	        GPIO_WriteHigh(GPIOC, GPIO_PIN_2)

#define E154_CLK_0  	    GPIO_WriteLow(GPIOC, GPIO_PIN_1)
#define E154_CLK_1	        GPIO_WriteHigh(GPIOC, GPIO_PIN_1)

#define E154_CS_0	        GPIO_WriteLow(GPIOE, GPIO_PIN_5)
#define E154_CS_1	        GPIO_WriteHigh(GPIOE, GPIO_PIN_5)

#define E154_DC_0	        GPIO_WriteLow(GPIOB, GPIO_PIN_0)
#define E154_DC_1	        GPIO_WriteHigh(GPIOB, GPIO_PIN_0)

#define E154_RST_0  	    GPIO_WriteLow(GPIOB, GPIO_PIN_1)
#define E154_RST_1	        GPIO_WriteHigh(GPIOB, GPIO_PIN_1)

#define E154_BUSY           GPIO_ReadInputPin(GPIOB, GPIO_PIN_2) 

void EPD_Dis_Part(unsigned char xStart,unsigned char xEnd,unsigned long yStart,unsigned long yEnd,unsigned char *DisBuffer,unsigned char Label);
void EPD_Dis_Full(unsigned char *DisBuffer,unsigned char Label);
void EPD_init_Full(void);
void EPD_init_Part(void);




#endif
