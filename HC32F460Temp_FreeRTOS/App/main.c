#include "hc32_ddl.h"
#include "System_Clk.h"
#include "User_Task.h"
#include "Uart_DMA.h"
#include "FPU_Enable.h"
#include "System_PowerDown.h"
#include "User_Timer.h"
#include "usbd_usr.h"
#include "usbd_desc.h"
#include "usb_bsp.h"
#include "Hw_Uart1.h"
#include "Hw_MPU.h"
#include "User_I2S.h"
#include "User_SDIO.h"
#include "hd_sdio.h"
#include "SEGGER_RTT.h"
#include "User_Gpio.h"
#include "Hw_SPI3.h"
#include "Hw_I2C.h"
#include "AT24C02.h"
USB_OTG_CORE_HANDLE  USB_OTG_dev;
stc_clk_freq_t Clkdata;
#ifdef APP_VERSION
#define APP_START_ADDRESS 0x40000
#endif 
char buffer1[100] = "abcde";
stc_pll_clk_freq_t pllfreq;
int main(void)
{
#ifdef APP_VERSION
	 __set_MSP(*(uint32_t *) APP_START_ADDRESS);

	/* Rebase the vector table base address */
	SCB->VTOR = ((uint32_t) APP_START_ADDRESS & SCB_VTOR_TBLOFF_Msk); 
#endif    
#if defined (__CC_ARM) && defined (__TARGET_FPU_VFP)
	SCB->CPACR |= 0x00F00000;
#endif 
	system_clk_init();
	Ddl_Delay1ms(2000);  
    CLK_LrcCmd(Disable);
    CLK_Xtal32Cmd(Disable);
	CLK_GetClockFreq(&Clkdata);
	SysTick_Config(Clkdata.hclkFreq/1000);
	NVIC_EnableIRQ(SysTick_IRQn);
	Ddl_UartInit();
	Hw_MPU_Init();
	CLK_GetPllClockFreq(&pllfreq);
	printf("system base software initialed.--------%s, %d\r\n",__FILE__, __LINE__);
//    Hw_SPI3_Init();
//    HW_I2C_Port_Init();
//	HW_I2C_Init(I2C1_UNIT,300000);
	User_Task_Create();
    while(1)
    {   
//        SPI_TX_8bit((uint8_t*)buffer1+10,(uint8_t*)"1234567890",10);
//        TestEEPROM();
//        Ddl_Delay1ms(100);
//        LPM_TEST();
		;
    }
}


