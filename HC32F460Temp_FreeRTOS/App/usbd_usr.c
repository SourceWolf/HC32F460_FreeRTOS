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
/** \file usbd_usr.c
 **
 ** A detailed description is available at
 ** @link
        This file includes the user application layer.
    @endlink
 **
 **   - 2019-11-19  1.0  zhangxl First version for USB MSC_HID composite demo.
 **
 ******************************************************************************/

/*******************************************************************************
 * Include files
 ******************************************************************************/
#include "hc32_ddl.h"
#include "usbd_usr.h"
#include "usbd_ioreq.h"
#include "usb_conf.h"
#include "usb_bsp.h"

/*******************************************************************************
 * Local type definitions ('typedef')
 ******************************************************************************/

/*******************************************************************************
 * Local pre-processor symbols/macros ('#define')
 ******************************************************************************/
#define USER_INFORMATION1      (uint8_t*)"[Key]:RemoteWakeup"
#define USER_INFORMATION2      (uint8_t*)"[Joystick]:Mouse emulation"

/*******************************************************************************
 * Global variable definitions (declared in header file with 'extern')
 ******************************************************************************/
USBD_Usr_cb_TypeDef USR_cb = {
    USBD_USR_Init,
    USBD_USR_DeviceReset,
    USBD_USR_DeviceConfigured,
    USBD_USR_DeviceSuspended,
    USBD_USR_DeviceResumed,
    USBD_USR_DeviceConnected,
    USBD_USR_DeviceDisconnected,
};

/*******************************************************************************
 * Local function prototypes ('static')
 ******************************************************************************/

/*******************************************************************************
 * Local variable definitions ('static')
 ******************************************************************************/

/*******************************************************************************
 * Function implementation - global ('extern') and local ('static')
 ******************************************************************************/
/**
* @brief  USBD_USR_Init
*
* @param  None
* @retval None
*/
void USBD_USR_Init(void)
{

}

/**
* @brief  USBD_USR_DeviceReset
*         Displays the message on terminal on device Reset Event
* @param  speed : device speed
* @retval None
*/
void USBD_USR_DeviceReset(uint8_t speed)
{
    switch (speed)
    {
        case USB_OTG_SPEED_HIGH:
            printf("     USB Device Library V1.2.1 [HS]");
            break;

        case USB_OTG_SPEED_FULL:
            printf("     USB Device Library V1.2.1 [FS]");
            break;
        default:
            printf("     USB Device Library V1.2.1 [??]");
    }
}


/**
* @brief  USBD_USR_DeviceConfigured
*         Displays the message on on terminal on device configuration Event
* @param  None
* @retval Status
*/
void USBD_USR_DeviceConfigured(void)
{
    printf("> HID Interface started.\n");
    printf("> MSC Interface started.\n");
}


/**
* @brief  USBD_USR_DeviceConnected
*         Displays the message on on terminal on device connection Event
* @param  None
* @retval Status
*/
void USBD_USR_DeviceConnected(void)
{
    printf("> USB Device Connected.\n");
}


/**
* @brief  USBD_USR_DeviceDisonnected
*         Displays the message on terminal on device disconnection Event
* @param  None
* @retval Status
*/
void USBD_USR_DeviceDisconnected(void)
{
    printf("> USB Device Disconnected.\n");
}

/**
* @brief  USBD_USR_DeviceSuspended
*         Displays the message on terminal on device suspend Event
* @param  None
* @retval None
*/
void USBD_USR_DeviceSuspended(void)
{
    printf("> USB Device in Suspend Mode.\n");
    /* Users can do their application actions here for the USB-Reset */
}


/**
* @brief  USBD_USR_DeviceResumed
*         Displays the message on terminal on device resume Event
* @param  None
* @retval None
*/
void USBD_USR_DeviceResumed(void)
{
    printf("> USB Device in Idle Mode.\n");
    /* Users can do their application actions here for the USB-Reset */
}

/*******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/
