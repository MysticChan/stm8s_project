/**
  ******************************************************************************
  * @file project\main.c
  * @brief This file contains the firmware main function.
  * @author STMicroelectronics - MCD Application Team
  * @version V1.1.1
  * @date 06/05/2009
  ******************************************************************************
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2009 STMicroelectronics</center></h2>
  * @image html logo.bmp
  ******************************************************************************
  */


/* Includes ------------------------------------------------------------------*/
#include "atom.h"
#include "atomport-private.h"
#include "uart.h"
#include "led.h"
//#include "dht22.h"

/* Private defines -----------------------------------------------------------*/
#define IDLE_STACK_SIZE_BYTES       128
static uint8_t idle_thread_stack[IDLE_STACK_SIZE_BYTES];
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/* Forward declarations */
void CLK_Init(void);

void main(void)
{
    CLK_Init();
    uart_init(9600);
    ATOMLOG("uart2\t\tOK\n");
    /* Initialise the OS before creating our threads */
    ATOMLOG("atomOSInit\n");
    atomOSInit(&idle_thread_stack[0], IDLE_STACK_SIZE_BYTES, TRUE);
    /* Infinite loop */
    /* Enable the system tick timer */
    ATOMLOG("archInitSystemTickTimer\n");
    archInitSystemTickTimer();
    /* Create an application thread */
    ATOMLOG("LED_Thread\n");
    LED_Thread();
    ATOMLOG("DHT22_Thread\n");
    DHT22_Thread();
    /* Start ATOM OS */
    atomOSStart();
}


void CLK_Init(void)
{
    CLK_DeInit();
    CLK_HSECmd(ENABLE);
    while(SET != CLK_GetFlagStatus(CLK_FLAG_HSERDY));
    CLK_SYSCLKConfig(CLK_PRESCALER_CPUDIV1);
    CLK_ClockSwitchCmd(ENABLE); 
    CLK_ClockSwitchConfig(CLK_SWITCHMODE_AUTO , CLK_SOURCE_HSE , DISABLE ,CLK_CURRENTCLOCKSTATE_DISABLE);
}

#ifdef USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param file: pointer to the source file name
  * @param line: assert_param error line source number
  * @retval : None
  */
void assert_failed(u8* file, u32 line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/******************* (C) COPYRIGHT 2009 STMicroelectronics *****END OF FILE****/
