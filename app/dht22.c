#include "dht22.h"
#include "atom.h"
#include "atomport-private.h"
#define DHT22_THREAD_PRIO 50
#define DHT22_THREAD_STACK_SIZE      1024
static ATOM_TCB dht22_tcb;
static uint8_t dht22_thread_stack[DHT22_THREAD_STACK_SIZE];
static void dht22_thread_func (uint32_t param);

static uint8_t DHT22_GetReadings (dht22_data *out);
static void DHT22_DecodeReadings (dht22_data *out);
static uint16_t DHT22_GetHumidity (dht22_data *out);
static uint16_t DHT22_GetTemperature (dht22_data *out);
static uint8_t DHT22_Read (dht22_data *out);

void DHT22_Thread (void)
{
    uint8_t status;
    

    /* creat thread */
    status = atomThreadCreate ( &dht22_tcb, DHT22_THREAD_PRIO, dht22_thread_func, 0, &dht22_thread_stack[0], DHT22_THREAD_STACK_SIZE, TRUE);   
    if (status == ATOM_OK)
        ATOMLOG("DHT22 thread\t\tOK\r\n");
    else
        ATOMLOG("DHT22 thread\t\tFAIL\r\n");     
}

static void dht22_thread_func (uint32_t param)
{
    param = param;
    static dht22_data dht22_data_t;
    /* GPIO PC2 */
    GPIO_DeInit(SDA_PORT);
    SDA_OUT;
    
    /* TIMER4 */
    TIM4_DeInit();
    TIM4_TimeBaseInit( TIM4_PRESCALER_8, 0xff );
    TIM4_Cmd(ENABLE);
// #ifdef ATOM_STACK_CHECKING
//     uint32_t used_bytes, free_bytes;
//     /* Check idle thread stack usage */
//     if (atomThreadStackCheck (&dht22_tcb, &used_bytes, &free_bytes) == ATOM_OK)
//     {
//         /* Check the thread did not use up to the end of stack */
//         if (free_bytes == 0)
//         {
//             ATOMLOG ("DHT22 stack overflow\n");
//         }
// #endif
//         /* Log the stack usage */
// #ifdef TESTS_LOG_STACK_USAGE
//         ATOMLOG ("DHT22 Use:%d\n", (int)used_bytes);
    
// #endif
//     }
    while(1)
    {
        ATOMLOG("DHT22 is sampling...\n");
        if( DHT22_Read( &dht22_data_t ) )
        {
            ATOMLOG("Humidity = %f %%\n", dht22_data_t.humidity);
            ATOMLOG("Temperature = %f 'C\n", dht22_data_t.temperature);
        }
        atomTimerDelay(300);
    }

}

static uint8_t DHT22_GetReadings (dht22_data *out)
{
    uint8_t i;
    uint8_t c; //cnt value
    // Switch pin to output
    SDA_OUT;
    // Generate start impulse
    SDA_LOW;
    atomTimerDelay(1);
    SDA_HIGH;
    // Switch pin to input with Pull-Up
    SDA_IN;
    // Wait for AM2302 to begin communication (20-40us)
    RST_CNT;
    while( (c = GET_CNT) < 100 && (SDA_READ) );
    RST_CNT;
    if( c >= 100 )
    {
        ATOMLOG("DHT22_RCV_NO_RESPONSE \r\n");
        ATOMLOG("counter value = %d \n", c);
        return DHT22_RCV_NO_RESPONSE;
    }
    // Check ACK strobe from sensor
    while( (c = GET_CNT) < 100 && !(SDA_READ) );
    RST_CNT;
    if( (c < 65) || (c > 95) )
    {
        ATOMLOG("DHT22_RCV_BAD_ACK1 \r\n");
        ATOMLOG("counter value = %d \n", c);
        return DHT22_RCV_BAD_ACK1;
    }
    while( (c = GET_CNT) < 100 && (SDA_READ) );
    RST_CNT;
    if( (c < 65) || (c > 95) )
    {
        ATOMLOG("DHT22_RCV_BAD_ACK2 \r\n");
        ATOMLOG("counter value = %d \n", c);
        return DHT22_RCV_BAD_ACK2;
    }
    // ACK strobe received --> receive 40 bits
    i = 0;
    while(i < 40)
    {
        // Measure bit start impulse (T_low = 50us)
        while( (c = GET_CNT) < 100 && !(SDA_READ) );
        if (c > 75)
        {
            // invalid bit start impulse length
	        out->bits[i] = 0xff;
            while( (c = GET_CNT) < 100 && (SDA_READ) );
            RST_CNT;
        }
        else
        {
            // Measure bit impulse length (T_h0 = 25us, T_h1 = 70us)
            RST_CNT;
            while( (c = GET_CNT) < 100 && (SDA_READ) );
            RST_CNT;
            out->bits[i] = (c < 100) ? (uint8_t) c : 0xff;
        }
        i++;
    }
    for(i = 0; i < 40; i++)
    {
        if (out->bits[i] == 0xff)
            return DHT22_RCV_TIMEOUT;
    }
    return DHT22_RCV_OK;
}

static void DHT22_DecodeReadings (dht22_data *out)
{
    uint8_t i = 0;

    for (; i < 8; i++)
    {
        out->hMSB <<= 1;
        if (out->bits[i] > 48)
            out->hMSB |= 1;
    }

    for (; i < 16; i++)
    {
        out->hLSB <<= 1;
        if (out->bits[i] > 48)
            out->hLSB |= 1;
    }

    for (; i < 24; i++)
    {
        out->tMSB <<= 1;
        if (out->bits[i] > 48)
            out->tMSB |= 1;
    }

    for (; i < 32; i++)
    {
        out->tLSB <<= 1;
        if (out->bits[i] > 48)
            out->tLSB |= 1;
    }

    for (; i < 40; i++)
    {
        out->parity_rcv <<= 1;
        if (out->bits[i] > 48)
            out->parity_rcv |= 1;
    }

    out->parity = out->hMSB + out->hLSB + out->tMSB + out->tLSB;

}

static uint16_t DHT22_GetHumidity (dht22_data *out)
{
    return (out->hMSB << 8) | out->hLSB;
}

static uint16_t DHT22_GetTemperature (dht22_data *out)
{
    return (out->tMSB << 8) | out->tLSB;
}



static uint8_t DHT22_Read (dht22_data *out)
{
	out->rcv_response = DHT22_GetReadings (out);
	if (out->rcv_response != DHT22_RCV_OK)
	{
	  return 0;
	}

	DHT22_DecodeReadings (out);

	if (out->parity != out->parity_rcv)
	{
	  out->rcv_response = DHT22_BAD_DATA;
	  return 0;
	}

	out->humidity = (float) DHT22_GetHumidity (out) / 10.0f;

	uint16_t temperature = DHT22_GetTemperature (out);
	out->temperature = ((float) (temperature & 0x7fff)) / 10.0f;

	if (temperature & 0x8000)
	{
	  out->temperature = -out->temperature;
	}

	return 1;

}
