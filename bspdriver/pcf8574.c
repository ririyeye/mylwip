#include "main.h"
#include "pcf8574.h"
#include <stdint.h>

extern I2C_HandleTypeDef hi2c2;
//初始化PCF8574
uint8_t PCF8574_Init(void)
{
	unsigned char send = 0xff;
	int ret = HAL_I2C_Master_Transmit(&hi2c2, PCF8574_ADDR, &send, 1, 1000);
	return ret;
}

//读取PCF8574的8位IO值
//返回值:读到的数据
uint8_t PCF8574_ReadOneByte(void)
{
	unsigned char tmp;
	HAL_I2C_Master_Receive(&hi2c2, PCF8574_ADDR, &tmp, 1, 1000);
	return tmp;
}
//向PCF8574写入8位IO值
//DataToWrite:要写入的数据
void PCF8574_WriteOneByte(uint8_t DataToWrite)
{
	HAL_I2C_Master_Transmit(&hi2c2, PCF8574_ADDR, &DataToWrite, 1, 1000);
}

//设置PCF8574某个IO的高低电平
//bit:要设置的IO编号,0~7
//sta:IO的状态;0或1
void PCF8574_WriteBit(uint8_t bit, uint8_t sta)
{
	uint8_t data;
	data = PCF8574_ReadOneByte(); //先读出原来的设置
	if (sta == 0)
		data &= ~(1 << bit);
	else
		data |= 1 << bit;
	PCF8574_WriteOneByte(data); //写入新的数据
}

//读取PCF8574的某个IO的值
//bit：要读取的IO编号,0~7
//返回值:此IO的值,0或1
uint8_t PCF8574_ReadBit(uint8_t bit)
{
	uint8_t data;
	data = PCF8574_ReadOneByte(); //先读取这个8位IO的值
	if (data & (1 << bit))
		return 1;
	else
		return 0;
}
