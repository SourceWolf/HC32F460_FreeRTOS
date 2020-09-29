#include "hc32_ddl.h"//只需要添加这个头文件即可，代码包含裁剪由ddl_config.h设置
#include "cmsis_os.h"
#include "../Tasks/Tasks_include.h"
#include "System_PowerDown.h"
#include "Test.h"
//#include "MCP33131_SPI_TMR_DMA.h"
#include "User_I2S.h"
#include "AD7689.h"
#include "Hw_Uart4.h"
#include "bsp_I2S_Full_Duplex.h"
uint8_t displaydata[4][128],a[10] = {0,1,2,3,4,5,6,7,8,9},b[10];
char freq[20];
TaskHandle_t Hd_Task_Start;
void Sleep_init(void)
{
    stc_pwc_pwr_mode_cfg_t  stcPwcPwrMdCfg;
//    stc_pwc_wkup_edge_cfg_t stcPwcWkupEdgCfg;

    MEM_ZERO_STRUCT(stcPwcPwrMdCfg);
//    MEM_ZERO_STRUCT(stcPwcWkupEdgCfg);
     /* Config power down mode. */
    stcPwcPwrMdCfg.enPwrDownMd = PowerDownMd1;
    stcPwcPwrMdCfg.enRLdo = Enable;
    stcPwcPwrMdCfg.enIoRetain = IoPwrDownRetain;
    stcPwcPwrMdCfg.enRetSram = Disable;
    stcPwcPwrMdCfg.enVHrc = Disable;
    stcPwcPwrMdCfg.enVPll = Disable;
    stcPwcPwrMdCfg.enRunDrvs =  RunUlowspeed;
    stcPwcPwrMdCfg.enDrvAbility = Ulowspeed;
    stcPwcPwrMdCfg.enPwrDWkupTm = Vcap0047;

    PWC_PowerModeCfg(&stcPwcPwrMdCfg);
}
void Task_START(void *param)
{
//	Task_LED_Start();	
//	Task_Display_Start();
//	Task_ADC_Start(); 
//	Task_USB_Start();
//	Hw_Uart4_Init();
//	HW_I2C_Port_Init();
//	HW_I2C_Init(I2C1_UNIT,400000);
    ////
//    Fs_Task_Start();
//	Hw_I2C_Slave_Init(I2C1_UNIT);
//	User_ADC_Init();
//	Fs_Task_Start();
//	Testcpp();
//	TimerACaptureInit();
//	Hw_SPI3_Init();
//    i2s_record_duplex_init();
//	Hw_TimerA3_Init();
//	User_I2S3_Init();
//	vTaskDelete(Hd_Task_Start);
//	AD7689_Init();
    hwdmx_uartInit();
    while(1)
    {
//        AD7689_SOC();
        Test_UART_TX();
//		LPM_TEST();
//		MEM_ZERO_STRUCT(freq);
//		sprintf(freq,"Freq = %d",GetFrequence());
//		OLED_ShowString2(0,16,(unsigned char *)freq);
//        vTaskDelay(100/portTICK_PERIOD_MS);
//		M4_PORT->POERE_f.POUTE01 = 1;
//		PORT_SetBits(PortE, Pin01);
//		vTaskDelay(10/portTICK_PERIOD_MS);
//		PORT_ResetBits(PortE, Pin01);
//		vTaskDelay(10/portTICK_PERIOD_MS);
//		while(PORT_GetBit(PortE, Pin01) == 1);
//		Hw_SPI3_TEST();
//		TestEEPROM();
//		I2C_Write_Buffer(I2C1_UNIT,0x06,a,5);
//		vTaskDelay(10/portTICK_PERIOD_MS);
//		I2C_Read_data(I2C1_UNIT,0x06,0x00,b,5);
//		ADC1_Start_convert();
//		AD7689_SOC();
//		Test_UART4_TX();
//		SPI_SendData16(SPI3_UNIT,CFG_CHANGE|INCC_GND|CHANNEL0|REF_EXT_TDIS|SEQ_DIS|NOTReadBackCFG);
		vTaskDelay(1000/portTICK_PERIOD_MS);
    }
}
void User_Task_Create(void)
{
	xTaskCreate(Task_START,(const char *)"LED", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+3, &Hd_Task_Start );		    
    vTaskStartScheduler();
}
