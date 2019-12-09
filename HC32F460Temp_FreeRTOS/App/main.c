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
//USB_OTG_CORE_HANDLE  USB_OTG_dev;
stc_clk_freq_t Clkdata;
int main(void)
{
    
#if defined (__CC_ARM) && defined (__TARGET_FPU_VFP)
SCB->CPACR |= 0x00F00000;
#endif    
  system_clk_init();
  CLK_GetClockFreq(&Clkdata);
  SysTick_Config(Clkdata.hclkFreq/1000);
  NVIC_EnableIRQ(SysTick_IRQn);
//  USBD_Init(&USB_OTG_dev,
//#ifdef USE_USB_OTG_FS
//              USB_OTG_FS_CORE_ID,
//#else
//              USB_OTG_HS_CORE_ID,
//#endif
//              &USR_desc,
//              &USBD_HID_cb,
//              &USR_cb);
  User_Task_Create();
    while(1)
    {
         
        ;
    }
}
