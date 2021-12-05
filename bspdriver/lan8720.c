#include "pcf8574.h"
#include "main.h"
extern ETH_HandleTypeDef heth;

void LAN8720_power_on(void)
{
	PCF8574_WriteBit(ETH_RESET_IO, 1);
	osDelay(100);
	PCF8574_WriteBit(ETH_RESET_IO, 0);
}


void LAN8720_Init(void)
{
	PCF8574_WriteBit(ETH_RESET_IO, 1);
	osDelay(100);
	PCF8574_WriteBit(ETH_RESET_IO, 0);
	osDelay(100);
	/* USER CODE BEGIN ETH_Init 0 */

	/* USER CODE END ETH_Init 0 */

	static uint8_t MACAddr[6];

	/* USER CODE BEGIN ETH_Init 1 */

	/* USER CODE END ETH_Init 1 */
	heth.Instance = ETH;
	heth.Init.AutoNegotiation = ETH_AUTONEGOTIATION_ENABLE;
	heth.Init.Speed = ETH_SPEED_100M;
	heth.Init.DuplexMode = ETH_MODE_FULLDUPLEX;
	heth.Init.PhyAddress = 0;
	MACAddr[0] = 0x00;
	MACAddr[1] = 0x80;
	MACAddr[2] = 0xE1;
	MACAddr[3] = 0x00;
	MACAddr[4] = 0x00;
	MACAddr[5] = 0x00;
	heth.Init.MACAddr = &MACAddr[0];
	heth.Init.RxMode = ETH_RXINTERRUPT_MODE;
	heth.Init.ChecksumMode = ETH_CHECKSUM_BY_HARDWARE;
	heth.Init.MediaInterface = ETH_MEDIA_INTERFACE_RMII;

	/* USER CODE BEGIN MACADDRESS */

	/* USER CODE END MACADDRESS */

	if (HAL_ETH_Init(&heth) != HAL_OK) {
		Error_Handler();
	}
}
