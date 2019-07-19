/******************************************************************************
 * Copyright (C) 2016, Huada Semiconductor Co.,Ltd. All rights reserved.
 *
 * This software is owned and published by:
 * Huada Semiconductor Co.,Ltd ("HDSC").
 *
 * BY DOWNLOADING, INSTALLING OR USING THIS SOFTWARE, YOU AGREE TO BE BOUND
 * BY ALL THE TERMS AND CONDITIONS OF THIS AGREEMENT.
 *
 * This software contains source code for use with HDSC
 * components. This software is licensed by HDSC to be adapted only
 * for use in systems utilizing HDSC components. HDSC shall not be
 * responsible for misuse or illegal use of this software for devices not
 * supported herein. HDSC is providing this software "AS IS" and will
 * not be responsible for issues arising from incorrect user implementation
 * of the software.
 *
 * Disclaimer:
 * HDSC MAKES NO WARRANTY, EXPRESS OR IMPLIED, ARISING BY LAW OR OTHERWISE,
 * REGARDING THE SOFTWARE (INCLUDING ANY ACOOMPANYING WRITTEN MATERIALS),
 * ITS PERFORMANCE OR SUITABILITY FOR YOUR INTENDED USE, INCLUDING,
 * WITHOUT LIMITATION, THE IMPLIED WARRANTY OF MERCHANTABILITY, THE IMPLIED
 * WARRANTY OF FITNESS FOR A PARTICULAR PURPOSE OR USE, AND THE IMPLIED
 * WARRANTY OF NONINFRINGEMENT.
 * HDSC SHALL HAVE NO LIABILITY (WHETHER IN CONTRACT, WARRANTY, TORT,
 * NEGLIGENCE OR OTHERWISE) FOR ANY DAMAGES WHATSOEVER (INCLUDING, WITHOUT
 * LIMITATION, DAMAGES FOR LOSS OF BUSINESS PROFITS, BUSINESS INTERRUPTION,
 * LOSS OF BUSINESS INFORMATION, OR OTHER PECUNIARY LOSS) ARISING FROM USE OR
 * INABILITY TO USE THE SOFTWARE, INCLUDING, WITHOUT LIMITATION, ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL OR CONSEQUENTIAL DAMAGES OR LOSS OF DATA,
 * SAVINGS OR PROFITS,
 * EVEN IF Disclaimer HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * YOU ASSUME ALL RESPONSIBILITIES FOR SELECTION OF THE SOFTWARE TO ACHIEVE YOUR
 * INTENDED RESULTS, AND FOR THE INSTALLATION OF, USE OF, AND RESULTS OBTAINED
 * FROM, THE SOFTWARE.
 *
 * This software may be replicated in part or whole for the licensed use,
 * with the restriction that this Disclaimer and Copyright notice must be
 * included with each copy of this software, whether used in part or whole,
 * at all times.
 */
/******************************************************************************/
/** \file hc32f46x_i2c.c
 **
 ** A detailed description is available at
 ** @link I2cGroup Inter-Integrated Circuit(I2C) description @endlink
 **
 **   - 2018-10-16  1.0  Wangmin  First version for Device Driver Library of I2C.
 **
 ******************************************************************************/

/*******************************************************************************
 * Include files
 ******************************************************************************/
#include "hc32f46x_i2c.h"
#include "hc32f46x_utility.h"
#include "hc32f46x_clk.h"

#if (DDL_I2C_ENABLE == DDL_ON)

/**
 *******************************************************************************
 ** \addtogroup I2cGroup
 ******************************************************************************/
//@{

/*******************************************************************************
 * Local type definitions ('typedef')
 ******************************************************************************/

/*******************************************************************************
 * Local pre-processor symbols/macros ('#define')
 ******************************************************************************/
#define I2C_BAUDRATE_MAX                400000ul

/*! Parameter valid check for peripheral Instances. */
#define IS_VALID_POINTER(x)           (NULL != (x))

/*! Parameter validity check for unit. */
#define IS_VALID_UNIT(x)                                                       \
(   ((x) == M4_I2C1)                               ||                          \
    ((x) == M4_I2C2)                               ||                          \
    ((x) == M4_I2C3))

/*! Parameter check for I2C baudrate value !*/
#define IS_VALID_SPEED(speed)           (speed <= (I2C_BAUDRATE_MAX))

/*! Parameter check for Function state !*/
#define IS_VALID_FUNCTION_STATE(x)                                             \
(   ((x) == Disable)                               ||                          \
    ((x) == Enable))

/*! Parameter check for Digital filter config !*/
#define IS_VALID_DIGITAL_FILTER(x)                                             \
(   ((x) == Filter1BaseCycle)                      ||                          \
    ((x) == Filter2BaseCycle)                      ||                          \
    ((x) == Filter3BaseCycle)                      ||                          \
    ((x) == Filter4BaseCycle))

/*! Parameter check for address mode !*/
#define IS_VALID_ADRMODE(x)                                                    \
(   ((x) == Adr7bit)                               ||                          \
    ((x) == Adr10bit))

/*! Parameter check for Time out control switch !*/
#define IS_VALID_TIMOUT_SWITCH(x)                                              \
(   ((x) == TimeoutFunOff)                         ||                          \
    ((x) == LowTimerOutOn)                         ||                          \
    ((x) == HighTimeOutOn)                         ||                          \
    ((x) == BothTimeOutOn))

/*! Parameter check for I2C 7 bit address range !*/
#define IS_VALID_7BIT_ADR(x)           (x <= 0x7F)

/*! Parameter check for readable I2C status bit !*/
#define IS_VALID_RD_STATUS_BIT(x)                                              \
(   ((x) == I2C_SR_STARTF)                         ||                          \
    ((x) == I2C_SR_SLADDR0F)                       ||                          \
    ((x) == I2C_SR_SLADDR1F)                       ||                          \
    ((x) == I2C_SR_TENDF)                          ||                          \
    ((x) == I2C_SR_STOPF)                          ||                          \
    ((x) == I2C_SR_RFULLF)                         ||                          \
    ((x) == I2C_SR_TEMPTYF)                        ||                          \
    ((x) == I2C_SR_ARLOF)                          ||                          \
    ((x) == I2C_SR_NACKDETECTF)                    ||                          \
    ((x) == I2C_SR_NACKSENDF)                      ||                          \
    ((x) == I2C_SR_TMOUTF)                         ||                          \
    ((x) == I2C_SR_MSL)                            ||                          \
    ((x) == I2C_SR_BUSY)                           ||                          \
    ((x) == I2C_SR_TRA)                            ||                          \
    ((x) == I2C_SR_GENCALLF)                       ||                          \
    ((x) == I2C_SR_SMBDEFAULTF)                    ||                          \
    ((x) == I2C_SR_SMBHOSTF)                       ||                          \
    ((x) == I2C_SR_SMBALRTF))

/*! Parameter check for writeable I2C status bit !*/
#define IS_VALID_WR_STATUS_BIT(x)                                              \
(   ((x) == I2C_SR_MSL)                            ||                          \
    ((x) == I2C_SR_TRA))

/*! Parameter check for I2C mode !*/
#define IS_VALID_I2C_MODE(x)                                                   \
(   ((x) == I2cMaster)                             ||                          \
    ((x) == I2cSlave))

/*******************************************************************************
 * Global variable definitions (declared in header file with 'extern')
 ******************************************************************************/

/*******************************************************************************
 * Local function prototypes ('static')
 ******************************************************************************/

/*******************************************************************************
 * Local variable definitions ('static')
 ******************************************************************************/
static uint8_t u8FreqDiv[8] = {1,2,4,8,16,32,64,128};

/*******************************************************************************
 * Function implementation - global ('extern') and local ('static')
 ******************************************************************************/

/**
 *******************************************************************************
 ** \brief  static function for baudrate calculating
 **
 ** \param  [in] fDiv      Divisor value input in float type
 **
 ** \retval uint8_t        Divisor value output
 **
 ** \note   None
 **
 ******************************************************************************/
static uint8_t GetFreqReg(float fDiv)
{
    uint8_t u8Reg = 7;
    for(uint8_t i=0; i<8; i++)
    {
        if(fDiv <= u8FreqDiv[i])
        {
            u8Reg = i;
            break;
        }
    }
    return u8Reg;
}

/**
 *******************************************************************************
 ** \brief  Set the baudrate for I2C peripheral.
 **
 ** \param [in] pstcI2Cx           Pointer to the I2C peripheral register, can
 **                                be M4_I2C1,M4_I2C2 or M4_I2C3.
 ** \param [in] u32Baudrate        The value of baudrate.
 **
 ** \retval None
 **
 ******************************************************************************/
void I2C_BaudrateConfig(M4_I2C_TypeDef* pstcI2Cx, uint32_t u32Baudrate)
{
    stc_clk_freq_t stcClkFreq;
    uint32_t u32Pclk3;
    float fDivIndex = 0;
    uint8_t u8DivIndex;
    uint32_t width = 0;

    /* Check parameters */
    DDL_ASSERT(IS_VALID_UNIT(pstcI2Cx));
    DDL_ASSERT(IS_VALID_SPEED(u32Baudrate));

    /* Get Pclk3*/
    CLK_GetClockFreq(&stcClkFreq);
    u32Pclk3 = stcClkFreq.pclk3Freq;

    /* Calculate the pclk3 div */
    fDivIndex = u32Pclk3/(u32Baudrate*32*2);
    u8DivIndex = GetFreqReg(fDivIndex);
    width =  u32Pclk3 / (u8FreqDiv[u8DivIndex] * u32Baudrate);

    /* Write register */
    pstcI2Cx->CCR_f.FREQ = u8DivIndex;
    pstcI2Cx->CCR_f.SLOWW = width / 2;
    pstcI2Cx->CCR_f.SHIGHW = width - width / 2;
}

/**
 *******************************************************************************
 ** \brief De-initialize I2C unit
 **
 ** \param [in] pstcI2Cx            Pointer to the I2C peripheral register, can
 ** \                               be M4_I2C1,M4_I2C2 or M4_I2C3.
 **
 ** \retval None
 **
 ******************************************************************************/
void I2C_DeInit(M4_I2C_TypeDef* pstcI2Cx)
{
    DDL_ASSERT(IS_VALID_UNIT(pstcI2Cx));

    /* Reset peripheral register and internal status*/
    pstcI2Cx->CR1_f.PE = 0;
    pstcI2Cx->CR1_f.SWRST = 1;
}

/**
 *******************************************************************************
 ** \brief Initialize I2C peripheral according to the structure
 **
 ** \param [in] pstcI2Cx            Pointer to the I2C peripheral register, can
 ** \                               be M4_I2C1,M4_I2C2 or M4_I2C3.
 ** \param [in] pstcI2C_InitStruct  Pointer to I2C configuration structure
 **
 ** \retval None
 **
 ******************************************************************************/
void I2C_Init(M4_I2C_TypeDef* pstcI2Cx, const stc_i2c_init_t* pstcI2C_InitStruct)
{
    DDL_ASSERT(IS_VALID_UNIT(pstcI2Cx));
    DDL_ASSERT(IS_VALID_POINTER(pstcI2C_InitStruct));
    DDL_ASSERT(IS_VALID_I2C_MODE(pstcI2C_InitStruct->enI2cMode));
    DDL_ASSERT(IS_VALID_SPEED(pstcI2C_InitStruct->u32Baudrate));

    /* Register and internal status reset */
    pstcI2Cx->CR1_f.PE = 0;
    pstcI2Cx->CR1_f.SWRST = 1;

    pstcI2Cx->CR1_f.PE = 1;

    I2C_BaudrateConfig(pstcI2Cx, pstcI2C_InitStruct->u32Baudrate);

    pstcI2Cx->CR1_f.ENGC = 0;
    pstcI2Cx->CR1_f.SWRST = 0;
    pstcI2Cx->CR1_f.PE = 0;
}

/**
 *******************************************************************************
 ** \brief I2C function command
 **
 ** \param [in] pstcI2Cx            Pointer to the I2C peripheral register, can
 ** \                               be M4_I2C1,M4_I2C2 or M4_I2C3.
 ** \param [in] enNewState          New state of the I2Cx function, can be
 **                                 Disable or Enable the function
 **
 ** \retval None
 **
 ******************************************************************************/
void I2C_Cmd(M4_I2C_TypeDef* pstcI2Cx, en_functional_state_t enNewState)
{
    DDL_ASSERT(IS_VALID_UNIT(pstcI2Cx));
    DDL_ASSERT(IS_VALID_FUNCTION_STATE(enNewState));

    pstcI2Cx->CR1_f.PE = enNewState;
}

/**
 *******************************************************************************
 ** \brief I2C SMBUS function configuration
 **
 ** \param [in] pstcI2Cx            Pointer to the I2C peripheral register, can
 ** \                               be M4_I2C1,M4_I2C2 or M4_I2C3.
 ** \param [in] pstcI2C_SmbusInitStruct
 **                                 Pointer to I2C SMBUS configuration structure
 **
 ** \retval None
 **
 ******************************************************************************/
void I2C_SmbusConfig(M4_I2C_TypeDef* pstcI2Cx, const stc_i2c_smbus_init_t* pstcI2C_SmbusInitStruct)
{
    DDL_ASSERT(IS_VALID_UNIT(pstcI2Cx));
    DDL_ASSERT(IS_VALID_POINTER(pstcI2C_SmbusInitStruct));
    DDL_ASSERT(IS_VALID_FUNCTION_STATE(pstcI2C_SmbusInitStruct->enHostAdrMatchFunc));
    DDL_ASSERT(IS_VALID_FUNCTION_STATE(pstcI2C_SmbusInitStruct->enDefaultAdrMatchFunc));
    DDL_ASSERT(IS_VALID_FUNCTION_STATE(pstcI2C_SmbusInitStruct->enAlarmAdrMatchFunc));

    pstcI2Cx->CR1_f.SMBHOSTEN = pstcI2C_SmbusInitStruct->enHostAdrMatchFunc;
    pstcI2Cx->CR1_f.SMBDEFAULTEN = pstcI2C_SmbusInitStruct->enDefaultAdrMatchFunc;
    pstcI2Cx->CR1_f.SMBALRTEN = pstcI2C_SmbusInitStruct->enAlarmAdrMatchFunc;
}

/**
 *******************************************************************************
 ** \brief I2C SMBUS function command
 **
 ** \param [in] pstcI2Cx            Pointer to the I2C peripheral register, can
 ** \                               be M4_I2C1,M4_I2C2 or M4_I2C3.
 ** \param [in] enNewState          New state of the I2Cx function, can be
 **                                 Disable or Enable the function
 **
 ** \retval None
 **
 ******************************************************************************/
void I2C_SmBusCmd(M4_I2C_TypeDef* pstcI2Cx, en_functional_state_t enNewState)
{
    DDL_ASSERT(IS_VALID_UNIT(pstcI2Cx));
    DDL_ASSERT(IS_VALID_FUNCTION_STATE(enNewState));

    pstcI2Cx->CR1_f.SMBUS = enNewState;
}

/**
 *******************************************************************************
 ** \brief I2C software reset function command
 **
 ** \param [in] pstcI2Cx            Pointer to the I2C peripheral register, can
 ** \                               be M4_I2C1,M4_I2C2 or M4_I2C3.
 ** \param [in] enNewState          New state of the I2Cx function, can be
 **                                 Disable or Enable the function
 **
 ** \retval None
 **
 ******************************************************************************/
void I2C_SoftwareResetCmd(M4_I2C_TypeDef* pstcI2Cx, en_functional_state_t enNewState)
{
    DDL_ASSERT(IS_VALID_UNIT(pstcI2Cx));
    DDL_ASSERT(IS_VALID_FUNCTION_STATE(enNewState));

    pstcI2Cx->CR1_f.SWRST = enNewState;
}

/**
 *******************************************************************************
 ** \brief I2C digital filter function configuration
 **
 ** \param [in] pstcI2Cx            Pointer to the I2C peripheral register, can
 ** \                               be M4_I2C1,M4_I2C2 or M4_I2C3.
 ** \param [in] enDigiFilterMode    Chose the digital filter mode, This parameter
 **                                 can be one of the following values:
 **                                 Filter1BaseCycle
 **                                 Filter2BaseCycle
 **                                 Filter3BaseCycle
 **                                 Filter4BaseCycle
 **
 ** \retval None
 **
 ******************************************************************************/
void I2C_DigitalFilterConfig(M4_I2C_TypeDef* pstcI2Cx, en_i2c_digital_filter_mode_t enDigiFilterMode)
{
    DDL_ASSERT(IS_VALID_UNIT(pstcI2Cx));
    DDL_ASSERT(IS_VALID_DIGITAL_FILTER(enDigiFilterMode));

    pstcI2Cx->FLTR_f.DNF = enDigiFilterMode;
}

/**
 *******************************************************************************
 ** \brief I2C digital filter function command
 **
 ** \param [in] pstcI2Cx            Pointer to the I2C peripheral register, can
 ** \                               be M4_I2C1,M4_I2C2 or M4_I2C3.
 ** \param [in] enNewState          New state of the I2Cx function, can be
 **                                 Disable or Enable the function
 **
 ** \retval None
 **
 ******************************************************************************/
void I2C_DigitalFilterCmd(M4_I2C_TypeDef* pstcI2Cx, en_functional_state_t enNewState)
{
    DDL_ASSERT(IS_VALID_UNIT(pstcI2Cx));
    DDL_ASSERT(IS_VALID_FUNCTION_STATE(enNewState));

    pstcI2Cx->FLTR_f.DNFEN = enNewState;
}

/**
 *******************************************************************************
 ** \brief I2C analog filter function command
 **
 ** \param [in] pstcI2Cx            Pointer to the I2C peripheral register, can
 ** \                               be M4_I2C1,M4_I2C2 or M4_I2C3.
 ** \param [in] enNewState          New state of the I2Cx function, can be
 **                                 Disable or Enable the function
 **
 ** \retval None
 **
 ******************************************************************************/
void I2C_AnalogFilterCmd(M4_I2C_TypeDef* pstcI2Cx, en_functional_state_t enNewState)
{
    DDL_ASSERT(IS_VALID_UNIT(pstcI2Cx));
    DDL_ASSERT(IS_VALID_FUNCTION_STATE(enNewState));

    pstcI2Cx->FLTR_f.ANFEN = enNewState;
}

/**
 *******************************************************************************
 ** \brief I2C general call function command
 **
 ** \param [in] pstcI2Cx            Pointer to the I2C peripheral register, can
 ** \                               be M4_I2C1,M4_I2C2 or M4_I2C3.
 ** \param [in] enNewState          New state of the I2Cx function, can be
 **                                 Disable or Enable the function
 **
 ** \retval None
 **
 ******************************************************************************/
void I2C_GeneralCallCmd(M4_I2C_TypeDef* pstcI2Cx, en_functional_state_t enNewState)
{
    DDL_ASSERT(IS_VALID_UNIT(pstcI2Cx));
    DDL_ASSERT(IS_VALID_FUNCTION_STATE(enNewState));

    pstcI2Cx->CR1_f.ENGC = enNewState;
}

/**
 *******************************************************************************
 ** \brief I2C slave address0 config
 **
 ** \param [in] pstcI2Cx            Pointer to the I2C peripheral register, can
 ** \                               be M4_I2C1,M4_I2C2 or M4_I2C3.
 ** \param [in] enNewState          New state of the I2Cx function, can be
 **                                 Disable or Enable the function
 ** \param [in] enAdrMode           Address mode,can be Adr7bit or Adr10bit
 ** \param [in] u8Adr               The slave address
 **
 ** \retval None
 **
 ******************************************************************************/
void I2C_SlaveAdr0Config(M4_I2C_TypeDef* pstcI2Cx, en_functional_state_t enNewState, en_address_bit_t enAdrMode, uint8_t u8Adr)
{
    DDL_ASSERT(IS_VALID_UNIT(pstcI2Cx));
    DDL_ASSERT(IS_VALID_FUNCTION_STATE(enNewState));
    DDL_ASSERT(IS_VALID_ADRMODE(enAdrMode));

    pstcI2Cx->SLR0_f.SLADDR0EN = enNewState;
    pstcI2Cx->SLR0_f.ADDRMOD0 = enAdrMode;
    if(Adr7bit == enAdrMode)
    {
        pstcI2Cx->SLR0_f.SLADDR0 = (u8Adr & 0x7F) << 1;
    }
    else
    {
        pstcI2Cx->SLR0_f.SLADDR0 = u8Adr;
    }
}

/**
 *******************************************************************************
 ** \brief I2C slave address1 config
 **
 ** \param [in] pstcI2Cx            Pointer to the I2C peripheral register, can
 ** \                               be M4_I2C1,M4_I2C2 or M4_I2C3.
 ** \param [in] enNewState          New state of the I2Cx function, can be
 **                                 Disable or Enable the function
 ** \param [in] enAdrMode           Address mode,can be Adr7bit or Adr10bit
 ** \param [in] u8Adr               The slave address
 **
 ** \retval None
 **
 ******************************************************************************/
void I2C_SlaveAdr1Config(M4_I2C_TypeDef* pstcI2Cx, en_functional_state_t enNewState, en_address_bit_t enAdrMode, uint8_t u8Adr)
{
    DDL_ASSERT(IS_VALID_UNIT(pstcI2Cx));
    DDL_ASSERT(IS_VALID_FUNCTION_STATE(enNewState));
    DDL_ASSERT(IS_VALID_ADRMODE(enAdrMode));

    pstcI2Cx->SLR1_f.SLADDR1EN = enNewState;
    pstcI2Cx->SLR1_f.ADDRMOD1 = enAdrMode;
    pstcI2Cx->SLR1_f.SLADDR1 = u8Adr;
}


/**
 *******************************************************************************
 ** \brief I2C fast ACK config
 **
 ** \param [in] pstcI2Cx            Pointer to the I2C peripheral register, can
 ** \                               be M4_I2C1,M4_I2C2 or M4_I2C3.
 ** \param [in] enNewState          New state of the fast ACK function, can be
 **                                 Disable or Enable the function
 **
 ** \retval None
 **
 ******************************************************************************/
void I2C_FastAckConfig(M4_I2C_TypeDef* pstcI2Cx, en_functional_state_t enNewState)
{
    DDL_ASSERT(IS_VALID_UNIT(pstcI2Cx));
    DDL_ASSERT(IS_VALID_FUNCTION_STATE(enNewState));

    pstcI2Cx->CR3_f.FACKEN = enNewState;
}

/**
 *******************************************************************************
 ** \brief I2C clock timer out function config
 **
 ** \param [in] pstcI2Cx            Pointer to the I2C peripheral register, can
 ** \                               be M4_I2C1,M4_I2C2 or M4_I2C3.
 ** \param [in] pstcTimoutInit      Pointer to I2C timeout function structure
 **
 ** \retval None
 **
 ******************************************************************************/
void I2C_ClkTimeOutConfig(M4_I2C_TypeDef* pstcI2Cx, const stc_clock_timeout_init_t* pstcTimoutInit)
{
    DDL_ASSERT(IS_VALID_UNIT(pstcI2Cx));
    DDL_ASSERT(IS_VALID_POINTER(pstcTimoutInit));
    DDL_ASSERT(IS_VALID_TIMOUT_SWITCH(pstcTimoutInit->enClkTimeOutSwitch));

    pstcI2Cx->SLTR_f.TOUTHIGH = pstcTimoutInit->u16TimeOutHigh;
    pstcI2Cx->SLTR_f.TOUTLOW = pstcTimoutInit->u16TimeOutLow;

    pstcI2Cx->CR3 &= ~0x00000007;
    pstcI2Cx->CR3 |= pstcTimoutInit->enClkTimeOutSwitch;
}

/**
 *******************************************************************************
 ** \brief I2C interrupt function command
 **
 ** \param [in] pstcI2Cx            Pointer to the I2C peripheral register, can
 ** \                               be M4_I2C1,M4_I2C2 or M4_I2C3.
 ** \param [in] u32IntEn            Specifies the I2C interrupts sources to be configuration
 **                                 This parameter can be any combination of the following values:
 **                                 I2C_CR2_STARTIE
 **                                 I2C_CR2_SLADDR0EN
 **                                 I2C_CR2_SLADDR1EN
 **                                 I2C_CR2_TENDIE
 **                                 I2C_CR2_STOPIE
 **                                 I2C_CR2_RFULLIE
 **                                 I2C_CR2_TEMPTYIE
 **                                 I2C_CR2_ARLOIE
 **                                 I2C_CR2_NACKIE
 **                                 I2C_CR2_TMOURIE
 **                                 I2C_CR2_GENCALLIE
 **                                 I2C_CR2_SMBDEFAULTIE
 **                                 I2C_CR2_SMBHOSTIE
 **                                 I2C_CR2_SMBALRTIE
 ** \param [in] enNewState          New state of the I2Cx function, can be
 **                                 Disable or Enable the function
 **
 ** \retval None
 **
 ******************************************************************************/
void I2C_IntCmd(M4_I2C_TypeDef* pstcI2Cx, uint32_t u32IntEn, en_functional_state_t enNewState)
{
    DDL_ASSERT(IS_VALID_UNIT(pstcI2Cx));
    DDL_ASSERT(IS_VALID_FUNCTION_STATE(enNewState));

    if(Enable == enNewState)
    {
        pstcI2Cx->CR2 |= u32IntEn;
    }
    else
    {
        pstcI2Cx->CR2 &= (uint32_t)~u32IntEn;
    }
}

/**
 *******************************************************************************
 ** \brief I2C generate start condition
 **
 ** \param [in] pstcI2Cx            Pointer to the I2C peripheral register, can
 **                                 be M4_I2C1,M4_I2C2 or M4_I2C3.
 ** \param [in] enNewState          new state of the I2Cx function, can be
 **                                 Disable or Enable the function
 **
 ** \retval None
 **
 ******************************************************************************/
void I2C_GenerateStart(M4_I2C_TypeDef* pstcI2Cx, en_functional_state_t enNewState)
{
    DDL_ASSERT(IS_VALID_UNIT(pstcI2Cx));
    DDL_ASSERT(IS_VALID_FUNCTION_STATE(enNewState));

    pstcI2Cx->CR1_f.START = enNewState;
}

/**
 *******************************************************************************
 ** \brief I2C generate restart condition
 **
 ** \param [in] pstcI2Cx            Pointer to the I2C peripheral register, can
 **                                 be M4_I2C1,M4_I2C2 or M4_I2C3.
 ** \param [in] enNewState          New state of the I2Cx function, can be
 **                                 Disable or Enable the function
 **
 ** \retval None
 **
 ******************************************************************************/
void I2C_GenerateReStart(M4_I2C_TypeDef* pstcI2Cx, en_functional_state_t enNewState)
{
    DDL_ASSERT(IS_VALID_UNIT(pstcI2Cx));
    DDL_ASSERT(IS_VALID_FUNCTION_STATE(enNewState));

    pstcI2Cx->CR1_f.RESTART = enNewState;

}

/**
 *******************************************************************************
 ** \brief I2C generate stop condition
 **
 ** \param [in] pstcI2Cx            Pointer to the I2C peripheral register, can
 **                                 be M4_I2C1,M4_I2C2 or M4_I2C3.
 ** \param [in] enNewState          New state of the I2Cx function, can be
 **                                 Disable or Enable the function
 **
 ** \retval None
 **
 ******************************************************************************/
void I2C_GenerateStop(M4_I2C_TypeDef* pstcI2Cx, en_functional_state_t enNewState)
{
    DDL_ASSERT(IS_VALID_UNIT(pstcI2Cx));
    DDL_ASSERT(IS_VALID_FUNCTION_STATE(enNewState));

    pstcI2Cx->CR1_f.STOP = enNewState;
}

/**
 *******************************************************************************
 ** \brief I2C send data or address
 **
 ** \param [in] pstcI2Cx            Pointer to the I2C peripheral register, can
 **                                 be M4_I2C1,M4_I2C2 or M4_I2C3.
 ** \param [in] u8Data              The data to be send
 **
 ** \retval None
 **
 ******************************************************************************/
void I2C_SendData(M4_I2C_TypeDef* pstcI2Cx, uint8_t u8Data)
{
    DDL_ASSERT(IS_VALID_UNIT(pstcI2Cx));

    pstcI2Cx->DTR = u8Data;
}

/**
 *******************************************************************************
 ** \brief I2C read data from register
 **
 ** \param [in] pstcI2Cx            Pointer to the I2C peripheral register, can
 **                                 be M4_I2C1,M4_I2C2 or M4_I2C3.
 **
 ** \retval     The value of the received data
 **
 ******************************************************************************/
uint8_t I2C_ReadData(M4_I2C_TypeDef* pstcI2Cx)
{
    DDL_ASSERT(IS_VALID_UNIT(pstcI2Cx));

    return pstcI2Cx->DRR;
}

/**
 *******************************************************************************
 ** \brief I2C ACK status configuration
 **
 ** \param [in] pstcI2Cx            Pointer to the I2C peripheral register, can
 **                                 be M4_I2C1,M4_I2C2 or M4_I2C3.
 ** \param [in] enNewState          New state of the I2Cx function, can be
 **                                 Disable or Enable the function
 **
 ** \retval  None
 **
 ******************************************************************************/
void I2C_NackConfig(M4_I2C_TypeDef* pstcI2Cx, en_functional_state_t enNewState)
{
    DDL_ASSERT(IS_VALID_UNIT(pstcI2Cx));
    DDL_ASSERT(IS_VALID_FUNCTION_STATE(enNewState));

    pstcI2Cx->CR1_f.ACK = enNewState;
}

/**
 *******************************************************************************
 ** \brief I2C status bit get
 **
 ** \param [in] pstcI2Cx            Pointer to the I2C peripheral register, can
 **                                 be M4_I2C1,M4_I2C2 or M4_I2C3.
 ** \param [in] u32StatusBit        specifies the flag to check,
 **                                 This parameter can be one of the following values:
 **                                 I2C_SR_STARTF
 **                                 I2C_SR_SLADDR0F
 **                                 I2C_SR_SLADDR1F
 **                                 I2C_SR_TENDF
 **                                 I2C_SR_STOPF
 **                                 I2C_SR_RFULLF
 **                                 I2C_SR_TEMPTYF
 **                                 I2C_SR_ARLOF
 **                                 I2C_SR_NACKDETECTF
 **                                 I2C_SR_NACKSENDF
 **                                 I2C_SR_TMOUTF
 **                                 I2C_SR_MSL
 **                                 I2C_SR_BUSY
 **                                 I2C_SR_TRA
 **                                 I2C_SR_GENCALLF
 **                                 I2C_SR_SMBDEFAULTF
 **                                 I2C_SR_SMBHOSTF
 **                                 I2C_SR_SMBALRTF
 **
 ** \retval     The status of the I2C status flag, may be Set or Reset.
 **
 ******************************************************************************/
en_flag_status_t I2C_GetStatus(M4_I2C_TypeDef* pstcI2Cx, uint32_t u32StatusBit)
{
    DDL_ASSERT(IS_VALID_UNIT(pstcI2Cx));
    DDL_ASSERT(IS_VALID_RD_STATUS_BIT(u32StatusBit));

    if(0 != (pstcI2Cx->SR & u32StatusBit))
    {
        return Set;
    }
    else
    {
        return Reset;
    }
}

/**
 *******************************************************************************
 ** \brief I2C status bit get
 **
 ** \param [in] pstcI2Cx            Pointer to the I2C peripheral register, can
 **                                 be M4_I2C1,M4_I2C2 or M4_I2C3.
 ** \param [in] u32StatusBit        specifies the flag to check,
 **                                 This parameter can be one of the following values:
 **                                 I2C_SR_MSL
 **                                 I2C_SR_TRA
 ** \param [in] enStatus            Ret or Reset the specified status bit.
 **
 ** \retval     None
 **
 ******************************************************************************/
void  I2C_WriteStatus(M4_I2C_TypeDef* pstcI2Cx, uint32_t u32StatusBit, en_flag_status_t enStatus)
{
    DDL_ASSERT(IS_VALID_UNIT(pstcI2Cx));
    DDL_ASSERT(IS_VALID_WR_STATUS_BIT(u32StatusBit));

    if(Set == enStatus)
    {
        pstcI2Cx->SR |= u32StatusBit;
    }
    else
    {
        pstcI2Cx->SR &= (~u32StatusBit);
    }
}

/**
 *******************************************************************************
 ** \brief Clear I2C status flag
 **
 ** \param [in] pstcI2Cx            Pointer to the I2C peripheral register, can
 **                                 be M4_I2C1,M4_I2C2 or M4_I2C3.
 ** \param [in] u32StatusBit        specifies the flag to clear,
 **                                 This parameter can be any combination of the following values:
 **                                 I2C_CLR_STARTFCLR
 **                                 I2C_CLR_SLADDR0FCLR
 **                                 I2C_CLR_SLADDR1FCLR
 **                                 I2C_CLR_TENDFCLR
 **                                 I2C_CLR_STOPFCLR
 **                                 I2C_CLR_RFULLFCLR
 **                                 I2C_CLR_TEMPTYFCLR
 **                                 I2C_CLR_ARLOFCLR
 **                                 I2C_CLR_NACKFCLR
 **                                 I2C_CLR_TMOUTFCLR
 **                                 I2C_CLR_GENCALLFCLR
 **                                 I2C_CLR_SMBDEFAULTFCLR
 **                                 I2C_CLR_SMBHOSTFCLR
 **                                 I2C_CLR_SMBALRTFCLR
 **
 ** \retval     None
 **
 ******************************************************************************/
void I2C_ClearStatus(M4_I2C_TypeDef* pstcI2Cx, uint32_t u32StatusBit)
{
    DDL_ASSERT(IS_VALID_UNIT(pstcI2Cx));

    pstcI2Cx->CLR |= (u32StatusBit & I2C_CLR_MASK);
}

//@} // I2cGroup

#endif /* DDL_I2C_ENABLE */

/*******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/
