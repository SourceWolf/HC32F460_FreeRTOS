#ifndef TIMER43_EXCLK_H
#define TIMER43_EXCLK_H
#include "hc32_ddl.h"
#define PORT_TIM43_EXCLK    PortB
#define PIN_TIM43_EXCLK     Pin03
#define UNIT_TIM43        M4_TMR43

#define TIMER43_IRQn       Int031_IRQn 
void User_Timer43_init(void);

#endif


