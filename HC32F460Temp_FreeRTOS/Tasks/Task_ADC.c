#include "../Tasks/Task_ADC.h"

#define STACKSIZE_ADC	configMINIMAL_STACK_SIZE
#define PRIORITY_ADC	(tskIDLE_PRIORITY+3)
TaskHandle_t H_Task_ADC;
QueueHandle_t xQueue_ADC;
FATFS FatFsADC;
FRESULT fr_Adc;
FIL DataFile;
char buffer[200],num;
char string[16];
static void Task_ADC(void* param)
{
	 User_DCU_Init();
    User_ADC_Init();
    User_DMA_Init();
	xQueue_ADC = xQueueCreate(1,sizeof(uint16_t));
//	disk_initialize(SD_Card);
//    fr_Adc = f_mount(SD_Card,&FatFsADC);//������0
//    fr_Adc = f_open(&DataFile,"a.txt",FA_READ|FA_WRITE|FA_CREATE_NEW);
	while(1)
	{
		SEGGER_RTT_printf(0,"%s\r\n",string);
		ADC1_Start_convert();
        if(flag_DCU1_INT)
        {
            flag_DCU1_INT = false;
//            printf("DCU_INT occur!\r\n");
        } 
		insertdisplaydata((Get_DCU1_Result()-1000)/64);
		num = sprintf(string,"ADC: %d",(Get_DCU1_Result()-1000));
		if(num<10)
		{
			for(int i = 0;i<(10-num);i++)
			{
				string[num+i] = ' ';
			}
			string[10] = '\0';
		}
		OLED_ShowString2(0,0,(unsigned char *)string);
//		num = sprintf(buffer,"%d\r\n",Get_DCU1_Result()-1000);
//		f_write(&DataFile,buffer,num,NULL);
        vTaskDelay(10/portTICK_PERIOD_MS);
	}
}
uint8_t Task_ADC_Start(void)
{
	xTaskCreate(Task_ADC,(const char *)"Display Task",STACKSIZE_ADC,NULL,PRIORITY_ADC,&H_Task_ADC);
	return Ok;
}


