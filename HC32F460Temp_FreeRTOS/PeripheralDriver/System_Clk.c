#include "hc32_ddl.h"
#include "system_clk_cfg.h"
/*
system clock initial, all parameter refer to system_clk_cfg.h setting.
*/
void system_clk_init(void)
{
    stc_clk_xtal_cfg_t XTAL_CFG;
    stc_clk_xtal32_cfg_t XTAL32_CFG;
    stc_clk_mpll_cfg_t MPLL_CFG;
    stc_clk_upll_cfg_t UPLL_CFG;
    stc_clk_sysclk_cfg_t SYS_CLK_CFG;
    stc_sram_config_t SRAM_CFG;
    MEM_ZERO_STRUCT(SRAM_CFG);
    MEM_ZERO_STRUCT(XTAL_CFG);
    MEM_ZERO_STRUCT(XTAL32_CFG);
    MEM_ZERO_STRUCT(MPLL_CFG);
    MEM_ZERO_STRUCT(UPLL_CFG);
    MEM_ZERO_STRUCT(SYS_CLK_CFG);
    
    
    CLK_HrcCmd((en_functional_state_t)HRC_ENABLE);
//    CLK_MrcCmd((en_functional_state_t)MRC_ENABLE);
    CLK_LrcCmd((en_functional_state_t)LRC_ENABLE);
#if (XTAL_ENABLE == ENABLE)
    XTAL_CFG.enMode = XTAL_MODE;
    XTAL_CFG.enDrv = XTAL_DRV;
    XTAL_CFG.enFastStartup = XTAL_SUPDRV_ENABLE;
    CLK_XtalStbConfig(XTAL_STB);
    CLK_XtalConfig(&XTAL_CFG);    
#endif
    CLK_XtalCmd((en_functional_state_t)XTAL_ENABLE);
#if (XTAL32_ENABLE == ENABLE)
    CLK_Xtal32Cmd(Disable);
    XTAL32_CFG.enFilterMode = XTAL32_NF_MODE;
    XTAL32_CFG.enFastStartup = XTAL32_SUPDRV_ENABLE;
    XTAL32_CFG.enDrv = XTAL32_DRV;
    CLK_Xtal32Config(&XTAL32_CFG);
#endif
    CLK_Xtal32Cmd((en_functional_state_t)XTAL32_ENABLE);
    
    CLK_SetPllSource(PLL_CLK_SOURCE);//MPLL&UPLL Clock source
#if (MPLL_CLK_ENABLE == ENABLE)
    MPLL_CFG.pllmDiv = MPLL_CLK_M_DIV;
    MPLL_CFG.plln = MPLL_CLK_NUM;
    MPLL_CFG.PllpDiv = MPLL_CLK_P_DIV;
    MPLL_CFG.PllqDiv = MPLL_CLK_Q_DIV;
    MPLL_CFG.PllrDiv = MPLL_CLK_R_DIV;    
    CLK_MpllConfig(&MPLL_CFG);
#endif
    CLK_MpllCmd((en_functional_state_t)MPLL_CLK_ENABLE);
#if MPLL_CLK_ENABLE
    while(Set != CLK_GetFlagStatus(ClkFlagMPLLRdy));
#endif    
#if (UPLL_CLK_ENABLE == ENABLE)
    UPLL_CFG.pllmDiv = UPLL_CLK_M_DIV;
    UPLL_CFG.plln = UPLL_CLK_NUM;
    UPLL_CFG.PllpDiv = UPLL_CLK_P_DIV;
    UPLL_CFG.PllqDiv = UPLL_CLK_Q_DIV;
    UPLL_CFG.PllrDiv = UPLL_CLK_R_DIV;
    CLK_UpllConfig(&UPLL_CFG);
#endif
    CLK_UpllCmd((en_functional_state_t)UPLL_CLK_ENABLE);
#if UPLL_CLK_ENABLE
    while(Set != CLK_GetFlagStatus(ClkFlagUPLLRdy));
#endif    
    CLK_SetUsbClkSource(USB_CLK_SOURCE);
    
    CLK_SetPeriClkSource((en_clk_peri_source_t)PERI_CLK_SOURCE);
    SYS_CLK_CFG.enExclkDiv = EXCKS_DIV;
    SYS_CLK_CFG.enHclkDiv = HCLK_DIV;
    SYS_CLK_CFG.enPclk0Div = PCLK0S_DIV;
    SYS_CLK_CFG.enPclk1Div = PCLK1S_DIV;
    SYS_CLK_CFG.enPclk2Div = PCLK2S_DIV;
    SYS_CLK_CFG.enPclk3Div = PCLK3S_DIV;
    SYS_CLK_CFG.enPclk4Div = PCLK4S_DIV;
//-------------Flash wait setting--------------//    
/* flash read wait cycle setting */
    EFM_Unlock();
    EFM_SetLatency(EFM_LATENCY_4);
    EFM_InstructionCacheCmd(Disable);
    EFM_Lock();
    SRAM_WT_Enable();
    SRAM_CK_Enable();
    M4_SRAMC->WTCR_f.SRAMBKRWT = SramCycle2;
    M4_SRAMC->WTCR_f.SRAMBKWWT = SramCycle2;
    M4_SRAMC->WTCR_f.SRAMECCRWT = SramCycle2;
    M4_SRAMC->WTCR_f.SRAMECCWWT = SramCycle2;
    M4_SRAMC->WTCR_f.SRAMSYSRWT = SramCycle2;
    M4_SRAMC->WTCR_f.SRAMSYSWWT = SramCycle2;
    SRAM_WT_Disable();
    SRAM_CK_Disable();
//-------------Switch system clock-----------------------//
    CLK_SetSysClkSource(SYSTEMCLKSOURCE);
    CLK_SysClkConfig(&SYS_CLK_CFG);  
}
/*End of file*/
