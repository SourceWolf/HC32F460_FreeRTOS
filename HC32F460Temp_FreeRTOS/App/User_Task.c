#include "hc32_ddl.h"//只需要添加这个头文件即可，代码包含裁剪由ddl_config.h设置
#include "cmsis_os.h"
#include "../Tasks/Tasks_include.h"
#include "Test.h"
uint8_t displaydata[4][128],a[100] = {0,1,2,3,4,5,6,7,8,9},b[100];

TaskHandle_t Hd_Task_Start;
void Sleep_init(void)
{
    stc_pwc_pwr_mode_cfg_t  stcPwcPwrMdCfg;
    stc_pwc_wkup_edge_cfg_t stcPwcWkupEdgCfg;

    MEM_ZERO_STRUCT(stcPwcPwrMdCfg);
    MEM_ZERO_STRUCT(stcPwcWkupEdgCfg);
     /* Config power down mode. */
    stcPwcPwrMdCfg.enPwrDownMd = PowerDownMd1;
    stcPwcPwrMdCfg.enRLdo = Enable;
    stcPwcPwrMdCfg.enIoRetain = IoPwrDownRetain;
    stcPwcPwrMdCfg.enRetSram = Disable;
    stcPwcPwrMdCfg.enVHrc = Disable;
    stcPwcPwrMdCfg.enVPll = Disable;
    stcPwcPwrMdCfg.enDynVol =  Voltage09;
    stcPwcPwrMdCfg.enDrvAbility = Ulowspeed;
    stcPwcPwrMdCfg.enPwrDWkupTm = Vcap0047;

    PWC_PowerModeCfg(&stcPwcPwrMdCfg);
}
void Task_START(void *param)
{
	Task_LED_Start();
	Task_Display_Start();
	Task_ADC_Start();
	Task_USB_Start();
	Fs_Task_Start();
//	HW_I2C_Init(I2C1_UNIT,400000);
//	Hw_I2C_Slave_Init(I2C1_UNIT);
//	I2C_Read_data(I2C1_UNIT,0x06,0x00,a,5);
	Testcpp();
	vTaskDelete(Hd_Task_Start);
    while(1)
    {
//		I2C_Write_Buffer(I2C1_UNIT,0x06,a,80);
//		vTaskDelay(100/portTICK_PERIOD_MS);
//		I2C_Read_Buffer(I2C1_UNIT,0x06,b,80);
        vTaskDelay(100/portTICK_PERIOD_MS);
    }
}
void User_Task_Create(void)
{
	xTaskCreate(Task_START,(const char *)"LED", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+3, &Hd_Task_Start );		    
    vTaskStartScheduler();
}
