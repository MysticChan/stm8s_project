#include "led.h"
#include "atom.h"
#include "atomport-private.h"
#define LED_STACK_SIZE_BYTES 64
#define LED_THREAD_PRIO 100
static ATOM_TCB led_tcb;
static uint8_t led_thread_stack[LED_STACK_SIZE_BYTES];
static void led_thread_func (uint32_t param);

void LED_Thread(void)
{
    uint8_t status = 0;
    status = atomThreadCreate(&led_tcb, LED_THREAD_PRIO, led_thread_func, 0, &led_thread_stack[0], LED_STACK_SIZE_BYTES, TRUE);
    if (status == ATOM_OK)
        ATOMLOG("LED thread\t\tOK\r\n");
    else
        ATOMLOG("LED thread\t\tFAIL\r\n");
}

static void led_thread_func (uint32_t param)
{
    // uint32_t used_bytes, free_bytes;
    param = param;
    GPIO_DeInit(LED_PORT);
    GPIO_Init(LED_PORT, LED_PIN, GPIO_MODE_OUT_PP_HIGH_FAST);
    // if (atomThreadStackCheck (&led_tcb, &used_bytes, &free_bytes) == ATOM_OK)
    // {
    //     if (free_bytes == 0)
    //     {
    //         ATOMLOG ("LED stack overflow\n");
    //     }
        
    // }

    while(1)
    {
        // ATOMLOG ("LED Use:%d\n", (int)used_bytes);
        LED_TOGGLE;
        // ATOMLOG("LED: Blink\n");
        atomTimerDelay (100);
    }
}