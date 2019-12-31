#include "hc32_ddl.h"
#include "User_Gpio.h"
#include "System_InterruptCFG_Def.h"
//#include "cmsis_os.h"
/*������������Timer01ͨ��A�����жϣ�����ͨ��B�Ϳ�����*/
void Timer01_CHB_CallBack(void)//
{
//    LED0_Toggle();
//    printf("TIMER01_CHA_Callback\r\n");
//    osSystickHandler();
//     Test_GPIO();
}
void User_Timer0_Init(void)
{
	stc_tim0_base_init_t stcTimerCfg;
    stc_irq_regi_conf_t stcIrqRegiConf;
	
	MEM_ZERO_STRUCT(stcTimerCfg);
	MEM_ZERO_STRUCT(stcIrqRegiConf);
	
	/*Write-off protection*/
//    PWC_Fcg2PeriphClockCmd(PWC_FCG2_PERIPH_TIM01,Enable);
	M4_MSTP->FCG2_f.TIMER0_1 = Reset;//ʱ��ʹ��
	
	stcTimerCfg.Tim0_CounterMode = Tim0_Sync;//ͬ������
	stcTimerCfg.Tim0_SyncClockSource = Tim0_Pclk1;//ʱ��ԴPCLK1
	stcTimerCfg.Tim0_ClockDivision = Tim0_ClkDiv2;//512��Ƶ
	stcTimerCfg.Tim0_CmpValue = 42000-1;
	TIMER0_BaseInit(M4_TMR01, Tim0_ChannelB, &stcTimerCfg);//TIMER0_BaseInit(M4_TMR01, Tim0_ChannelA, &stcTimerCfg);
//    TIMER0_BaseInit(M4_TMR01, Tim0_ChannelA, &stcTimerCfg);
    TIMER0_IntCmd(M4_TMR01,Tim0_ChannelB,Enable);
//    TIMER0_IntCmd(M4_TMR01,Tim0_ChannelA,Enable);
	
	stcIrqRegiConf.enIntSrc = INT_TMR01_GCMB;//INT_TMR01_GCMA
	stcIrqRegiConf.pfnCallback = Timer01_CHB_CallBack;
	stcIrqRegiConf.enIRQn = TIMER01_CHB_IRQn;
	enIrqRegistration(&stcIrqRegiConf);
	
	NVIC_EnableIRQ(stcIrqRegiConf.enIRQn);//ʹ���ж�
	
	TIMER0_Cmd(M4_TMR01, Tim0_ChannelB, Enable);//����TIMER
    //TIMER0_Cmd(M4_TMR01, Tim0_ChannelA, Enable);
}
