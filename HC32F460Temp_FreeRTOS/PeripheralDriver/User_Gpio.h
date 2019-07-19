#ifndef USER_GPIO_H
#define USER_GPIO_H
#include "hc32_ddl.h"
extern bool flag_key0,flag_key1;
void User_Gpio_Init(void);
void LED0_Toggle(void);
void LED_Close(void);
void Test_GPIO(void);

#endif
