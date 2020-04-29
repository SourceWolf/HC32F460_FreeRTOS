/*****************************************************************************
 * Copyright (C) 2016, Huada Semiconductor Co.,Ltd All rights reserved.
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
 * REGARDING THE SOFTWARE (INCLUDING ANY ACCOMPANYING WRITTEN MATERIALS),
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
/** \file usb_bsp.h
 **
 ** A detailed description is available at
 ** @link Specific api's relative to the used hardware platform @endlink
 **
 **   - 2018-12-26  1.0  wangmin First version for USB demo.
 **
 ******************************************************************************/
#ifndef __USB_BSP__H__
#define __USB_BSP__H__

/*******************************************************************************
 * Include files
 ******************************************************************************/
#include "usb_core.h"
#include "usb_conf.h"
#include "hc32_ddl.h"

/*******************************************************************************
 * Global type definitions ('typedef')
 ******************************************************************************/

/*******************************************************************************
 * Global pre-processor symbols/macros ('#define')
 ******************************************************************************/
/* KEY0 */
#define  SW2_PORT       (PortD)
#define  SW2_PIN        (Pin03)

/* LED0 Port/Pin definition */
#define  LED0_PORT      (PortE)
#define  LED0_PIN       (Pin06)

/* LED1 Port/Pin definition */
#define  LED1_PORT      (PortA)
#define  LED1_PIN       (Pin07)

/* LED2 Port/Pin definition */
#define  LED2_PORT      (PortB)
#define  LED2_PIN       (Pin05)

/* LED3 Port/Pin definition */
#define  LED3_PORT      (PortB)
#define  LED3_PIN       (Pin09)

/* LED0~3 toggle definition */
#define  LED0_TOGGLE()    (PORT_Toggle(LED0_PORT, LED0_PIN))
#define  LED1_TOGGLE()    (PORT_Toggle(LED1_PORT, LED1_PIN))
#define  LED2_TOGGLE()    (PORT_Toggle(LED2_PORT, LED2_PIN))
#define  LED3_TOGGLE()    (PORT_Toggle(LED3_PORT, LED3_PIN))

/* LED0~3 Control definition */
#define  LED0_CTL(x)      ((Reset != (x))?PORT_SetBits(LED0_PORT, LED0_PIN):PORT_ResetBits(LED0_PORT, LED0_PIN))
#define  LED1_CTL(x)      ((Reset != (x))?PORT_SetBits(LED1_PORT, LED1_PIN):PORT_ResetBits(LED1_PORT, LED1_PIN))
#define  LED2_CTL(x)      ((Reset != (x))?PORT_SetBits(LED2_PORT, LED2_PIN):PORT_ResetBits(LED2_PORT, LED2_PIN))
#define  LED3_CTL(x)      ((Reset != (x))?PORT_SetBits(LED3_PORT, LED3_PIN):PORT_ResetBits(LED3_PORT, LED3_PIN))
/*******************************************************************************
 * Global variable definitions ('extern')
 ******************************************************************************/

/*******************************************************************************
  Global function prototypes (definition in C source)
 ******************************************************************************/
void BSP_Init(void);

void USB_OTG_BSP_Init (USB_OTG_CORE_HANDLE *pdev);
void USB_OTG_BSP_uDelay (const uint32_t usec);
void USB_OTG_BSP_mDelay (const uint32_t msec);
void USB_OTG_BSP_EnableInterrupt (void);
#ifdef USE_HOST_MODE
void USB_OTG_BSP_ConfigVBUS(USB_OTG_CORE_HANDLE *pdev);
void USB_OTG_BSP_DriveVBUS(USB_OTG_CORE_HANDLE *pdev,uint8_t state);
#endif

#endif //__USB_BSP__H__

/*******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/
