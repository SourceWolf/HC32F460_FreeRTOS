/*******************************************************************************
 * Copyright (C) 2016, Huada Semiconductor Co., Ltd. All rights reserved.
 *
 * This software is owned and published by:
 * Huada Semiconductor Co., Ltd. ("HDSC").
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
/** \file wm8731.c
 **
 ** \brief I2S codec WM8731 driver API functions
 **
 **   - 2018-11-13  1.0  Wangmin First version for Device Driver Library
 **
 ******************************************************************************/

/*******************************************************************************
 * Include files
 ******************************************************************************/
#include "hc32_ddl.h"
#include "wm8731.h"

/*******************************************************************************
 * Local type definitions ('typedef')
 ******************************************************************************/

/*******************************************************************************
 * Local pre-processor symbols/macros ('#define')
 ******************************************************************************/
#define TIMEOUT                         ((uint32_t)0x10000)

#define I2C_RET_OK                      0
#define I2C_RET_ERROR                   1

#define GENERATE_START                  0x00
#define GENERATE_RESTART                0x01

#define E2_ADDRESS_W                    0x00
#define E2_ADDRESS_R                    0x01

/*******************************************************************************
 * Global variable definitions (declared in header file with 'extern')
 ******************************************************************************/

/*******************************************************************************
 * Local function prototypes ('static')
 ******************************************************************************/

/*******************************************************************************
 * Local variable definitions ('static')
 ******************************************************************************/
static uint16_t u16LeftHpOut, u16RightHpOut;

/*******************************************************************************
 * Function implementation - global ('extern') and local ('static')
 ******************************************************************************/

/**
 ******************************************************************************
 ** \brief  Send start or restart condition
 **
 ** \param  none
 **
 ** \return Process result
 **         - I2C_RET_ERROR  Send start failed
 **         - I2C_RET_OK     Send start success
 ******************************************************************************/
static uint8_t I2C_StartOrRestart(M4_I2C_TypeDef* pstcI2c, uint8_t u8Start)
{
    uint32_t u32TimeOut = TIMEOUT;
    
    /* generate start or restart signal */
    if(GENERATE_START == u8Start)
    {
        /* Wait I2C bus idle */
        while(Set == I2C_GetStatus(pstcI2c, I2C_SR_BUSY))
        {
            if(0 == (u32TimeOut--)) return I2C_RET_ERROR;
        }
      
        I2C_GenerateStart(pstcI2c , Enable);
    }
    else
    {
        /* Clear start status flag */
        I2C_ClearStatus(pstcI2c, I2C_CLR_STARTFCLR);
        /* Send restart condition */
        I2C_GenerateReStart(pstcI2c , Enable);
    }

    /* Judge if start success*/
    u32TimeOut = TIMEOUT;
    while((Reset == I2C_GetStatus(pstcI2c, I2C_SR_BUSY)) ||
            (Reset == I2C_GetStatus(pstcI2c, I2C_SR_STARTF)))
    {
        if(0 == (u32TimeOut--)) return I2C_RET_ERROR;
    }
    
    return I2C_RET_OK;
}

/**
 ******************************************************************************
 ** \brief  Send e2prom device address
 **
 ** \param  u16Adr  The slave address
 **
 ** \return Process result
 **         - I2C_RET_ERROR  Send start failed
 **         - I2C_RET_OK     Send start success
 ******************************************************************************/
static uint8_t I2C_SendAdr(M4_I2C_TypeDef* pstcI2c, uint8_t u8Adr)
{
    uint32_t u32TimeOut = TIMEOUT;

    /* Wait tx buffer empty */
    while(Reset == I2C_GetStatus(pstcI2c, I2C_SR_TEMPTYF))
    {
        if(0 == (u32TimeOut--)) return I2C_RET_ERROR;
    }
    
    /* Send I2C address */
    I2C_SendData(pstcI2c, u8Adr);
   
    if(E2_ADDRESS_W == (u8Adr & 0x01))
    {
        /* If in master transfer process, Need wait transfer end*/
        uint32_t u32TimeOut = TIMEOUT;
        while(Reset == I2C_GetStatus(pstcI2c, I2C_SR_TENDF))
        {
            if(0 == (u32TimeOut--)) return I2C_RET_ERROR;
        }
        
        /* Check ACK */
        u32TimeOut = TIMEOUT;
        while(Set == I2C_GetStatus(pstcI2c, I2C_SR_NACKDETECTF))
        {
            if(0 == (u32TimeOut--)) return I2C_RET_ERROR;
        }
    }
    
    return I2C_RET_OK;
}


/**
 ******************************************************************************
 ** \brief  Send data to e2prom
 **
 ** \param  pTxData  Pointer to the data buffer
 ** \param  u32Size  Data size
 **
 ** \return Process result
 **         - I2C_RET_ERROR  Send start failed
 **         - I2C_RET_OK     Send start success
 ******************************************************************************/
static uint8_t I2C_WriteData(M4_I2C_TypeDef* pstcI2c, uint8_t *pTxData, uint32_t u32Size)
{
    uint32_t u32TimeOut = TIMEOUT;
    
    while(u32Size--)
    {
        /* Wait tx buffer empty */
        u32TimeOut = TIMEOUT;
        while(Reset == I2C_GetStatus(pstcI2c, I2C_SR_TEMPTYF))
        {
            if(0 == (u32TimeOut--)) return I2C_RET_ERROR;
        }
        
        /* Send one byte data */
        I2C_SendData(pstcI2c, *pTxData++);

        /* Wait transfer end*/
        u32TimeOut = TIMEOUT;
        while(Reset == I2C_GetStatus(pstcI2c, I2C_SR_TENDF))
        {
            if(0 == (u32TimeOut--)) return I2C_RET_ERROR;
        }
        
        /* Check ACK */
        u32TimeOut = TIMEOUT;
        while(Set == I2C_GetStatus(pstcI2c, I2C_SR_NACKDETECTF))
        {
            if(0 == (u32TimeOut--)) return I2C_RET_ERROR;
        }
    }
    
    return I2C_RET_OK;
}

/**
 ******************************************************************************
 ** \brief  Write address and receive data from e2prom
 **
 ** \param  u8Adr    Device address and R/W bit
 ** \param  pTxData  Pointer to the data buffer
 ** \param  u32Size  Data size
 **
 ** \return Process result
 **         - I2C_RET_ERROR  Send start failed
 **         - I2C_RET_OK     Send start success
 ******************************************************************************/
#if 0
static uint8_t I2C_SendAdrRevData(M4_I2C_TypeDef* pstcI2c, uint8_t u8Adr, uint8_t *pRxData, uint32_t u32Size)
{
    uint32_t u32TimeOut = TIMEOUT;

    /* Wait tx buffer empty */
    while(Reset == I2C_GetStatus(pstcI2c, I2C_SR_TEMPTYF))
    {
        if(0 == (u32TimeOut--)) return I2C_RET_ERROR;
    }
    
    for(uint32_t i=0; i<u32Size; i++)
    {
        /* if the last byte receive, need config NACK*/
        if(i == (u32Size - 1))
        {
            I2C_NackConfig(pstcI2c, Enable);
        }
        
        /* if first byte receive, need send adr*/
        if(0 == i)
        {
            I2C_SendData(pstcI2c, u8Adr);
        }
    
        /* Wait receive full flag*/
        u32TimeOut = TIMEOUT;
        while(Reset == I2C_GetStatus(pstcI2c, I2C_SR_RFULLF))
        {
            if(0 == (u32TimeOut--)) return I2C_RET_ERROR;
        }
        
        /* read data from register*/
        *pRxData++ = I2C_ReadData(pstcI2c);
        
    }
    
    return I2C_RET_OK;  
}
#endif
/**
 ******************************************************************************
 ** \brief  General stop condition to e2prom
 **
 ** \param  None
 **
 ** \return Process result
 **         - I2C_RET_ERROR  Send start failed
 **         - I2C_RET_OK     Send start success
 ******************************************************************************/
static uint8_t I2C_Stop(M4_I2C_TypeDef* pstcI2c)
{
    uint32_t u32TimeOut;
    
    /* Wait I2C bus busy */
    u32TimeOut = TIMEOUT;
    while(Reset == I2C_GetStatus(pstcI2c, I2C_SR_BUSY))
    {
        if(0 == (u32TimeOut--)) return I2C_RET_ERROR;
    }
  
    I2C_GenerateStop(pstcI2c, Enable);
    
    /* Wait STOPF */
    u32TimeOut = TIMEOUT;
    while(Reset == I2C_GetStatus(pstcI2c, I2C_SR_STOPF))
    {
        if(0 == (u32TimeOut--)) return I2C_RET_ERROR;
    }
    
    return I2C_RET_OK;
}

/**
 ******************************************************************************
 ** \brief Send command to WM8731
 **
 ** \param u8RegAddr Regist address
 ** \param u16Cmd    Command data
 **
 ** \return   Ok   Coded initialized
 ******************************************************************************/
void WM8731_SendCmd(M4_I2C_TypeDef* pstcI2c, uint8_t u8RegAddr, uint16_t u16Cmd){

    // B[15:9] Are Control Address Bits
    // B[8:0]  Are Control Data Bits
    uint8_t u8Data[2];
    uint8_t Ret;
  
    u8Data[0] = (uint8_t)((u8RegAddr << 1) | ((u16Cmd>>8) & 0x1));
    u8Data[1] = (uint8_t)(u16Cmd & 0xFF);

    Ret = I2C_StartOrRestart(pstcI2c, GENERATE_START);
    if(I2C_RET_ERROR == Ret) while(1);
    Ret = I2C_SendAdr(pstcI2c, (WM8731_ADDRESS << 1) | E2_ADDRESS_W);
    if(I2C_RET_ERROR == Ret) while(1);
    Ret = I2C_WriteData(pstcI2c, u8Data, 2);
    if(I2C_RET_ERROR == Ret) while(1);
    Ret = I2C_Stop(pstcI2c);
}

/**
 ******************************************************************************
 ** \brief  Initialize the I2C peripheral for e2prom
 **
 ** \param  None
 **
 ** \return Process result
 **         - I2C_RET_ERROR  Send start failed
 **         - I2C_RET_OK     Send start success
 ******************************************************************************/
uint8_t WM8731_Init(M4_I2C_TypeDef* pstcI2c, stc_wm8731_reg_t* pstcReg)
{
    stc_i2c_init_t stcI2cInit;
    
    I2C_DeInit(pstcI2c);
    
    MEM_ZERO_STRUCT(stcI2cInit);
    stcI2cInit.enI2cMode = I2cMaster;
    stcI2cInit.u32Baudrate = 100000;
    I2C_Init(pstcI2c, &stcI2cInit);
    
    I2C_Cmd(pstcI2c, Enable);

    WM8731_SendCmd(pstcI2c, WM8731_REG_RESET,         pstcReg->RESET);    // Reset module
    WM8731_SendCmd(pstcI2c, WM8731_REG_LLINE_IN,      pstcReg->LLIN);     // Left line in settings
    WM8731_SendCmd(pstcI2c, WM8731_REG_RLINE_IN,      pstcReg->RLIN);     // Rigth line in settings    
    WM8731_SendCmd(pstcI2c, WM8731_REG_LHPHONE_OUT,   pstcReg->LHOUT);    // Left headphone out settings
    WM8731_SendCmd(pstcI2c, WM8731_REG_RHPHONE_OUT,   pstcReg->RHOUT);    // Right headphone out settings
    WM8731_SendCmd(pstcI2c, WM8731_REG_ANALOG_PATH,   pstcReg->AAPC);     // Analog paths
    WM8731_SendCmd(pstcI2c, WM8731_REG_DIGITAL_PATH,  pstcReg->DAPC);     // Digital paths  
    WM8731_SendCmd(pstcI2c, WM8731_REG_PDOWN_CTRL,    pstcReg->PDC);      // Power down control
    WM8731_SendCmd(pstcI2c, WM8731_REG_DIGITAL_IF,    pstcReg->DAIF);     // Digital interface
    WM8731_SendCmd(pstcI2c, WM8731_REG_SAMPLING_CTRL, pstcReg->SC);       // Sampling control
    WM8731_SendCmd(pstcI2c, WM8731_REG_ACTIVE_CTRL,   pstcReg->AC);       // Activate module
    
    u16LeftHpOut = pstcReg->LHOUT;   // Store left headphone out data
    u16RightHpOut = pstcReg->RHOUT;  // Store right headphone out data
    
    return I2C_RET_OK;
}

/**
 ******************************************************************************
 ** \brief Set volume of headphone
 **
 ** \return   Ok   Coded initialized
 ******************************************************************************/
uint8_t WM8731_SetHpVolume(M4_I2C_TypeDef* pstcI2c, uint16_t u16LeftVol, uint16_t u16RightVol)
{
    u16LeftHpOut = u16LeftHpOut & 0xC0u;
    u16RightHpOut = u16RightHpOut & 0xC0u;
    
    u16LeftHpOut |= u16LeftVol;
    u16RightHpOut |= u16RightVol;
  
    WM8731_SendCmd(pstcI2c, WM8731_REG_LHPHONE_OUT, u16LeftHpOut);
    WM8731_SendCmd(pstcI2c, WM8731_REG_RHPHONE_OUT, u16RightHpOut); 

    return Ok;
}




/*******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/
