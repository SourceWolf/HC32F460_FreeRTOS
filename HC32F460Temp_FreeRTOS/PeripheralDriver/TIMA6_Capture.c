#include "TIMA6_Capture.h"
#define Period 33600
uint32_t Cpt_data,Cpt_data_last,Cpt_count = 0;
int32_t Cpt_time;
uint32_t singnal_Freq;
void TIMA6_IrqCallback(void)
{
    if(1 == TIMERA_GetFlag(UNIT_TIMA6,TimeraFlagCaptureOrCompareCh8))
    {
        TIMERA_ClearFlag(UNIT_TIMA6,TimeraFlagCaptureOrCompareCh8);
        if(Cpt_count == 0)
        {
           Cpt_data = TIMERA_GetCaptureValue(UNIT_TIMA6,CH8_TIMA6);
        }
        else
        {
            Cpt_data_last = TIMERA_GetCaptureValue(UNIT_TIMA6,CH8_TIMA6);
        }       
        Cpt_count++;
        if(Cpt_count>1)
        {
            Cpt_time = Cpt_data_last - Cpt_data;
            if(Cpt_time < 0)
            {
                Cpt_time += Period;
            }
            Cpt_count = 0;
            singnal_Freq = 84000000/(512*Cpt_time);
        }
    }
}
void TIMA6_OVF_IrqCallback(void)
{
    TIMERA_ClearFlag(UNIT_TIMA6,TimeraFlagOverflow);
}
void Timera6_CPT_Config(void)
{
    stc_timera_base_init_t stcTimeraInit;
    stc_timera_capture_init_t stcTimeraCaptureInit;
    stc_irq_regi_conf_t stcIrqRegiConf;
    stc_port_init_t stcPortInit;

    /* configuration structure initialization */
    MEM_ZERO_STRUCT(stcTimeraInit);
    MEM_ZERO_STRUCT(stcIrqRegiConf);
    MEM_ZERO_STRUCT(stcTimeraCaptureInit);
    MEM_ZERO_STRUCT(stcPortInit);
    
    
    stcPortInit.enPinMode = Pin_Mode_In;
    PORT_Init(PORT_CPT,PIN_CPT, &stcPortInit);
    /* Configuration peripheral clock */
    PWC_Fcg2PeriphClockCmd(CLK_TIMA6, Enable);
//    PWC_Fcg0PeriphClockCmd(PWC_FCG0_PERIPH_PTDIS, Enable);

    /* Configuration TIMERA capture pin */
    PORT_SetFunc(PORT_CPT, PIN_CPT, Func_CPT, Disable);

    /* Configuration timera unit 1 base structure */
    stcTimeraInit.enClkDiv = TimeraPclkDiv512;
    stcTimeraInit.enCntMode = TimeraCountModeSawtoothWave;
    stcTimeraInit.enCntDir = TimeraCountDirUp;
    stcTimeraInit.enSyncStartupEn = Disable;
    stcTimeraInit.u16PeriodVal = Period;        //100ms
    TIMERA_BaseInit(UNIT_TIMA6, &stcTimeraInit);

    /* Configuration timera unit 1 capture structure */
    stcTimeraCaptureInit.enCapturePwmRisingEn = Enable;
    stcTimeraCaptureInit.enCapturePwmFallingEn = Disable;
    stcTimeraCaptureInit.enCaptureSpecifyEventEn = Disable;
    stcTimeraCaptureInit.enPwmClkDiv = TimeraFilterPclkDiv1;
    stcTimeraCaptureInit.enPwmFilterEn = Enable;
    stcTimeraCaptureInit.enCaptureTrigRisingEn = Disable;
    stcTimeraCaptureInit.enCaptureTrigFallingEn = Disable;
    stcTimeraCaptureInit.enTrigClkDiv = TimeraFilterPclkDiv1;
    stcTimeraCaptureInit.enTrigFilterEn = Disable;
    /* Enable channel 1 capture and interrupt */
    TIMERA_CaptureInit(UNIT_TIMA6, CH8_TIMA6, &stcTimeraCaptureInit);
    TIMERA_IrqCmd(UNIT_TIMA6, INT_CH8_TIMA6, Enable);
    TIMERA_IrqCmd(UNIT_TIMA6, INT_OVF_TIMA6, Enable);

//    /* Enable channel 2 capture and interrupt */
//    TIMERA_CaptureInit(TIMERA_UNIT1, TIMERA_UNIT1_CH2, &stcTimeraCaptureInit);
//    TIMERA_IrqCmd(TIMERA_UNIT1, TIMERA_UNIT1_CH2_INT, Enable);

    /* Configure interrupt of timera unit 1 */
    stcIrqRegiConf.enIntSrc = INT_SOURCE_TIMA6;
    stcIrqRegiConf.enIRQn = Int006_IRQn;
    stcIrqRegiConf.pfnCallback = TIMA6_IrqCallback;
    enIrqRegistration(&stcIrqRegiConf);
    NVIC_ClearPendingIRQ(stcIrqRegiConf.enIRQn);
    NVIC_SetPriority(stcIrqRegiConf.enIRQn, DDL_IRQ_PRIORITY_15);
    NVIC_EnableIRQ(stcIrqRegiConf.enIRQn);
    
    stcIrqRegiConf.enIntSrc = INT_TMRA6_OVF;
    stcIrqRegiConf.enIRQn = Int005_IRQn;
    stcIrqRegiConf.pfnCallback = TIMA6_OVF_IrqCallback;
    enIrqRegistration(&stcIrqRegiConf);
    NVIC_ClearPendingIRQ(stcIrqRegiConf.enIRQn);
    NVIC_SetPriority(stcIrqRegiConf.enIRQn, DDL_IRQ_PRIORITY_15);
    NVIC_EnableIRQ(stcIrqRegiConf.enIRQn);

    /* Set external Int Ch.4 trigger timera compare */
//    stcPortInit.enExInt = Enable;
//    PORT_Init(KEY1_PORT, KEY1_PIN, &stcPortInit);
//    TIMERA_SetCaptureTriggerSrc(KEY1_TRIGGER_EVENT);

    /* Timera unit 1 startup */
    TIMERA_Cmd(UNIT_TIMA6, Enable);
}

void TimerA5_OVF_Callback(void)
{
    ;
}
void TimerA5_config(void)
{
   stc_timera_base_init_t stcTimeraInit;
    stc_timera_compare_init_t stcTimerCompareInit;
    stc_irq_regi_conf_t stcIrqRegiConf;
    stc_timera_hw_startup_cofig_t stcTimeraHwConfig;
    stc_port_init_t stcPortInit;
    stc_timera_orthogonal_coding_init_t stcTimerORthoCFG;

    /* configuration structure initialization */
    MEM_ZERO_STRUCT(stcTimeraInit);
    MEM_ZERO_STRUCT(stcIrqRegiConf);
    MEM_ZERO_STRUCT(stcTimerCompareInit);
    MEM_ZERO_STRUCT(stcTimeraHwConfig);
    MEM_ZERO_STRUCT(stcPortInit);
    MEM_ZERO_STRUCT(stcTimerORthoCFG);
    /* Configuration peripheral clock */
    PWC_Fcg2PeriphClockCmd(CLK_TIMA5, Enable);

    /* Configuration timera unit 1 base structure */
    stcTimeraInit.enCntMode = TimeraCountModeSawtoothWave;
    stcTimeraInit.enCntDir = TimeraCountDirUp;
    stcTimeraInit.enSyncStartupEn = Enable;
    stcTimeraInit.u16PeriodVal = 10000;        //freq: 100Hz
//    
    TIMERA_BaseInit(UNIT_TIMA5, &stcTimeraInit);
    
    /* Enable period count interrupt */
    TIMERA_IrqCmd(UNIT_TIMA5, TimeraIrqOverflow, Enable);
    /* Interrupt of timera unit 1 */
    stcIrqRegiConf.enIntSrc = TIMERA5_UNITH_OVERFLOW_INT;
    stcIrqRegiConf.enIRQn = Int007_IRQn;
    stcIrqRegiConf.pfnCallback = TimerA5_OVF_Callback;
    enIrqRegistration(&stcIrqRegiConf);
    NVIC_ClearPendingIRQ(stcIrqRegiConf.enIRQn);
    NVIC_SetPriority(stcIrqRegiConf.enIRQn, DDL_IRQ_PRIORITY_15);
    NVIC_EnableIRQ(stcIrqRegiConf.enIRQn);

    /* Configure timera unit 1 SourceCLK */

    stcTimerORthoCFG.enIncAnotherUnitOverflowEn = Enable;
    TIMERA_OrthogonalCodingInit(UNIT_TIMA5,&stcTimerORthoCFG);

    TIMERA_Cmd(UNIT_TIMA5,Enable);
}
