#include "hc32_ddl.h"//只需要添加这个头文件即可，代码包含裁剪由ddl_config.h设置
#include "User_Gpio.h"
#include "User_Timer.h"
#include "User_Uart.h"
#include "System_Clk.h"
#include "User_ADC.h"
#include "User_I2C.h"
#include "User_SPI.h"
#include "User_DMA.h"
#include "User_RTC.h"
#include "User_OTS.h"
#include "User_Timer4.h"
#include "cmsis_os.h"
#include "User_I2S.h"
#include "User_Timer6.h"
#include "USer_TRNG.h"
#include "User_Crypto.h"
#include "OLED.h"
//#include "User_FFT.h"
#include "User_DCU.h"
#include "User_PWC.h"
#include "User_CAN.h"
#include "User_QSPI.h"
//#include "User_UART.h"
#include "Uart_DMA.h"
#include "ff.h"
#include "diskio.h"
uint8_t displaydata[4][128];
TaskHandle_t Hd_Task_LED, Hd_Task_ADC,Hd_Task_Sleep;
void Task_ADC(void *param);
char line[82];
FATFS FatFs;
FRESULT fr;
FIL Myfile; 
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
void loop(void)
{
    LED0_Toggle();
}
void Task_LED(void *param)
{ 
    User_Gpio_Init();
    OLED_Init();
    User_OTS_Init();
//    User_SPI_Init();
    Ddl_UartInit();
    User_Timer4_init();
    taskENTER_CRITICAL();
    OLED_ShowString(48,0,(uint8_t *)"HDSC");
    OLED_ShowString(32,2,(uint8_t *)"HC32F460");
    printf("system Initailed!\r\n");
    Print_CPU_Temperature();
    taskEXIT_CRITICAL();
    disk_initialize(SD_Card);
    f_mount(SD_Card,&FatFs);//驱动器0
    fr = f_open(&Myfile,"myfile.txt",FA_READ);
    f_gets(line, sizeof line, &Myfile);
        printf("%s\r\n",line);
    f_close(&Myfile);
//    User_SPI_Init();
//    User_USART_Init();
    xTaskCreate(Task_ADC,(const char *)"ADC", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+4, &Hd_Task_ADC );
    while(1)
    {        
        loop();
        vTaskDelay(1000/portTICK_PERIOD_MS);
    }
}
void Task_ADC(void *param)
{
    User_DCU_Init();
    User_ADC_Init();
    User_DMA_Init();
//    User_CAN_Init();
//    User_QSPI_Flash_Init();
//    Test_QSPI();
    taskENTER_CRITICAL();
    printf("AIN10 data:%d\r\n",Get_DCU1_Result()-1000);
    taskEXIT_CRITICAL();    
    while(1)
    {      
 //       User_CAN_Test(); 
        ADC1_Start_convert();
        if(flag_DCU1_INT)
        {
            flag_DCU1_INT = false;
//            printf("DCU_INT occur!\r\n");
        } 
        taskENTER_CRITICAL();
        printf("AIN10 data:%d\r\n",Get_DCU1_Result()-1000);
        taskEXIT_CRITICAL();   
        vTaskDelay(1000/portTICK_PERIOD_MS);
    }
}

void User_Task_Create(void)
{
    xTaskCreate(Task_LED,(const char *)"LED", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+3, &Hd_Task_LED );
//    xTaskCreate(Task_Sleep,(const char *)"sleep", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+5, &Hd_Task_Sleep );
    vTaskStartScheduler();
}
