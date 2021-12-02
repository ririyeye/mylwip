#include <stdint.h>
#include <stddef.h>
#include <rt_misc.h>
#include "main.h"
#include "cmsis_os2.h"
#include <string.h>
#define u32 uint32_t
#define u16 uint16_t
#define Bank5_SDRAM_ADDR ((u32)(0XC0000000))
#define SDRAM_SIZE (32 * 1024 * 1024)
#define SDRAM_MODEREG_BURST_LENGTH_1 ((u16)0x0000)
#define SDRAM_MODEREG_BURST_LENGTH_2 ((u16)0x0001)
#define SDRAM_MODEREG_BURST_LENGTH_4 ((u16)0x0002)
#define SDRAM_MODEREG_BURST_LENGTH_8 ((u16)0x0004)
#define SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL ((u16)0x0000)
#define SDRAM_MODEREG_BURST_TYPE_INTERLEAVED ((u16)0x0008)
#define SDRAM_MODEREG_CAS_LATENCY_2 ((u16)0x0020)
#define SDRAM_MODEREG_CAS_LATENCY_3 ((u16)0x0030)
#define SDRAM_MODEREG_OPERATING_MODE_STANDARD ((u16)0x0000)
#define SDRAM_MODEREG_WRITEBURST_MODE_PROGRAMMED ((u16)0x0000)
#define SDRAM_MODEREG_WRITEBURST_MODE_SINGLE ((u16)0x0200)

static int allocflg = 0;

unsigned __rt_heap_extend(unsigned size, void **block)
{
	if (allocflg == 0) {
		allocflg = 1;
		*block = (char *)Bank5_SDRAM_ADDR;
		memset(*block, 0, SDRAM_SIZE);
		return SDRAM_SIZE;
	}

	return 0;
}

extern SDRAM_HandleTypeDef hsdram1;
unsigned char SDRAM_Send_Cmd(unsigned char bankx, unsigned char cmd, unsigned char refresh, unsigned short regval)
{
	unsigned int target_bank = 0;
	FMC_SDRAM_CommandTypeDef Command;

	if (bankx == 0)
		target_bank = FMC_SDRAM_CMD_TARGET_BANK1;
	else if (bankx == 1)
		target_bank = FMC_SDRAM_CMD_TARGET_BANK2;
	Command.CommandMode = cmd; //命令
	Command.CommandTarget = target_bank; //目标SDRAM存储区域
	Command.AutoRefreshNumber = refresh; //自刷新次数
	Command.ModeRegisterDefinition = regval; //要写入模式寄存器的值
	if (HAL_SDRAM_SendCommand(&hsdram1, &Command, 0X1000) == HAL_OK) //向SDRAM发送命令
	{
		return 0;
	} else
		return 1;
}

void mysdraminit()
{
	// SDRAM控制器初始化完成以后还需要按照如下顺序初始化SDRAM
	SDRAM_Send_Cmd(0, FMC_SDRAM_CMD_CLK_ENABLE, 1, 0); //时钟配置使能
	osDelay(10); //至少延时200us
	SDRAM_Send_Cmd(0, FMC_SDRAM_CMD_PALL, 1, 0); //对所有存储区预充电
	SDRAM_Send_Cmd(0, FMC_SDRAM_CMD_AUTOREFRESH_MODE, 8, 0); //设置自刷新次数
		//配置模式寄存器,SDRAM的bit0~bit2为指定突发访问的长度，
	// bit3为指定突发访问的类型，bit4~bit6为CAS值，bit7和bit8为运行模式
	// bit9为指定的写突发模式，bit10和bit11位保留位
	uint32_t temp = (uint32_t)SDRAM_MODEREG_BURST_LENGTH_1 | //设置突发长度:1(可以是1/2/4/8)
			SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL | //设置突发类型:连续(可以是连续/交错)
			SDRAM_MODEREG_CAS_LATENCY_3 | //设置CAS值:3(可以是2/3)
			SDRAM_MODEREG_OPERATING_MODE_STANDARD | //设置操作模式:0,标准模式
			SDRAM_MODEREG_WRITEBURST_MODE_SINGLE; //设置突发写模式:1,单点访问
	SDRAM_Send_Cmd(0, FMC_SDRAM_CMD_LOAD_MODE, 1, temp); //设置SDRAM的模式寄存器

	//刷新频率计数器(以SDCLK频率计数),计算方法:
	// COUNT=SDRAM刷新周期/行数-20=SDRAM刷新周期(us)*SDCLK频率(Mhz)/行数
	//我们使用的SDRAM刷新周期为64ms,SDCLK=180/2=90Mhz,行数为8192(2^13).
	//所以,COUNT=64*1000*90/8192-20=683
	HAL_SDRAM_ProgramRefreshRate(&hsdram1, 683);
}

#if 0
#include <stdlib.h>
char *testbuff[64];
int index = 0;
int checkerr = 0;
const int testlen = 1000 * 1024;
void testmemory(void)
{
	while (1) {
		char *buff = malloc(testlen);
		if (!buff) {
			break;
		}
		testbuff[index] = buff;
		memset(testbuff[index], index, testlen);
		index++;
	}

	for (int i = 0; i < index; i++) {
		for (int check = 0; check < testlen; check++) {
			if (i != testbuff[i][check]) {
				checkerr++;
				break;
			}
		}
	}

	for (int i = 0; i < index; i++) {
		free(testbuff[i]);
		testbuff[i] = 0;
	}
}
#endif
