#include <time.h>
#include <modbus/modbus.h>
#include "lora_pkg.h"
#include "typedef.h"
#include "LoRaDevOps.h"
#include "GatewayPragma.h"
#include "Server.h"

st_MjDevInfo stMjDevInfo[MAX_DEV_ID];
extern int fd_cdev;
extern modbus_mapping_t *mb_mapping;

pthread_t get_tid_by_netaddr(uint32_t DevAddr)
{
	uint16_t loop;
	for(loop = 0;loop < MAX_DEV_ID;loop ++)
	{
		if(stMjDevInfo[loop].DevAddr == DevAddr)
		{
			return loop;
		}
	}
	return -1;
}

void *mjlora_pkg_routin(void *data)
{
	int len;
	uint8_t readbuffer[256 + sizeof(st_ServerMsgUp)];
	pst_ServerMsgUp pstServerMsgUp = (pst_ServerMsgUp)readbuffer;
	pst_MjLoraPkgUpV1 pstMjLoraPkgUpV1 = NULL;
	pst_MjLoraPkgUpV2 pstMjLoraPkgUpV2 = NULL;
	uint16_t devidinpkg;
	struct tm *local;
	time_t tt;
	pthread_t pthreadtokill;

	tzset();//void tzset(void);设置时间环境变量-时区
	while(1)
	{
		if((len = read(fd_cdev,readbuffer,sizeof(readbuffer))) > 0)
		{
			if(mb_mapping == NULL)
			{
				continue;
			}
			if(pstServerMsgUp->enMsgUpFramType == en_MsgUpFramDataReceive)
			{
				if(pstServerMsgUp->Msg.stData2Server.CtrlBits.Ack)
				{
					pthreadtokill = get_tid_by_netaddr(pstServerMsgUp->Msg.stData2Server.DevAddr);
					pthread_detach(pthreadtokill);
					pthread_kill(pthreadtokill,0);
				}
				if(pstServerMsgUp->Msg.stData2Server.size)
				{
					continue;
				}
				pstServerMsgUp->Msg.stData2Server.payload = &readbuffer[sizeof(st_ServerMsgUp)];
				tt=time(NULL);//等价于time(&tt);
				local=localtime(&tt);
				if(pstServerMsgUp->Msg.stData2Server.fPort == UP_DATA_PORT_V1)
				{
					pstMjLoraPkgUpV1 = (pst_MjLoraPkgUpV1)pstServerMsgUp->Msg.stData2Server.payload;
					devidinpkg = pstMjLoraPkgUpV1->devid;
					stMjDevInfo[devidinpkg].fPort = pstServerMsgUp->Msg.stData2Server.fPort;
					stMjDevInfo[devidinpkg].devid = devidinpkg;
					stMjDevInfo[devidinpkg].DevAddr = pstServerMsgUp->Msg.stData2Server.DevAddr;
					stMjDevInfo[devidinpkg].AckRequest = pstServerMsgUp->Msg.stData2Server.CtrlBits.AckRequest;
					stMjDevInfo[devidinpkg].ClassType = pstServerMsgUp->Msg.stData2Server.ClassType;
					stMjDevInfo[devidinpkg].rssi = pstServerMsgUp->Msg.stData2Server.rssi;
					stMjDevInfo[devidinpkg].snr = pstServerMsgUp->Msg.stData2Server.snr;
					stMjDevInfo[devidinpkg].Ack = pstServerMsgUp->Msg.stData2Server.CtrlBits.Ack;
					mb_mapping->tab_input_registers[1 + devidinpkg * 25 - 1] = local->tm_year + 1900;
					mb_mapping->tab_input_registers[2 + devidinpkg * 25 - 1] = local->tm_mon + 1;
					mb_mapping->tab_input_registers[3 + devidinpkg * 25 - 1] = local->tm_mday;
					mb_mapping->tab_input_registers[4 + devidinpkg * 25 - 1] = local->tm_hour;
					mb_mapping->tab_input_registers[5 + devidinpkg * 25 - 1] = local->tm_min;
					mb_mapping->tab_input_registers[6 + devidinpkg * 25 - 1] = local->tm_sec;
					mb_mapping->tab_input_registers[7 + devidinpkg * 25 - 1] = stMjDevInfo[devidinpkg].rssi;
					mb_mapping->tab_input_registers[8 + devidinpkg * 25 - 1] = stMjDevInfo[devidinpkg].snr;
					mb_mapping->tab_input_registers[9 + devidinpkg * 25 - 1] = pstMjLoraPkgUpV1->Temperature;
					mb_mapping->tab_input_registers[10 + devidinpkg * 25 - 1] = pstMjLoraPkgUpV1->Temperature;
					mb_mapping->tab_input_registers[11 + devidinpkg * 25 - 1] = pstMjLoraPkgUpV1->humidity;
					mb_mapping->tab_input_registers[12 + devidinpkg * 25 - 1] = pstMjLoraPkgUpV1->humidity;
					pthread_create(&stMjDevInfo[devidinpkg].datadownprocess, NULL, mjlora_data_down_routin, &devidinpkg);
				}
				else if(pstServerMsgUp->Msg.stData2Server.fPort == UP_DATA_PORT_V2)
				{
					pstMjLoraPkgUpV2 = (pst_MjLoraPkgUpV2)pstServerMsgUp->Msg.stData2Server.payload;
					devidinpkg = pstMjLoraPkgUpV2->devid;
					stMjDevInfo[devidinpkg].fPort = pstServerMsgUp->Msg.stData2Server.fPort;
					stMjDevInfo[devidinpkg].devid = devidinpkg;
					stMjDevInfo[devidinpkg].DevAddr = pstServerMsgUp->Msg.stData2Server.DevAddr;
					stMjDevInfo[devidinpkg].AckRequest = pstServerMsgUp->Msg.stData2Server.CtrlBits.AckRequest;
					stMjDevInfo[devidinpkg].ClassType = pstServerMsgUp->Msg.stData2Server.ClassType;
					stMjDevInfo[devidinpkg].rssi = pstServerMsgUp->Msg.stData2Server.rssi;
					stMjDevInfo[devidinpkg].snr = pstServerMsgUp->Msg.stData2Server.snr;
					stMjDevInfo[devidinpkg].Ack = pstServerMsgUp->Msg.stData2Server.CtrlBits.Ack;
					mb_mapping->tab_input_registers[1 + devidinpkg * 25 - 1] = local->tm_year + 1900;
					mb_mapping->tab_input_registers[2 + devidinpkg * 25 - 1] = local->tm_mon + 1;
					mb_mapping->tab_input_registers[3 + devidinpkg * 25 - 1] = local->tm_mday;
					mb_mapping->tab_input_registers[4 + devidinpkg * 25 - 1] = local->tm_hour;
					mb_mapping->tab_input_registers[5 + devidinpkg * 25 - 1] = local->tm_min;
					mb_mapping->tab_input_registers[6 + devidinpkg * 25 - 1] = local->tm_sec;
					mb_mapping->tab_input_registers[7 + devidinpkg * 25 - 1] = stMjDevInfo[devidinpkg].rssi;
					mb_mapping->tab_input_registers[8 + devidinpkg * 25 - 1] = stMjDevInfo[devidinpkg].snr;
					mb_mapping->tab_input_registers[9 + devidinpkg * 25 - 1] = pstMjLoraPkgUpV2->Temperature[0];
					mb_mapping->tab_input_registers[10 + devidinpkg * 25 - 1] = pstMjLoraPkgUpV2->Temperature[1];
					mb_mapping->tab_input_registers[11 + devidinpkg * 25 - 1] = pstMjLoraPkgUpV2->humidity[0];
					mb_mapping->tab_input_registers[12 + devidinpkg * 25 - 1] = pstMjLoraPkgUpV2->humidity[1];
					mb_mapping->tab_input_registers[13 + devidinpkg * 25 - 1] = pstMjLoraPkgUpV2->pressure[0];
					mb_mapping->tab_input_registers[14 + devidinpkg * 25 - 1] = pstMjLoraPkgUpV2->pressure[1];
					mb_mapping->tab_input_registers[15 + devidinpkg * 25 - 1] = pstMjLoraPkgUpV2->pressure[2];
					mb_mapping->tab_input_registers[16 + devidinpkg * 25 - 1] = pstMjLoraPkgUpV2->pressure[3];
					mb_mapping->tab_input_registers[17 + devidinpkg * 25 - 1] = pstMjLoraPkgUpV2->pressure[4];
					mb_mapping->tab_input_registers[18 + devidinpkg * 25 - 1] = pstMjLoraPkgUpV2->pressure[5];
					mb_mapping->tab_input_registers[19 + devidinpkg * 25 - 1] = pstMjLoraPkgUpV2->pressure[6];
					mb_mapping->tab_input_registers[20 + devidinpkg * 25 - 1] = pstMjLoraPkgUpV2->pressure[7];
					mb_mapping->tab_input_registers[21 + devidinpkg * 25 - 1] = pstMjLoraPkgUpV2->pressure[8];
					mb_mapping->tab_input_registers[22 + devidinpkg * 25 - 1] = pstMjLoraPkgUpV2->pressure[9];
					mb_mapping->tab_input_registers[23 + devidinpkg * 25 - 1] = pstMjLoraPkgUpV2->reserve[0];
					mb_mapping->tab_input_registers[24 + devidinpkg * 25 - 1] = pstMjLoraPkgUpV2->reserve[1];
					mb_mapping->tab_input_registers[25 + devidinpkg * 25 - 1] = pstMjLoraPkgUpV2->Voltage;
					pthread_create(&stMjDevInfo[devidinpkg].datadownprocess, NULL, mjlora_data_down_routin, &devidinpkg);
				}
				else
				{

				}
			}
		}
		else
		{
			usleep(100000);
		}
	}
}

void *mjlora_data_down_routin(void *data)
{
	uint16_t devid = *(uint16_t*)data;
	uint8_t writebuf[256 + sizeof(st_ServerMsgDown)];
	uint32_t sendlen;
	pst_ServerMsgDown pstServerMsgDown;

	usleep(900000);
	pstServerMsgDown = (pst_ServerMsgDown)writebuf;
	pstServerMsgDown->enMsgDownFramType = en_MsgDownFramDataSend;
	pstServerMsgDown->Msg.stData2Node.payload = writebuf + sizeof(st_ServerMsgDown);
	pstServerMsgDown->Msg.stData2Node.DevAddr = stMjDevInfo[devid].DevAddr;
	pstServerMsgDown->Msg.stData2Node.fPort = stMjDevInfo[devid].fPort;
	pstServerMsgDown->Msg.stData2Node.CtrlBits.AckRequest = true;
	if(stMjDevInfo[devid].AckRequest)
	{
		pstServerMsgDown->Msg.stData2Node.CtrlBits.Ack = true;
	}
	else
	{
		pstServerMsgDown->Msg.stData2Node.CtrlBits.Ack = false;
	}
	if(stMjDevInfo[devid].fPort == UP_DATA_PORT_V1)
	{
		pstServerMsgDown->Msg.stData2Node.size = 5;
		pstServerMsgDown->Msg.stData2Node.payload[0] = 0x06;
		pstServerMsgDown->Msg.stData2Node.payload[1] = (uint8_t)mb_mapping->tab_registers[1 + devid * 25 - 1] >> 8;
		pstServerMsgDown->Msg.stData2Node.payload[2] = (uint8_t)(mb_mapping->tab_registers[1 + devid * 25 - 1] & 0xff);
		pstServerMsgDown->Msg.stData2Node.payload[3] = (uint8_t)0x00; // crc
		pstServerMsgDown->Msg.stData2Node.payload[4] = (uint8_t)0x00; // crc
	}
	else if(stMjDevInfo[devid].fPort == UP_DATA_PORT_V2)
	{
		pstServerMsgDown->Msg.stData2Node.size = 11;
		pstServerMsgDown->Msg.stData2Node.payload[0] = 0x10;
		pstServerMsgDown->Msg.stData2Node.payload[1] = (uint8_t)mb_mapping->tab_registers[1 + devid * 25 - 1] >> 8;
		pstServerMsgDown->Msg.stData2Node.payload[2] = (uint8_t)(mb_mapping->tab_registers[1 + devid * 25 - 1] & 0xff);
		pstServerMsgDown->Msg.stData2Node.payload[3] = (uint8_t)mb_mapping->tab_registers[2 + devid * 25 - 1] >> 8;
		pstServerMsgDown->Msg.stData2Node.payload[4] = (uint8_t)(mb_mapping->tab_registers[2 + devid * 25 - 1] & 0xff);
		pstServerMsgDown->Msg.stData2Node.payload[5] = (uint8_t)mb_mapping->tab_registers[3 + devid * 25 - 1] >> 8;
		pstServerMsgDown->Msg.stData2Node.payload[6] = (uint8_t)(mb_mapping->tab_registers[3 + devid * 25 - 1] & 0xff);
		pstServerMsgDown->Msg.stData2Node.payload[7] = (uint8_t)mb_mapping->tab_registers[4 + devid * 25 - 1] >> 8;
		pstServerMsgDown->Msg.stData2Node.payload[8] = (uint8_t)(mb_mapping->tab_registers[4 + devid * 25 - 1] & 0xff);
		pstServerMsgDown->Msg.stData2Node.payload[9] = (uint8_t)0x00; // crc
		pstServerMsgDown->Msg.stData2Node.payload[10] = (uint8_t)0x00; // crc
	}
	else
	{
		pstServerMsgDown->Msg.stData2Node.size = 0;
	}
	sendlen = pstServerMsgDown->Msg.stData2Node.size + sizeof(st_ServerMsgDown);
	
	write(fd_cdev,writebuf,sendlen);

	usleep(1000000); // retry in 2nd rx window
	pstServerMsgDown = (pst_ServerMsgDown)writebuf;
	pstServerMsgDown->enMsgDownFramType = en_MsgDownFramDataSend;
	pstServerMsgDown->Msg.stData2Node.payload = writebuf + sizeof(st_ServerMsgDown);
	pstServerMsgDown->Msg.stData2Node.DevAddr = stMjDevInfo[devid].DevAddr;
	pstServerMsgDown->Msg.stData2Node.fPort = stMjDevInfo[devid].fPort;
	pstServerMsgDown->Msg.stData2Node.CtrlBits.AckRequest = true;
	if(stMjDevInfo[devid].AckRequest)
	{
		pstServerMsgDown->Msg.stData2Node.CtrlBits.Ack = true;
	}
	else
	{
		pstServerMsgDown->Msg.stData2Node.CtrlBits.Ack = false;
	}
	if(stMjDevInfo[devid].fPort == UP_DATA_PORT_V1)
	{
		pstServerMsgDown->Msg.stData2Node.size = 5;
		pstServerMsgDown->Msg.stData2Node.payload[0] = 0x06;
		pstServerMsgDown->Msg.stData2Node.payload[1] = (uint8_t)mb_mapping->tab_registers[1 + devid * 25 - 1] >> 8;
		pstServerMsgDown->Msg.stData2Node.payload[2] = (uint8_t)(mb_mapping->tab_registers[1 + devid * 25 - 1] & 0xff);
		pstServerMsgDown->Msg.stData2Node.payload[3] = (uint8_t)0x00; // crc
		pstServerMsgDown->Msg.stData2Node.payload[4] = (uint8_t)0x00; // crc
	}
	else if(stMjDevInfo[devid].fPort == UP_DATA_PORT_V2)
	{
		pstServerMsgDown->Msg.stData2Node.size = 11;
		pstServerMsgDown->Msg.stData2Node.payload[0] = 0x10;
		pstServerMsgDown->Msg.stData2Node.payload[1] = (uint8_t)mb_mapping->tab_registers[1 + devid * 25 - 1] >> 8;
		pstServerMsgDown->Msg.stData2Node.payload[2] = (uint8_t)(mb_mapping->tab_registers[1 + devid * 25 - 1] & 0xff);
		pstServerMsgDown->Msg.stData2Node.payload[3] = (uint8_t)mb_mapping->tab_registers[2 + devid * 25 - 1] >> 8;
		pstServerMsgDown->Msg.stData2Node.payload[4] = (uint8_t)(mb_mapping->tab_registers[2 + devid * 25 - 1] & 0xff);
		pstServerMsgDown->Msg.stData2Node.payload[5] = (uint8_t)mb_mapping->tab_registers[3 + devid * 25 - 1] >> 8;
		pstServerMsgDown->Msg.stData2Node.payload[6] = (uint8_t)(mb_mapping->tab_registers[3 + devid * 25 - 1] & 0xff);
		pstServerMsgDown->Msg.stData2Node.payload[7] = (uint8_t)mb_mapping->tab_registers[4 + devid * 25 - 1] >> 8;
		pstServerMsgDown->Msg.stData2Node.payload[8] = (uint8_t)(mb_mapping->tab_registers[4 + devid * 25 - 1] & 0xff);
		pstServerMsgDown->Msg.stData2Node.payload[9] = (uint8_t)0x00; // crc
		pstServerMsgDown->Msg.stData2Node.payload[10] = (uint8_t)0x00; // crc
	}
	else
	{
		pstServerMsgDown->Msg.stData2Node.size = 0;
	}
	sendlen = pstServerMsgDown->Msg.stData2Node.size + sizeof(st_ServerMsgDown);
	
	write(fd_cdev,writebuf,sendlen);
	pthread_detach(pthread_self());
}