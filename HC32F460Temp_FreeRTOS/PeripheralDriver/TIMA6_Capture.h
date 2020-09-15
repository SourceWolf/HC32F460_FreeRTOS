#ifndef TIMA6_CPT_H
#define TIMA6_CPT_H
#include "hc32_ddl.h"

#define UNIT_TIMA6          M4_TMRA6
#define PORT_CPT            PortD
#define PIN_CPT             Pin04
#define Func_CPT            Func_Tima1
#define CLK_TIMA6           PWC_FCG2_PERIPH_TIMA6
#define CH8_TIMA6           TimeraCh8
#define INT_CH8_TIMA6       TimeraIrqCaptureOrCompareCh8
#define INT_OVF_TIMA6       TimeraIrqOverflow
#define INT_SOURCE_TIMA6    INT_TMRA6_CMP


#define UNIT_TIMA5                  M4_TMRA5
#define CLK_TIMA5                   PWC_FCG2_PERIPH_TIMA5
#define TIMERA5_UNITH_OVERFLOW_INT  INT_TMRA2_OVF
void Timera6_CPT_Config(void);
void TimerA5_config(void);

#endif





