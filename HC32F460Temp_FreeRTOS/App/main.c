#include "hc32_ddl.h"
#include "System_Clk.h"
#include "User_Task.h"
#include "Uart_DMA.h"
#include "FPU_Enable.h"
USB_OTG_CORE_HANDLE  USB_OTG_dev;
stc_clk_freq_t Clkdata;
int main(void)
{
  FPU_Enable();
  system_clk_init();
  CLK_GetClockFreq(&Clkdata);
  SysTick_Config(Clkdata.hclkFreq/1000);
  NVIC_EnableIRQ(SysTick_IRQn);
  User_Task_Create();
    while(1)
    {
        ;
    }
}
