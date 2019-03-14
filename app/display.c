#include "display.h"
#include "atom.h"
#include "uart.h"
#include "e154.h"
#define E154_THREAD_PRIO 32
#define E154_THREAD_STACK_SIZE      256
static ATOM_TCB e154_tcb;
static uint8_t e154_thread_stack[E154_THREAD_STACK_SIZE];
static void e154_thread_func (uint32_t param);


void E154_Thread(void)
{
    uint8_t status;

    /* creat thread */
    status = atomThreadCreate ( &e154_tcb, E154_THREAD_PRIO, e154_thread_func, 0, &e154_thread_stack[0], E154_THREAD_STACK_SIZE, TRUE);   
    if (status == ATOM_OK)
    {
        //PC2 MOSI CLK CS CD REST BUSY PC1 PE5 PB0 PB1 PB2
        /* PC2 MOSI */
        GPIO_Init( GPIOC, GPIO_PIN_2, GPIO_MODE_OUT_PP_HIGH_SLOW );
        /* PC1 CLK */
        GPIO_Init( GPIOC, GPIO_PIN_1, GPIO_MODE_OUT_PP_HIGH_SLOW );
        /* PE5 CS */
        GPIO_Init( GPIOE, GPIO_PIN_5, GPIO_MODE_OUT_PP_HIGH_SLOW );
        /* PB0 CD */
        GPIO_Init( GPIOB, GPIO_PIN_0, GPIO_MODE_OUT_PP_HIGH_SLOW );
        /* PB1 REST */
        GPIO_Init( GPIOB, GPIO_PIN_1, GPIO_MODE_OUT_PP_HIGH_SLOW );
        /* PB2 BUSY */
        GPIO_Init( GPIOB, GPIO_PIN_2, GPIO_MODE_IN_FL_NO_IT );

        sprintf( uartbuf, "E154 thread\t\tOK\n" );
        SerialSendBuf(); 
    }
    else
    {
        sprintf( uartbuf, "E154 thread\t\tFALURE\n" );
        SerialSendBuf(); 
    }
}

static void e154_thread_func (uint32_t param)
{
    uint8_t m;
    param = param;
    EPD_init_Full();
	m=0xff;
	EPD_Dis_Full((unsigned char *)&m,1);  //all white
    while(1)
    {
        
	    // atomTimerDelay(500);
        // // EPD_Dis_Full((unsigned char *)logo,1);
        // // atomTimerDelay(300);
        // m=0;
        // EPD_Dis_Full((unsigned char *)&m,1);
        atomTimerDelay(500);
        // EPD_Dis_Full((unsigned char *)logo,0);
        // atomTimerDelay(500);
    }
    
    
}


