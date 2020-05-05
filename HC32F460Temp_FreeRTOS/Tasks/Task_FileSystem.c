#include "../Tasks/Task_FileSystem.h"
FATFS FatFs;
FRESULT fr;
FIL Myfile;
char line[82];
TaskHandle_t H_Task_Fs;
#define STACKSIZE_FS	configMINIMAL_STACK_SIZE
#define PRIORITY_TASKFS	(tskIDLE_PRIORITY+3)
static void Task_FS_Operation(void* param)
{
	disk_initialize(SD_Card);
    f_mount(SD_Card,&FatFs);//Çý¶¯Æ÷0
    fr = f_open(&Myfile,"myfile.txt",FA_READ);
    f_gets(line, sizeof line, &Myfile);
        printf("%s\r\n",line);
    f_close(&Myfile);
	vTaskDelete(H_Task_Fs);
	while(1)
		{
			vTaskDelay(100/portTICK_PERIOD_MS);
		}
}


uint8_t Fs_Task_Start(void)
{
	xTaskCreate(Task_FS_Operation,(const char *)"File System Task",STACKSIZE_FS,NULL,PRIORITY_TASKFS,&H_Task_Fs);
	return Ok;
}























