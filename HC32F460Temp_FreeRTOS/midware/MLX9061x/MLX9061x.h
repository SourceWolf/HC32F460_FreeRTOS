#ifndef MLX9061X_H
#define MLX9061X_H
#include "hc32_ddl.h"
void MLX90615_Init(void);
void MLX90615_Read_Temperature(uint16_t *p_data);
float Test90615(void);
#endif