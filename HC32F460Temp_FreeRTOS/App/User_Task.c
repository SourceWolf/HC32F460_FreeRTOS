#include "hc32_ddl.h"//只需要添加这个头文件即可，代码包含裁剪由ddl_config.h设置
#include "User_Gpio.h"
#include "User_Timer.h"
//#include "User_Uart.h"
#include "System_Clk.h"
#include "User_ADC.h"
#include "User_I2C.h"
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
#include "usbd_usr.h"
#include "usbd_desc.h"
#include "usb_bsp.h"
#include "Hw_Uart1.h"
#include "NRF24L01.h"
#include "MLX9061x.h"
#include "Hw_TIMERA_Capture.h"
#include "HW_I2C.h"
#include "SEGGER_RTT.h"
//#define Add_UserSystem_Init 0x00040000

USB_OTG_CORE_HANDLE  USB_OTG_dev;
uint8_t displaydata[4][128];
extern USB_OTG_CORE_HANDLE  USB_OTG_dev;
TaskHandle_t Hd_Task_LED, Hd_Task_ADC,Hd_Task_Sleep,Hd_Task_USB,Hd_Task_USBReport;
void Task_ADC(void *param);
//void (*P_UserSystem_Init)(void);
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
	static uint8_t data;
	en_result_t status;
//    User_Gpio_Init();
//	HW_I2C_Init(400000);
//	P_UserSystem_Init = (void *)(Add_UserSystem_Init+4);
//	User_I2C1_Init();
//    OLED_Init();
//    User_OTS_Init();
	NRF24L01_Init();
//    Ddl_UartInit();
//    User_Timer4_init();
//    taskENTER_CRITICAL();
//    OLED_ShowString(0,0,(uint8_t *)"HDSC");
//    OLED_ShowString(32,2,(uint8_t *)"HC32F460");
//    printf("system Initailed!\r\n");
//    Print_CPU_Temperature();
//    taskEXIT_CRITICAL();
//    disk_initialize(SD_Card);
//    f_mount(SD_Card,&FatFs);//驱动器0
//    fr = f_open(&Myfile,"myfile.txt",FA_READ);
//    f_gets(line, sizeof line, &Myfile);
//        printf("%s\r\n",line);
//    f_close(&Myfile);
//	NRF24L01_Read_Reg(0x00,rxdata,1);
//    Hw_Uart1_Init();
//    UART1_TX_DMA_Init();
//    User_SPI_Init();
//    User_USART_Init();
//    xTaskCreate(Task_ADC,(const char *)"ADC", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+4, &Hd_Task_ADC );
//	MLX90615_Init();
//	P_UserSystem_Init();
//	User_I2S3_Init();
//	TimerACaptureInit();
    while(1)
    { 
	Test_NRF24L01_TX();		
//			PORT_Toggle(LED0_PORT,LED0_Pin);
//			User_I2C1_Master_Read(0x32,0x0d,&data,1);

//		OLED_Refresh();
//        loop();
//		OLED_ShowNum(48,0,GetFrequence(),8,16);
//		Test90615();
//		NRF24L01_check();
//		OLED_ShowNum(48,0,Test90615(),2,16);
//		OLED_ShowNum(64,0,(uint32_t)(Test90615()*10)%10,2,16);		
//        UART1_DMA_TX_Write_Buffer(txdata,10);
        vTaskDelay(900/portTICK_PERIOD_MS);
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
void Task_USB_Report(void *param)
{
    int8_t  x=0, y=0;
    static uint8_t HID_Buffer [4];        
    while(1)
    {
        if(Reset == PORT_GetBit(Key0_PORT,Key0_Pin))
        {
            y = -5;
        }
        if(Reset == PORT_GetBit(Key2_PORT,Key2_Pin))
        {
            y = 5;
        }
        if(Reset == PORT_GetBit(Key1_PORT,Key1_Pin))
        {
            x = -5;
        }
        if(Reset == PORT_GetBit(Key3_PORT,Key3_Pin))
        {
            x = 5;
        }
        HID_Buffer[0] = (uint8_t)0;
        HID_Buffer[1] = (uint8_t)x;
        HID_Buffer[2] = (uint8_t)y;
        HID_Buffer[3] = (uint8_t)0;
         if((HID_Buffer[1] != 0u) ||(HID_Buffer[2] != 0u))
        {
            USBD_HID_SendReport (&USB_OTG_dev, HID_Buffer, 4u);
        }
        x = y =0;
        vTaskDelay(100/portTICK_PERIOD_MS);
    }
}
void Task_USB(void *param)
{
    stc_clk_freq_t Clkdata;
    __IO uint32_t test = 0ul;
     USBD_Init(&USB_OTG_dev,
#ifdef USE_USB_OTG_FS
              USB_OTG_FS_CORE_ID,
#else
              USB_OTG_HS_CORE_ID,
#endif
              &USR_desc,
              &USBD_HID_cb,
              &USR_cb);
    CLK_GetClockFreq(&Clkdata);
    xTaskCreate(Task_USB_Report,(const char *)"USB_report", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+3, &Hd_Task_USBReport );
    while(1)
    {
        if(test == 0x1ul)
        {
            USB_OTG_ActiveRemoteWakeup(&USB_OTG_dev);
            test  = 0ul;
        }
        vTaskDelay(1000/portTICK_PERIOD_MS);
    }
}

void User_Task_Create(void)
{
    xTaskCreate(Task_LED,(const char *)"LED", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+3, &Hd_Task_LED );
//    xTaskCreate(Task_Sleep,(const char *)"sleep", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+5, &Hd_Task_Sleep );
//    xTaskCreate(Task_USB,(const char *)"USB", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+3, &Hd_Task_USB );
    
    vTaskStartScheduler();
}
