#include "dht22.h"
#include "atom.h"
#include "atomport-private.h"
#define DHT22_THREAD_PRIO 50
#define DHT22_THREAD_STACK_SIZE      128
static ATOM_TCB dht22_tcb;
static uint8_t dht22_thread_stack[DHT22_THREAD_STACK_SIZE];
static void dht22_thread_func (uint32_t param);

static uint8_t DHT22_GetReadings (dht22_data *out);
static void DHT22_DecodeReadings (dht22_data *out);
// static uint16_t DHT22_GetHumidity (dht22_data *out);
// static uint16_t DHT22_GetTemperature (dht22_data *out);
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
    // uint32_t used_bytes, free_bytes;
    static dht22_data dht22_data_t;
    param = param;
    
    /* GPIO PC2 */
    GPIO_DeInit(SDA_PORT);
    SDA_OUT;
    
    /* TIMER4 */
    TIM4_DeInit();
    TIM4_TimeBaseInit( TIM4_PRESCALER_8, 0xff );
    TIM4_Cmd(ENABLE);

    // if (atomThreadStackCheck (&dht22_tcb, &used_bytes, &free_bytes) == ATOM_OK)
    // {
    //     if (free_bytes == 0)
    //     {
    //         ATOMLOG ("DHT22 stack overflow\n");
    //     }
        
    // }

    while(1)
    {
        // ATOMLOG("DHT22 Use:%d\n", (int)used_bytes);
        ATOMLOG("DHT22: Sampling...\n");
        if( DHT22_Read( &dht22_data_t ) )
        {
            ATOMLOG("DHT22: Completed.\n");
            ATOMLOG("DHT22: Humidity = %d (0.1%%).\n", dht22_data_t.humidity);
            ATOMLOG("DHT22: Temperature = %d (0.1'C).\n", dht22_data_t.temperature);
        }
        else
        {
            ATOMLOG("DHT22: Sampling failure.\n");
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

// static uint16_t DHT22_GetHumidity (dht22_data *out)
// {
//     uint16_t value = 0;
//     value |= out->hMSB;
//     value <<= 8;
//     value |= out->hLSB;
//     return value;
// }

// static uint16_t DHT22_GetTemperature (dht22_data *out)
// {
//     uint16_t value = 0;
//     value |= out->tMSB;
//     value <<= 8;
//     value |= out->tLSB;
//     return value;
// }



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
    out->humidity = 0;
	out->humidity |= out->hMSB;
    out->humidity <<= 8;
    out->humidity |= out->hLSB;

    out->temperature = 0;
    out->temperature |= out->tMSB;
    out->temperature <<= 8;
    out->temperature |= out->tLSB;

	return 1;

}
