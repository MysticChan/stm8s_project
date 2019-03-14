#include <stdio.h>
#include "atom.h"
#include "atommutex.h"
#include "uart.h"
#include "string.h"

static ATOM_MUTEX uart_mutex;

struct Serial_struct Serial_Data;
/*
 * Initialize the UART to requested baudrate, tx/rx, 8N1.
 */
void uart_init(uint32_t baudrate)
{
  /**
   * Set up UART2 for putting out debug messages.
   * This the UART used on STM8S Discovery, change if required.
   */
  UART2_DeInit();
  UART2_Init (baudrate, UART2_WORDLENGTH_8D, UART2_STOPBITS_1, UART2_PARITY_NO, UART2_SYNCMODE_CLOCK_DISABLE, UART2_MODE_TXRX_ENABLE);
  atomMutexCreate (&uart_mutex);
}


/**
 * \b uart_putchar
 *
 * Write a char out via UART2
 *
 * @param[in] c Character to send
 *
 * @return Character sent
 */
char uart_putchar (char c)
{
    /* Block on private access to the UART */
    if (atomMutexGet(&uart_mutex, 0) == ATOM_OK)
    {
        /* Convert \n to \r\n */
        if (c == '\n')
            putchar('\r');

        /* Write a character to the UART2 */
        UART2_SendData8(c);
      
        /* Loop until the end of transmission */
        while (UART2_GetFlagStatus(UART2_FLAG_TXE) == RESET)
            ;

        /* Return mutex access */
        atomMutexPut(&uart_mutex);

    }

    return (c);
}


#if defined(__SDCC__)
#if __SDCC_REVISION >= 9624
int putchar (int c)
{
    return(uart_putchar(c));
    /* Write a character to the UART1 */
    // CRITICAL_STORE;
    // CRITICAL_START();
    // if(c == '\n')
    // {
    //     UART2_SendData8('\r');
    //     while (UART2_GetFlagStatus(UART2_FLAG_TXE) == RESET);
    // }
    // UART2_SendData8(c);
    // /* Loop until the end of transmission */
    // while (UART2_GetFlagStatus(UART2_FLAG_TXE) == RESET);
    // // CRITICAL_END();
    // return (c);
}
#else
void putchar (char c)
{
    uart_putchar(c);
}
#endif
#endif

void SerialSendBuf( void )
{
    uint16_t i, lenth;
    lenth = strlen( Serial_Data.buffer );
    for( i=0; i<lenth; i++)
    {
        if( Serial_Data.buffer[i] == '\n' )
        {
            UART2_SendData8('\r');
            while (UART2_GetFlagStatus(UART2_FLAG_TXE) == RESET);
        }
        UART2_SendData8( Serial_Data.buffer[i] );
        while (UART2_GetFlagStatus(UART2_FLAG_TXE) == RESET);
    }
}

