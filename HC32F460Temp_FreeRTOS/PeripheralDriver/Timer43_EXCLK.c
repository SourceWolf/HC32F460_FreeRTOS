#include "Timer43_EXCLK.h"
void Timer43_OVF_Callback(void)
{
    TIMER4_CNT_ClearIrqFlag(UNIT_TIM43,Timer4CntPeakMatchInt);
}


void User_Timer43_init(void)
{
    stc_irq_regi_conf_t stcIrqRegiCfg;
    stc_timer4_cnt_init_t stcCntInit;
    stc_port_init_t Port_CFG;
    MEM_ZERO_STRUCT(Port_CFG);
    MEM_ZERO_STRUCT(stcCntInit);
    MEM_ZERO_STRUCT(stcIrqRegiCfg);
    
    PWC_Fcg2PeriphClockCmd(PWC_FCG2_PERIPH_TIM41 | PWC_FCG2_PERIPH_TIM42 | PWC_FCG2_PERIPH_TIM43 , Enable);
    PORT_Unlock();
    M4_PORT->PSPCR &= 0x03;
    PORT_Lock();
    Port_CFG.enPinMode = Pin_Mode_In;
    PORT_Init(PORT_TIM43_EXCLK,PIN_TIM43_EXCLK,&Port_CFG);
        /* Initialize PWM I/O */
    PORT_SetFunc(PORT_TIM43_EXCLK, PIN_TIM43_EXCLK, Func_Tim4, Disable);
    
    stcCntInit.enBufferCmd = Disable;//禁止CPRS缓存
    stcCntInit.enClk = Timer4CntExtclk;//Timer4CntPclk;//Timer4CntExtclk;//时钟源选择
    stcCntInit.enClkDiv = Timer4CntPclkDiv1;//
    stcCntInit.enCntMode = Timer4CntSawtoothWave;//锯齿波模式
    stcCntInit.enPeakIntMsk = Timer4CntIntMask0;//不屏蔽上溢中断
    stcCntInit.enZeroIntMsk = Timer4CntIntMask0;//不屏蔽下溢中断
    stcCntInit.u16Cycle = 1000;//周期
    TIMER4_CNT_Init(UNIT_TIM43, &stcCntInit);
    TIMER4_CNT_IrqCmd(UNIT_TIM43,Timer4CntPeakMatchInt,Enable);//开启上溢中断
    
    stcIrqRegiCfg.enIntSrc = INT_TMR43_GOVF;
    stcIrqRegiCfg.enIRQn = TIMER43_IRQn;
    stcIrqRegiCfg.pfnCallback = Timer43_OVF_Callback;
    enIrqRegistration(&stcIrqRegiCfg);
    NVIC_SetPriority(stcIrqRegiCfg.enIRQn, DDL_IRQ_PRIORITY_DEFAULT);
    NVIC_ClearPendingIRQ(stcIrqRegiCfg.enIRQn);
    NVIC_EnableIRQ(stcIrqRegiCfg.enIRQn);
    /* Clear && Start CNT */
    TIMER4_CNT_ClearCountVal(UNIT_TIM43);
    TIMER4_CNT_Start(UNIT_TIM43);
}

