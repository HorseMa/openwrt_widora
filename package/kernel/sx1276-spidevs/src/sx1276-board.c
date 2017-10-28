/*
 / _____)             _              | |
( (____  _____ ____ _| |_ _____  ____| |__
 \____ \| ___ |    (_   _) ___ |/ ___)  _ \
 _____) ) ____| | | || |_| ____( (___| | | |
(______/|_____)_|_|_| \__)_____)\____)_| |_|
    (C)2013 Semtech

Description: SX1276 driver specific target board functions implementation

License: Revised BSD License, see LICENSE.TXT file include in the project

Maintainer: Miguel Luis and Gregory Cristian
*/
#include "radio.h"
#include "sx1276.h"
#include "sx1276-board.h"
#include "typedef.h"
#include <asm/irq.h> //---disable_irq, enable_irq()
#include <linux/interrupt.h> //---request_irq()
#include "pinmap.h"
#include <linux/gpio.h>
#include <linux/spi/spi.h>
#include <linux/spi/spi_gpio.h>
#include "utilities.h"

/*!
 * Flag used to set the RF switch control pins in low power mode when the radio is not active.
 */
static bool RadioIsActive = false;
extern struct spi_device *slave;

/*!
 * Radio driver structure initialization
 */
const struct Radio_s Radio =
{
    SX1276Init,
    SX1276GetStatus,
    SX1276SetModem,
    SX1276SetChannel,
    SX1276IsChannelFree,
    SX1276Random,
    SX1276SetRxConfig,
    SX1276SetTxConfig,
    SX1276CheckRfFrequency,
    SX1276GetTimeOnAir,
    SX1276Send,
    SX1276SetSleep,
    SX1276SetStby,
    SX1276SetRx,
    SX1276StartCad,
    SX1276SetTxContinuousWave,
    SX1276ReadRssi,
    SX1276Write,
    SX1276Read,
    SX1276WriteBuffer,
    SX1276ReadBuffer,
    SX1276SetMaxPayloadLength,
    SX1276SetPublicNetwork
};

unsigned int sx1278_1_dio0irq = 0,sx1278_1_dio1irq = 0,sx1278_1_dio2irq = 0,sx1278_1_dio3irq = 0,sx1278_1_dio4irq = 0,sx1278_1_dio5irq = 0;
unsigned int sx1278_2_dio0irq = 0,sx1278_2_dio1irq = 0,sx1278_2_dio2irq = 0,sx1278_2_dio3irq = 0,sx1278_2_dio4irq = 0,sx1278_2_dio5irq = 0;

extern void SX1276OnDio0Irq(unsigned long);
extern void SX1276OnDio1Irq(unsigned long);
extern void SX1276OnDio2Irq(unsigned long);
extern void SX1276OnDio3Irq(unsigned long);
extern void SX1276OnDio4Irq(unsigned long);
extern void SX1276OnDio5Irq(unsigned long);
DECLARE_TASKLET(sx1276_1OnDio0,SX1276OnDio0Irq,0);
DECLARE_TASKLET(sx1276_1OnDio1,SX1276OnDio1Irq,0);
DECLARE_TASKLET(sx1276_1OnDio2,SX1276OnDio2Irq,0);
DECLARE_TASKLET(sx1276_1OnDio3,SX1276OnDio3Irq,0);
DECLARE_TASKLET(sx1276_1OnDio4,SX1276OnDio4Irq,0);
DECLARE_TASKLET(sx1276_1OnDio5,SX1276OnDio5Irq,0);
DECLARE_TASKLET(sx1276_2OnDio0,SX1276OnDio0Irq,1);
DECLARE_TASKLET(sx1276_2OnDio1,SX1276OnDio1Irq,1);
DECLARE_TASKLET(sx1276_2OnDio2,SX1276OnDio2Irq,1);
DECLARE_TASKLET(sx1276_2OnDio3,SX1276OnDio3Irq,1);
DECLARE_TASKLET(sx1276_2OnDio4,SX1276OnDio4Irq,1);
DECLARE_TASKLET(sx1276_2OnDio5,SX1276OnDio5Irq,1);

static irqreturn_t sx1278_1_dio0irq_handler(int irq, void *dev_id)
{
    tasklet_schedule(&sx1276_1OnDio0);//调度底半部
    printk("%s, %d\r\n",__func__,__LINE__);
    return 0;
}
static irqreturn_t sx1278_1_dio1irq_handler(int irq, void *dev_id)
{
    tasklet_schedule(&sx1276_1OnDio1);//调度底半部
    printk("%s, %d\r\n",__func__,__LINE__);
    return 0;
}
static irqreturn_t sx1278_1_dio2irq_handler(int irq, void *dev_id)
{
    tasklet_schedule(&sx1276_1OnDio2);//调度底半部
    printk("%s, %d\r\n",__func__,__LINE__);
    return 0;
}
static irqreturn_t sx1278_1_dio3irq_handler(int irq, void *dev_id)
{
    tasklet_schedule(&sx1276_1OnDio3);//调度底半部
    printk("%s, %d\r\n",__func__,__LINE__);
    return 0;
}
static irqreturn_t sx1278_1_dio4irq_handler(int irq, void *dev_id)
{
    tasklet_schedule(&sx1276_1OnDio4);//调度底半部
    printk("%s, %d\r\n",__func__,__LINE__);
    return 0;
}
#if 0
static irqreturn_t sx1278_1_dio5irq_handler(int irq, void *dev_id)
{
    tasklet_schedule(&sx1276_1OnDio5);//调度底半部
    printk("%s, %d\r\n",__func__,__LINE__);
    return 0;
}
#endif
static irqreturn_t sx1278_2_dio0irq_handler(int irq, void *dev_id)
{
    tasklet_schedule(&sx1276_2OnDio0);//调度底半部
    printk("%s, %d\r\n",__func__,__LINE__);
    return 0;
}
static irqreturn_t sx1278_2_dio1irq_handler(int irq, void *dev_id)
{
    tasklet_schedule(&sx1276_2OnDio1);//调度底半部
    printk("%s, %d\r\n",__func__,__LINE__);
    return 0;
}
static irqreturn_t sx1278_2_dio2irq_handler(int irq, void *dev_id)
{
    tasklet_schedule(&sx1276_2OnDio2);//调度底半部
    printk("%s, %d\r\n",__func__,__LINE__);
    return 0;
}
static irqreturn_t sx1278_2_dio3irq_handler(int irq, void *dev_id)
{
    tasklet_schedule(&sx1276_2OnDio3);//调度底半部
    printk("%s, %d\r\n",__func__,__LINE__);
    return 0;
}
static irqreturn_t sx1278_2_dio4irq_handler(int irq, void *dev_id)
{
    tasklet_schedule(&sx1276_2OnDio4);//调度底半部
    printk("%s, %d\r\n",__func__,__LINE__);
    return 0;
}
#if 0
static irqreturn_t sx1278_2_dio5irq_handler(int irq, void *dev_id)
{
    tasklet_schedule(&sx1276_2OnDio5);//调度底半部
    printk("%s, %d\r\n",__func__,__LINE__);
    return 0;
}
#endif
/*!
 * Antenna switch GPIO pins objects
 */
//Gpio_t AntSwitchLf;
//Gpio_t AntSwitchHf;

void SX1276IoInit( int chip )
{
    int err;
    switch(chip)
    {
        case 0:
            err = gpio_request(SX1278_1_RST_PIN, "SX1278_1_RST_PIN");
            if(err)
            {
                printk("%s,%d\r\n",__func__,__LINE__);
            }
            err = gpio_direction_output(SX1278_1_RST_PIN,0);
            if(err)
            {
                printk("%s,%d\r\n",__func__,__LINE__);
            }
            //SX1276.Spi = slave;
            err = gpio_request(SX1278_1_DIO0_PIN, "SX1278_1_DIO0_PIN");
            if(err)
            {
                printk("%s,%d\r\n",__func__,__LINE__);
            }
            err = gpio_direction_input(SX1278_1_DIO0_PIN);
            if(err)
            {
                printk("%s,%d\r\n",__func__,__LINE__);
            }
            err = gpio_request(SX1278_1_DIO1_PIN, "SX1278_1_DIO1_PIN");
            if(err)
            {
                printk("%s,%d\r\n",__func__,__LINE__);
            }
            err = gpio_direction_input(SX1278_1_DIO1_PIN);
            if(err)
            {
                printk("%s,%d\r\n",__func__,__LINE__);
            }
            err = gpio_request(SX1278_1_DIO2_PIN, "SX1278_1_DIO2_PIN");
            if(err)
            {
                printk("%s,%d\r\n",__func__,__LINE__);
            }
            err = gpio_direction_input(SX1278_1_DIO2_PIN);
            if(err)
            {
                printk("%s,%d\r\n",__func__,__LINE__);
            }
            err = gpio_request(SX1278_1_DIO3_PIN, "SX1278_1_DIO3_PIN");
            if(err)
            {
                printk("%s,%d\r\n",__func__,__LINE__);
            }
            err = gpio_direction_input(SX1278_1_DIO3_PIN);
            if(err)
            {
                printk("%s,%d\r\n",__func__,__LINE__);
            }
            err = gpio_request(SX1278_1_DIO4_PIN, "SX1278_1_DIO4_PIN");
            if(err)
            {
                printk("%s,%d\r\n",__func__,__LINE__);
            }
            err = gpio_direction_input(SX1278_1_DIO4_PIN);
            if(err)
            {
                printk("%s,%d\r\n",__func__,__LINE__);
            }
            err = gpio_request(SX1278_1_DIO5_PIN, "SX1278_1_DIO5_PIN");
            if(err)
            {
                printk("%s,%d\r\n",__func__,__LINE__);
            }
            err = gpio_direction_input(SX1278_1_DIO5_PIN);
            if(err)
            {
                printk("%s,%d\r\n",__func__,__LINE__);
            }
            break;
        case 1:
            err = gpio_request(SX1278_2_RST_PIN, "SX1278_2_RST_PIN");
            if(err)
            {
                printk("%s,%d\r\n",__func__,__LINE__);
            }
            err = gpio_direction_output(SX1278_2_RST_PIN,0);
            if(err)
            {
                printk("%s,%d\r\n",__func__,__LINE__);
            }
            //SX1276.Spi = slave;
            err = gpio_request(SX1278_2_DIO0_PIN, "SX1278_2_DIO0_PIN");
            if(err)
            {
                printk("%s,%d\r\n",__func__,__LINE__);
            }
            err = gpio_direction_input(SX1278_2_DIO0_PIN);
            if(err)
            {
                printk("%s,%d\r\n",__func__,__LINE__);
            }
            err = gpio_request(SX1278_2_DIO1_PIN, "SX1278_2_DIO1_PIN");
            if(err)
            {
                printk("%s,%d\r\n",__func__,__LINE__);
            }
            err = gpio_direction_input(SX1278_2_DIO1_PIN);
            if(err)
            {
                printk("%s,%d\r\n",__func__,__LINE__);
            }
            err = gpio_request(SX1278_2_DIO2_PIN, "SX1278_2_DIO2_PIN");
            if(err)
            {
                printk("%s,%d\r\n",__func__,__LINE__);
            }
            err = gpio_direction_input(SX1278_2_DIO2_PIN);
            if(err)
            {
                printk("%s,%d\r\n",__func__,__LINE__);
            }
            err = gpio_request(SX1278_2_DIO3_PIN, "SX1278_2_DIO3_PIN");
            if(err)
            {
                printk("%s,%d\r\n",__func__,__LINE__);
            }
            err = gpio_direction_input(SX1278_2_DIO3_PIN);
            if(err)
            {
                printk("%s,%d\r\n",__func__,__LINE__);
            }
            err = gpio_request(SX1278_2_DIO4_PIN, "SX1278_2_DIO4_PIN");
            if(err)
            {
                printk("%s,%d\r\n",__func__,__LINE__);
            }
            err = gpio_direction_input(SX1278_2_DIO4_PIN);
            if(err)
            {
                printk("%s,%d\r\n",__func__,__LINE__);
            }
            err = gpio_request(SX1278_2_DIO5_PIN, "SX1278_2_DIO5_PIN");
            if(err)
            {
                printk("%s,%d\r\n",__func__,__LINE__);
            }
            err = gpio_direction_input(SX1278_2_DIO5_PIN);
            if(err)
            {
                printk("%s,%d\r\n",__func__,__LINE__);
            }
            break;
        case 2:
            break;
        default:
            break;
    }
}

void SX1276IoIrqInit( int chip )
{
    int err;
    switch(chip)
    {
        case 0:
            sx1278_1_dio0irq = gpio_to_irq(SX1278_1_DIO0_PIN);
            err = request_irq(sx1278_1_dio0irq,sx1278_1_dio0irq_handler,IRQF_TRIGGER_RISING,"sx1278_1_dio0irq",NULL);
            //disable_irq(sx1278_1_dio0irq);
            if(err)
            {
                printk( "sx1278_1_dio0irq request failed\r\n");
            }
            sx1278_1_dio1irq = gpio_to_irq(SX1278_1_DIO1_PIN);
            err = request_irq(sx1278_1_dio1irq,sx1278_1_dio1irq_handler,IRQF_TRIGGER_RISING,"sx1278_1_dio1irq",NULL);
            //disable_irq(sx1278_1_dio1irq);
            if(err)
            {
                printk( "sx1278_1_dio1irq request failed\r\n");
            }
            sx1278_1_dio2irq = gpio_to_irq(SX1278_1_DIO2_PIN);
            err = request_irq(sx1278_1_dio2irq,sx1278_1_dio2irq_handler,IRQF_TRIGGER_RISING,"sx1278_1_dio2irq",NULL);
            //disable_irq(sx1278_1_dio2irq);
            if(err)
            {
                printk( "sx1278_1_dio2irq request failed\r\n");
            }
            sx1278_1_dio3irq = gpio_to_irq(SX1278_1_DIO3_PIN);
            err = request_irq(sx1278_1_dio3irq,sx1278_1_dio3irq_handler,IRQF_TRIGGER_RISING,"sx1278_1_dio3irq",NULL);
            //disable_irq(sx1278_1_dio3irq);
            if(err)
            {
                printk( "sx1278_1_dio3irq request failed\r\n");
            }
            sx1278_1_dio4irq = gpio_to_irq(SX1278_1_DIO4_PIN);
            err = request_irq(sx1278_1_dio4irq,sx1278_1_dio4irq_handler,IRQF_TRIGGER_RISING,"sx1278_1_dio4irq",NULL);
            //disable_irq(sx1278_1_dio4irq);
            if(err)
            {
                printk( "sx1278_1_dio4irq request failed\r\n");
            }
            /*sx1278_1_dio5irq = gpio_to_irq(SX1278_1_DIO5_PIN);
            err = request_irq(sx1278_1_dio5irq,sx1278_1_dio5irq_handler,IRQF_TRIGGER_RISING,"sx1278_1_dio5irq",NULL);*/
            //disable_irq(sx1278_1_dio5irq);
            //if(err)
            //{
            //    printk( "sx1278_1_dio5irq request failed\r\n");
            //}
            break;
        case 1:
            sx1278_2_dio0irq = gpio_to_irq(SX1278_2_DIO0_PIN);
            err = request_irq(sx1278_2_dio0irq,sx1278_2_dio0irq_handler,IRQF_TRIGGER_RISING,"sx1278_2_dio0irq",NULL);
            //disable_irq(sx1278_2_dio0irq);
            if(err)
            {
                printk( "sx1278_2_dio0irq request failed\r\n");
            }
            sx1278_2_dio1irq = gpio_to_irq(SX1278_2_DIO1_PIN);
            err = request_irq(sx1278_2_dio1irq,sx1278_2_dio1irq_handler,IRQF_TRIGGER_RISING,"sx1278_2_dio1irq",NULL);
            //disable_irq(sx1278_2_dio1irq);
            if(err)
            {
                printk( "sx1278_2_dio1irq request failed\r\n");
            }
            sx1278_2_dio2irq = gpio_to_irq(SX1278_2_DIO2_PIN);
            err = request_irq(sx1278_2_dio2irq,sx1278_2_dio2irq_handler,IRQF_TRIGGER_RISING,"sx1278_2_dio2irq",NULL);
            //disable_irq(sx1278_2_dio2irq);
            if(err)
            {
                printk( "sx1278_2_dio2irq request failed\r\n");
            }
            sx1278_2_dio3irq = gpio_to_irq(SX1278_2_DIO3_PIN);
            err = request_irq(sx1278_2_dio3irq,sx1278_2_dio3irq_handler,IRQF_TRIGGER_RISING,"sx1278_2_dio3irq",NULL);
            //disable_irq(sx1278_2_dio3irq);
            if(err)
            {
                printk( "sx1278_2_dio3irq request failed\r\n");
            }
            sx1278_2_dio4irq = gpio_to_irq(SX1278_2_DIO4_PIN);
            err = request_irq(sx1278_2_dio4irq,sx1278_2_dio4irq_handler,IRQF_TRIGGER_RISING,"sx1278_2_dio4irq",NULL);
            //disable_irq(sx1278_2_dio4irq);
            if(err)
            {
                printk( "sx1278_2_dio4irq request failed\r\n");
            }
            /*sx1278_2_dio5irq = gpio_to_irq(SX1278_2_DIO5_PIN);
            err = request_irq(sx1278_2_dio5irq,sx1278_2_dio5irq_handler,IRQF_TRIGGER_RISING,"sx1278_2_dio5irq",NULL);*/
            //disable_irq(sx1278_2_dio5irq);
            //if(err)
            //{
            //    printk( "sx1278_2_dio5irq request failed\r\n");
            //}
            break;
        case 2:
            break;
        default:
            break;
    }
}
void SX1276IoDeInit( void )
{

}

void SX1276SetRfTxPower( int chip ,int8_t power )
{
    uint8_t paConfig = 0;
    uint8_t paDac = 0;

    paConfig = SX1276Read( chip,REG_PACONFIG );
    paDac = SX1276Read( chip,REG_PADAC );

    paConfig = ( paConfig & RF_PACONFIG_PASELECT_MASK ) | SX1276GetPaSelect( chip,SX1276[chip].Settings.Channel );
    paConfig = ( paConfig & RF_PACONFIG_MAX_POWER_MASK ) | 0x70;

    if( ( paConfig & RF_PACONFIG_PASELECT_PABOOST ) == RF_PACONFIG_PASELECT_PABOOST )
    {
        if( power > 17 )
        {
            paDac = ( paDac & RF_PADAC_20DBM_MASK ) | RF_PADAC_20DBM_ON;
        }
        else
        {
            paDac = ( paDac & RF_PADAC_20DBM_MASK ) | RF_PADAC_20DBM_OFF;
        }
        if( ( paDac & RF_PADAC_20DBM_ON ) == RF_PADAC_20DBM_ON )
        {
            if( power < 5 )
            {
                power = 5;
            }
            if( power > 20 )
            {
                power = 20;
            }
            paConfig = ( paConfig & RF_PACONFIG_OUTPUTPOWER_MASK ) | ( uint8_t )( ( uint16_t )( power - 5 ) & 0x0F );
        }
        else
        {
            if( power < 2 )
            {
                power = 2;
            }
            if( power > 17 )
            {
                power = 17;
            }
            paConfig = ( paConfig & RF_PACONFIG_OUTPUTPOWER_MASK ) | ( uint8_t )( ( uint16_t )( power - 2 ) & 0x0F );
        }
    }
    else
    {
        if( power < -1 )
        {
            power = -1;
        }
        if( power > 14 )
        {
            power = 14;
        }
        paConfig = ( paConfig & RF_PACONFIG_OUTPUTPOWER_MASK ) | ( uint8_t )( ( uint16_t )( power + 1 ) & 0x0F );
    }
    SX1276Write( chip,REG_PACONFIG, paConfig );
    SX1276Write( chip,REG_PADAC, paDac );
}

uint8_t SX1276GetPaSelect( int chip,uint32_t channel )
{
    if( channel < RF_MID_BAND_THRESH )
    {
        return RF_PACONFIG_PASELECT_PABOOST;
    }
    else
    {
        return RF_PACONFIG_PASELECT_RFO;
    }
}

void SX1276SetAntSwLowPower( int chip,bool status )
{
    if( RadioIsActive != status )
    {
        RadioIsActive = status;

        if( status == false )
        {
            SX1276AntSwInit( chip);
        }
        else
        {
            SX1276AntSwDeInit( chip);
        }
    }
}

void SX1276AntSwInit( int chip )
{
    //GpioInit( &AntSwitchLf, RADIO_ANT_SWITCH_LF, PIN_OUTPUT, PIN_PUSH_PULL, PIN_PULL_UP, 1 );
    //GpioInit( &AntSwitchHf, RADIO_ANT_SWITCH_HF, PIN_OUTPUT, PIN_PUSH_PULL, PIN_PULL_UP, 0 );
}

void SX1276AntSwDeInit( int chip )
{
    //GpioInit( &AntSwitchLf, RADIO_ANT_SWITCH_LF, PIN_OUTPUT, PIN_OPEN_DRAIN, PIN_NO_PULL, 0 );
    //GpioInit( &AntSwitchHf, RADIO_ANT_SWITCH_HF, PIN_OUTPUT, PIN_OPEN_DRAIN, PIN_NO_PULL, 0 );
}

void SX1276SetAntSw( int chip,uint8_t opMode )
{
    switch( opMode )
    {
    case RFLR_OPMODE_TRANSMITTER:
        //GpioWrite( &AntSwitchLf, 0 );
        //GpioWrite( &AntSwitchHf, 1 );
        break;
    case RFLR_OPMODE_RECEIVER:
    case RFLR_OPMODE_RECEIVER_SINGLE:
    case RFLR_OPMODE_CAD:
    default:
        //GpioWrite( &AntSwitchLf, 1 );
        //GpioWrite( &AntSwitchHf, 0 );
        break;
    }
}

bool SX1276CheckRfFrequency( int chip,uint32_t frequency )
{
    // Implement check. Currently all frequencies are supported
    return true;
}

void SX1276IoFree(int chip)
{
    switch(chip)
    {
        case 0:
            gpio_free(SX1278_1_RST_PIN);
            gpio_free(SX1278_1_DIO0_PIN);
            gpio_free(SX1278_1_DIO1_PIN);
            gpio_free(SX1278_1_DIO2_PIN);
            gpio_free(SX1278_1_DIO3_PIN);
            gpio_free(SX1278_1_DIO4_PIN);
            gpio_free(SX1278_1_DIO5_PIN);
            break;
        case 1:
            gpio_free(SX1278_2_RST_PIN);
            gpio_free(SX1278_2_DIO0_PIN);
            gpio_free(SX1278_2_DIO1_PIN);
            gpio_free(SX1278_2_DIO2_PIN);
            gpio_free(SX1278_2_DIO3_PIN);
            gpio_free(SX1278_2_DIO4_PIN);
            gpio_free(SX1278_2_DIO5_PIN);
            break;
        case 2:
            break;
        default:
            break;
    }
}

void SX1276IoIrqFree(int chip)
{
    switch(chip)
    {
        case 0:
            free_irq(sx1278_1_dio0irq,NULL);
            free_irq(sx1278_1_dio1irq,NULL);
            free_irq(sx1278_1_dio2irq,NULL);
            free_irq(sx1278_1_dio3irq,NULL);
            free_irq(sx1278_1_dio4irq,NULL);
            //free_irq(sx1278_1_dio5irq,NULL);
            break;
        case 1:
            free_irq(sx1278_2_dio0irq,NULL);
            free_irq(sx1278_2_dio1irq,NULL);
            free_irq(sx1278_2_dio2irq,NULL);
            free_irq(sx1278_2_dio3irq,NULL);
            free_irq(sx1278_2_dio4irq,NULL);
            //free_irq(sx1278_2_dio5irq,NULL);
            break;
        case 2:
            break;
        default:
            break;
    }
}