#include "../Tasks/Task_FileSystem.h"
FATFS FatFs;
FRESULT fr;
FIL Myfile;
FIL f_Wave;
uint32_t bw,len;
WaveHeader Wavedata;
char line[512];
TaskHandle_t H_Task_Fs;
#define STACKSIZE_FS	configMINIMAL_STACK_SIZE
#define PRIORITY_TASKFS	(tskIDLE_PRIORITY+3)
static void Task_FS_Operation(void* param)
{
	hd_sdio_hw_init();
	disk_initialize(SD_Card);
	wavefileinfo_init(&Wavedata);
    f_mount(SD_Card,&FatFs);//Çý¶¯Æ÷0
    fr = f_open(&Myfile,"myfile.txt",FA_READ|FA_WRITE|FA_CREATE_ALWAYS);
	fr = f_write(&Myfile,&Wavedata,sizeof(Wavedata),&bw);
//    f_gets(line, sizeof line, &Myfile);
//        printf("%s\r\n",line);
   
//	fr = f_open(&f_Wave,"wave.txt",FA_WRITE|FA_READ|FA_CREATE_ALWAYS);
//	f_lseek(&f_Wave,0);	
//	len = sizeof(Wavedata);
//	fr = f_write(&f_Wave,&Wavedata,sizeof(Wavedata),&bw);
	fr = f_write(&Myfile,&au16PixieDustSoundI2s_8,u32WavLen_8k*2,&bw);
	Wavedata.data.ChunkSize = u32WavLen_8k*2;
	Wavedata.riff.ChunkSize = Wavedata.data.ChunkSize+46;
//	f_lseek(&Myfile,0);
//	fr = f_write(&Myfile,&Wavedata,sizeof(Wavedata),&bw);
	 f_close(&Myfile);
//	vTaskDelete(H_Task_Fs);
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























