#include "System_PowerDown.h"
/**
 *******************************************************************************
 ** \brief System_Enter_StopMode
 **
 ** \param  None
 **
 ** \retval None
 **
 ******************************************************************************/
void System_Enter_StopMode(void)
{
    stc_pwc_stop_mode_cfg_t stcPwcStopCfg;

    MEM_ZERO_STRUCT(stcPwcStopCfg);

    /* Config stop mode. */
    stcPwcStopCfg.enStpDrvAbi = StopHighspeed;
    stcPwcStopCfg.enStopClk = ClkFix;
    stcPwcStopCfg.enStopFlash = Wait;
    PWC_StopModeCfg(&stcPwcStopCfg);
    PWC_EnterStopMd();
}
/**
 *******************************************************************************
 ** \brief GetWakeupFlag
 **
 ** \param  None
 **
 ** \retval None
 **
 ******************************************************************************/
uint16_t GetWakeupFlag(void)
{
    uint16_t temp;
    temp = M4_SYSREG->PWR_PDWKF0;
    temp += M4_SYSREG->PWR_PDWKF1<<8;
    PWC_ClearWakeup1Flag(PWC_RTCPRD_WKUPFALG | PWC_RXD0_WKUPFLAG | PWC_RTCAL_WKUPFLAG | PWC_XTAL32ER_WKUPFALG | PWC_WKTM_WKUPFLAG);
    PWC_ClearWakeup0Flag(PWC_PTWK0_WKUPFLAG | PWC_PTWK1_WKUPFLAG | PWC_PTWK2_WKUPFLAG | PWC_PTWK3_WKUPFLAG | PWC_PVD1_WKUPFLAG | PWC_PVD2_WKUPFLAG | PWC_NMI_WKUPFLAG);
    return temp;
}
/**
 *******************************************************************************
 ** \brief WakeupSourceEnable
 **
 ** \param  None
 **
 ** \retval None
 **
 ******************************************************************************/
static void WakeupSourceEnable(void)
{
    PWC_PdWakeup0Cmd(KEY_WK0_EN | KEY_WK1_EN | KEY_WK02_EN | KEY_WK13_EN,Enable);
    PWC_PdWakeup1Cmd(KEY_WK2_EN | KEY_WK3_EN,Enable);
}
/**
 *******************************************************************************
 ** \brief System_Enter_PowerDown
 **
 ** \param  None
 **
 ** \retval None
 **
 ******************************************************************************/
void System_Enter_PowerDown(void)
{
    stc_pwc_pwr_mode_cfg_t  stcPwcPwrMdCfg;
    /* Config power down mode. */
    stcPwcPwrMdCfg.enPwrDownMd = PowerDownMd2;
    stcPwcPwrMdCfg.enRLdo = Disable;
    stcPwcPwrMdCfg.enIoRetain = IoPwrDownRetain;
    stcPwcPwrMdCfg.enRetSram = Enable;
    stcPwcPwrMdCfg.enVHrc = Disable;
    stcPwcPwrMdCfg.enVPll = Disable;
    stcPwcPwrMdCfg.enDynVol =  Voltage09;
    stcPwcPwrMdCfg.enDrvAbility = Ulowspeed;
    stcPwcPwrMdCfg.enPwrDWkupTm = Vcap0047;
    PWC_PowerModeCfg(&stcPwcPwrMdCfg);
    CLK_LrcCmd(Disable); //�ر�LRC
    PWC_EnterPowerDownMd();
}
/**
 *******************************************************************************
 ** \brief KeyWakeup0_Callback
 **
 ** \param  None
 **
 ** \retval None
 **
 ******************************************************************************/
static void KeyWakeup0_Callback(void)
{
    if (Set == EXINT_IrqFlgGet(KEY_WK0_EICH))
    {
        /* clear int request flag */
        EXINT_IrqFlgClr(KEY_WK0_EICH);
    }
}
/**
 *******************************************************************************
 ** \brief KeyWakeup1_Callback
 **
 ** \param  None
 **
 ** \retval None
 **
 ******************************************************************************/
static void KeyWakeup1_Callback(void)
{
    if (Set == EXINT_IrqFlgGet(KEY_WK1_EICH))
    {
        /* clear int request flag */
        EXINT_IrqFlgClr(KEY_WK1_EICH);
    }
}
/**
 *******************************************************************************
 ** \brief KeyWakeup2_Callback
 **
 ** \param  None
 **
 ** \retval None
 **
 ******************************************************************************/
static void KeyWakeup2_Callback(void)
{
    if (Set == EXINT_IrqFlgGet(KEY_WK2_EICH))
    {
        /* clear int request flag */
        EXINT_IrqFlgClr(KEY_WK2_EICH);
    }
}
/**
 *******************************************************************************
 ** \brief KeyWakeup3_Callback
 **
 ** \param  None
 **
 ** \retval None
 **
 ******************************************************************************/
static void KeyWakeup3_Callback(void)
{
    if (Set == EXINT_IrqFlgGet(KEY_WK3_EICH))
    {
        /* clear int request flag */
        EXINT_IrqFlgClr(KEY_WK3_EICH);
    }
}
/**
 *******************************************************************************
 ** \brief Key_Wakeup_Inits
 **
 ** \param  None
 **
 ** \retval None
 **
 ******************************************************************************/
void Key_Wakeup_Init(void)
{
    stc_exint_config_t stcExtiConfig;
    stc_irq_regi_conf_t stcIrqRegiConf;
    stc_port_init_t stcPortInit;

    /* configuration structure initialization */
    MEM_ZERO_STRUCT(stcExtiConfig);
    MEM_ZERO_STRUCT(stcIrqRegiConf);
    MEM_ZERO_STRUCT(stcPortInit);
    PWC_PdWakeup0Cmd(KEY_WK0_EN | KEY_WK1_EN | KEY_WK02_EN | KEY_WK13_EN,Disable);
    PWC_PdWakeup1Cmd(KEY_WK2_EN | KEY_WK3_EN,Disable);
    EXINT_IrqFlgClr(KEY_WK0_EICH);
    EXINT_IrqFlgClr(KEY_WK1_EICH);
    EXINT_IrqFlgClr(KEY_WK2_EICH);
    EXINT_IrqFlgClr(KEY_WK3_EICH);
    /**************************************************************************/
    /* External Int                                           */
    /**************************************************************************/
    
    /* Filter setting */
    stcExtiConfig.enFilterEn = Enable;
    stcExtiConfig.enFltClk = Pclk3Div64;
    /* falling edge for keyscan function */
    stcExtiConfig.enExtiLvl = ExIntFallingEdge;
    stcExtiConfig.enExitCh = KEY_WK0_EICH;
    EXINT_Init(&stcExtiConfig);
    stcExtiConfig.enExitCh = KEY_WK1_EICH;
    EXINT_Init(&stcExtiConfig);
    stcExtiConfig.enExitCh = KEY_WK2_EICH;
    EXINT_Init(&stcExtiConfig);
    stcExtiConfig.enExitCh = KEY_WK3_EICH;
    EXINT_Init(&stcExtiConfig); 
    /* Set PD12 as External Int Ch.12 input */
    MEM_ZERO_STRUCT(stcPortInit);
    stcPortInit.enPinMode = Pin_Mode_In;
    stcPortInit.enExInt = Enable;
    stcPortInit.enPullUp = Enable;
    PORT_Init(KEY_WK0_PORT, KEY_WK0_PIN, &stcPortInit);
    PORT_Init(KEY_WK1_PORT, KEY_WK1_PIN, &stcPortInit);
    PORT_Init(KEY_WK2_PORT, KEY_WK2_PIN, &stcPortInit);
    PORT_Init(KEY_WK3_PORT, KEY_WK3_PIN, &stcPortInit);
    PORT_Init(KEY_WK02_PORT,KEY_WK02_PIN, &stcPortInit);
    PORT_Init(PortB, Pin07, &stcPortInit);

    /* Select External Int Ch.01 */
    stcIrqRegiConf.enIntSrc = KEY_WK0_INT;

    /* Register External Int to Vect.No.000 */
    stcIrqRegiConf.enIRQn = KEY_WK0_IRQn;

    /* Callback function */
    stcIrqRegiConf.pfnCallback = KeyWakeup0_Callback;

    /* Registration IRQ */
    enIrqRegistration(&stcIrqRegiConf);

    /* Clear pending */
    NVIC_ClearPendingIRQ(stcIrqRegiConf.enIRQn);

    /* Set priority */
    NVIC_SetPriority(stcIrqRegiConf.enIRQn, DDL_IRQ_PRIORITY_15);

    /* Enable NVIC */
    NVIC_EnableIRQ(stcIrqRegiConf.enIRQn);
    
    ////////////////////////////////////////////////////////////
     /* Select External Int Ch.05 */
    stcIrqRegiConf.enIntSrc = KEY_WK1_INT;

    /* Register External Int to Vect.No.000 */
    stcIrqRegiConf.enIRQn = KEY_WK1_IRQn;

    /* Callback function */
    stcIrqRegiConf.pfnCallback = KeyWakeup1_Callback;

    /* Registration IRQ */
    enIrqRegistration(&stcIrqRegiConf);

    /* Clear pending */
    NVIC_ClearPendingIRQ(stcIrqRegiConf.enIRQn);

    /* Set priority */
    NVIC_SetPriority(stcIrqRegiConf.enIRQn, DDL_IRQ_PRIORITY_15);

    /* Enable NVIC */
    NVIC_EnableIRQ(stcIrqRegiConf.enIRQn);
     ////////////////////////////////////////////////////////////
     /* Select External Int Ch.08 */
    stcIrqRegiConf.enIntSrc = KEY_WK2_INT;

    /* Register External Int to Vect.No.000 */
    stcIrqRegiConf.enIRQn = KEY_WK2_IRQn;

    /* Callback function */
    stcIrqRegiConf.pfnCallback = KeyWakeup2_Callback;

    /* Registration IRQ */
    enIrqRegistration(&stcIrqRegiConf);

    /* Clear pending */
    NVIC_ClearPendingIRQ(stcIrqRegiConf.enIRQn);

    /* Set priority */
    NVIC_SetPriority(stcIrqRegiConf.enIRQn, DDL_IRQ_PRIORITY_15);

    /* Enable NVIC */
    NVIC_EnableIRQ(stcIrqRegiConf.enIRQn);
    
     ////////////////////////////////////////////////////////////
     /* Select External Int Ch.15 */
    stcIrqRegiConf.enIntSrc = KEY_WK3_INT;

    /* Register External Int to Vect.No.000 */
    stcIrqRegiConf.enIRQn = KEY_WK3_IRQn;

    /* Callback function */
    stcIrqRegiConf.pfnCallback = KeyWakeup3_Callback;

    /* Registration IRQ */
    enIrqRegistration(&stcIrqRegiConf);

    /* Clear pending */
    NVIC_ClearPendingIRQ(stcIrqRegiConf.enIRQn);

    /* Set priority */
    NVIC_SetPriority(stcIrqRegiConf.enIRQn, DDL_IRQ_PRIORITY_15);

    /* Enable NVIC */
    NVIC_EnableIRQ(stcIrqRegiConf.enIRQn);
    WakeupSourceEnable();
    GetWakeupFlag();
    M4_INTC->EICFR = 0xFF;//��������ⲿ�жϱ�־
    
}
void LPM_TEST(void)
{
//    printf("System Enter sleep!\r\n");  
//    PWC_EnterSleepMd(); 
//    printf("System_Wakeup\r\n");
//    Ddl_Delay1ms(5000);
//    
//    printf("System Enter Stop!\r\n"); 
//    enIntWakeupEnable(Extint4WU);
//    System_Enter_StopMode(); 
//    printf("System_Wakeup\r\n");
    Key_Wakeup_Init();
    Ddl_Delay1ms(5000);
    
    printf("System Enter Powerdown!\r\n"); 
    System_Enter_PowerDown(); 
}

