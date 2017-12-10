#ifndef __LORA_PROC_H__
#define __LORA_PROC_H__

#include <linux/string.h>
#include "radio.h"
#include "sx1276.h"
#include "sx1276-board.h"
#include <linux/time.h>
#include <linux/timer.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/gpio.h>
#include "pinmap.h"
#include <linux/spinlock.h>
#include "utilities.h"

typedef struct
{
	uint32_t freq_tx[48];
	uint32_t freq_rx[96];
	uint32_t dr_range;
	uint32_t datarate[3];
	uint32_t channel[3];
	RadioModems_t modem;
    int8_t power;
    uint32_t fdev;
    uint32_t bandwidth;
    uint8_t coderate;
    uint16_t preambleLen;
    bool fixLen;
    bool crcOn;
    bool freqHopOn;
    uint8_t hopPeriod;
    bool iqInverted;
    uint32_t timeout;
    uint32_t bandwidthAfc;
    uint16_t symbTimeout;
    uint8_t payloadLen;
    bool rxContinuous;
	bool isPublic;
}st_RadioCfg,*pst_RadioCfg;

extern st_RadioCfg stRadioCfg_Rx,stRadioCfg_Tx;

int init_procfs_lora(void);
void cleanup_procfs_lora(void);

#endif
