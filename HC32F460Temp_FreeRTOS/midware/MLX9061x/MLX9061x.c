#include "HW_I2C.h"
#define MLX90615_I2C_Init	HW_I2C_Init
#define MLX90615_I2CPort_Init HW_I2C_Port_Init
#define MLX90615_Addr	0x5B		//0xB6
#define MLX90615_RAM	0x20		
#define ADD_RAW_data	0x05
#define ADD_TA			0x06
#define ADD_TO			0x07
static uint16_t t_test,ta;
static float temperature;
void MLX90615_Init(void)
{
	MLX90615_I2CPort_Init();
	MLX90615_I2C_Init(I2C1_UNIT,400000);
}
void MLX90615_Read_Temperature(uint16_t *p_data)
{
	I2C_Read_data(I2C1_UNIT,MLX90615_Addr,MLX90615_RAM+ADD_TO,(uint8_t*)p_data,2);
}
void MLX90615_Read_TA(uint16_t *p_data)
{
	I2C_Read_data(I2C1_UNIT,MLX90615_Addr,MLX90615_RAM+ADD_TA,(uint8_t*)p_data,2);
}
float Test90615(void)
{
	MLX90615_Read_Temperature(&t_test);
	MLX90615_Read_TA(&ta);
	temperature = t_test;
	temperature = temperature * 0.02 - 273.15+2.5;
	return temperature;
}