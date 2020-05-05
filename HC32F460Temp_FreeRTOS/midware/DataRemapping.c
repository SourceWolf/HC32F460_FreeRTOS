#include "DataRemapping.h"
float fdata;
#define MAXVALUE 	60.0f
#define MINVALUE	0.0f
float MappingData[60] = {0,1,2,3,4,5,6,7,8,9,//0-9
						 10,11,12,13,14,15,16,17,18,19,//10-19
						 20,21,22,23,24,25,26,27,28,29,//20-29
						 32,26.5,33,33.5,34,37,38,38.5,39.5,40,//30-39
						 40.5,41,42,43,44,45,46,47,48,49,//40-49
						 50,51,52,53,54,55,56,57,58,59};//50-59
float DataRemapping(float datain)
{
	uint8_t pos;
	float delta;
	float dataout;
	if(datain>MAXVALUE)
	{
		return datain;//不转换
	}
	if(datain<MINVALUE)
	{
		return datain;
	}
	pos = (uint8_t)(datain);//取整数得到位置
	delta = (MappingData[pos+1]-MappingData[pos]);//获取当前区间的delta值
	dataout = (datain-(float)pos)*delta + MappingData[pos];//小数位 X delta + 整数
	return dataout;
}
void Test(void)
{
	fdata = 30;
	for(int i=0;i<200;i++)
	{
		printf("fdata : %f",fdata);
		printf("fdata out : %f\r\n",DataRemapping(fdata));
		fdata += 0.01;
	}
}