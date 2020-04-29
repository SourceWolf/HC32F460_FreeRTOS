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
/** \file usbd_desc.c
 **
 ** A detailed description is available at
 ** @link
        This file provides the USBD descriptors and string formating method.
    @endlink
 **
 **   - 2019-11-19  1.0  zhangxl First version for USB MSC_HID composite demo.
 **
 ******************************************************************************/

/*******************************************************************************
 * Include files
 ******************************************************************************/
#include "usbd_core.h"
#include "usbd_desc.h"
#include "usbd_req.h"
#include "usbd_conf.h"
#include "usb_otg_regs.h"

/*******************************************************************************
 * Local pre-processor symbols/macros ('#define')
 ******************************************************************************/
#define USBD_VID                        0x2E88
#define USBD_PID                        0x4607

#define USBD_LANGID_STRING              0x409
#define USBD_MANUFACTURER_STRING        "HDSC_mcu"
#define USBD_PRODUCT_HS_STRING          "Composite in HS Mode"
#define USBD_PRODUCT_FS_STRING          "Composite in FS Mode"
#define USBD_CONFIGURATION_HS_STRING    "COMPOSITE Config"
#define USBD_INTERFACE_HS_STRING        "COMPOSITE Interface"
#define USBD_CONFIGURATION_FS_STRING    "COMPOSITE Config"
#define USBD_INTERFACE_FS_STRING        "COMPOSITE Interface"

/*******************************************************************************
 * Local function prototypes ('static')
 ******************************************************************************/
static void IntToUnicode(uint32_t value, uint8_t * pbuf, uint8_t len);
static void Get_SerialNum(void);

/*******************************************************************************
 * Local variable definitions ('static')
 ******************************************************************************/
USBD_DEVICE USR_desc =
{
    USBD_USR_DeviceDescriptor,
    USBD_USR_LangIDStrDescriptor,
    USBD_USR_ManufacturerStrDescriptor,
    USBD_USR_ProductStrDescriptor,
    USBD_USR_SerialStrDescriptor,
    USBD_USR_ConfigStrDescriptor,
    USBD_USR_InterfaceStrDescriptor,
};

#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
#if defined ( __ICCARM__ )      /* !< IAR Compiler */
#pragma data_alignment=4
#endif
#endif                          /* USB_OTG_HS_INTERNAL_DMA_ENABLED */
/* USB Standard Device Descriptor */
__USB_ALIGN_BEGIN uint8_t USBD_DeviceDesc[USB_SIZ_DEVICE_DESC] __USB_ALIGN_END =
{
    0x12,                         /* bLength */
    USB_DEVICE_DESCRIPTOR_TYPE,   /* bDescriptorType */
    0x00,                         /* bcdUSB */
    0x02,
    0x00,                         /* bDeviceClass */
    0x00,                         /* bDeviceSubClass */
    0x00,                         /* bDeviceProtocol */
    USB_OTG_MAX_EP0_SIZE,         /* bMaxPacketSize */
    LOBYTE(USBD_VID),             /* idVendor */
    HIBYTE(USBD_VID),             /* idVendor */
    LOBYTE(USBD_PID),             /* idVendor */
    HIBYTE(USBD_PID),             /* idVendor */
    0x00,                         /* bcdDevice rel. 2.00 */
    0x02,
    USBD_IDX_MFC_STR,             /* Index of manufacturer string */
    USBD_IDX_PRODUCT_STR,         /* Index of product string */
    USBD_IDX_SERIAL_STR,          /* Index of serial number string */
    USBD_CFG_MAX_NUM              /* bNumConfigurations */
};                              /* USB_DeviceDescriptor */

#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
#if defined ( __ICCARM__ )      /* !< IAR Compiler */
#pragma data_alignment=4
#endif
#endif                          /* USB_OTG_HS_INTERNAL_DMA_ENABLED */
/* USB Standard Device Descriptor */
__USB_ALIGN_BEGIN uint8_t USBD_DeviceQualifierDesc[USB_LEN_DEV_QUALIFIER_DESC] __USB_ALIGN_END =
{
    USB_LEN_DEV_QUALIFIER_DESC,
    USB_DESC_TYPE_DEVICE_QUALIFIER,
    0x00,
    0x02,
    0x00,
    0x00,
    0x00,
    0x40,
    0x01,
    0x00,
};

#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
#if defined ( __ICCARM__ )      /* !< IAR Compiler */
#pragma data_alignment=4
#endif
#endif                          /* USB_OTG_HS_INTERNAL_DMA_ENABLED */
/* USB Standard Device Descriptor */
__USB_ALIGN_BEGIN uint8_t USBD_LangIDDesc[USB_SIZ_STRING_LANGID] __USB_ALIGN_END =
{
    USB_SIZ_STRING_LANGID,
    USB_DESC_TYPE_STRING,
    LOBYTE(USBD_LANGID_STRING),
    HIBYTE(USBD_LANGID_STRING),
};

uint8_t USBD_StringSerial[USB_SIZ_STRING_SERIAL] =
{
    USB_SIZ_STRING_SERIAL,
    USB_DESC_TYPE_STRING,
};

/*******************************************************************************
 * Function implementation - global ('extern') and local ('static')
 ******************************************************************************/
/**
* @brief  USBD_USR_DeviceDescriptor
*         return the device descriptor
* @param  speed : current device speed
* @param  length : pointer to data length variable
* @retval pointer to descriptor buffer
*/
uint8_t *USBD_USR_DeviceDescriptor(uint8_t speed, uint16_t * length)
{
    *length = sizeof(USBD_DeviceDesc);
    return (uint8_t *) USBD_DeviceDesc;
}

/**
* @brief  USBD_USR_LangIDStrDescriptor
*         return the LangID string descriptor
* @param  speed : current device speed
* @param  length : pointer to data length variable
* @retval pointer to descriptor buffer
*/
uint8_t *USBD_USR_LangIDStrDescriptor(uint8_t speed, uint16_t * length)
{
    *length = sizeof(USBD_LangIDDesc);
    return (uint8_t *) USBD_LangIDDesc;
}

/**
* @brief  USBD_USR_ProductStrDescriptor
*         return the product string descriptor
* @param  speed : current device speed
* @param  length : pointer to data length variable
* @retval pointer to descriptor buffer
*/
uint8_t *USBD_USR_ProductStrDescriptor(uint8_t speed, uint16_t * length)
{
    if (speed == 0)
    {
        USBD_GetString((uint8_t *) (uint8_t *) USBD_PRODUCT_HS_STRING, USBD_StrDesc, length);
    }
    else
    {
        USBD_GetString((uint8_t *) (uint8_t *) USBD_PRODUCT_FS_STRING, USBD_StrDesc, length);
    }
    return USBD_StrDesc;
}

/**
* @brief  USBD_USR_ManufacturerStrDescriptor
*         return the manufacturer string descriptor
* @param  speed : current device speed
* @param  length : pointer to data length variable
* @retval pointer to descriptor buffer
*/
uint8_t *USBD_USR_ManufacturerStrDescriptor(uint8_t speed, uint16_t * length)
{
    USBD_GetString((uint8_t *) (uint8_t *) USBD_MANUFACTURER_STRING, USBD_StrDesc, length);
    return USBD_StrDesc;
}

/**
* @brief  USBD_USR_SerialStrDescriptor
*         return the serial number string descriptor
* @param  speed : current device speed
* @param  length : pointer to data length variable
* @retval pointer to descriptor buffer
*/
uint8_t *USBD_USR_SerialStrDescriptor(uint8_t speed, uint16_t * length)
{
    *length = USB_SIZ_STRING_SERIAL;

    /* Update the serial number string descriptor with the data from the unique
    * ID */
    Get_SerialNum();

    return (uint8_t *) USBD_StringSerial;
}

/**
* @brief  USBD_USR_ConfigStrDescriptor
*         return the configuration string descriptor
* @param  speed : current device speed
* @param  length : pointer to data length variable
* @retval pointer to descriptor buffer
*/
uint8_t *USBD_USR_ConfigStrDescriptor(uint8_t speed, uint16_t * length)
{
    if (speed == USB_OTG_SPEED_HIGH)
    {
        USBD_GetString((uint8_t *) (uint8_t *) USBD_CONFIGURATION_HS_STRING, USBD_StrDesc, length);
    }
    else
    {
        USBD_GetString((uint8_t *) (uint8_t *) USBD_CONFIGURATION_FS_STRING, USBD_StrDesc, length);
    }
    return USBD_StrDesc;
}


/**
* @brief  USBD_USR_InterfaceStrDescriptor
*         return the interface string descriptor
* @param  speed : current device speed
* @param  length : pointer to data length variable
* @retval pointer to descriptor buffer
*/
uint8_t *USBD_USR_InterfaceStrDescriptor(uint8_t speed, uint16_t * length)
{
    if (speed == 0)
    {
        USBD_GetString((uint8_t *) (uint8_t *) USBD_INTERFACE_HS_STRING, USBD_StrDesc, length);
    }
    else
    {
        USBD_GetString((uint8_t *) (uint8_t *) USBD_INTERFACE_FS_STRING, USBD_StrDesc, length);
    }
    return USBD_StrDesc;
}

/**
  * @brief  Create the serial number string descriptor
  * @param  None
  * @retval None
  */
static void Get_SerialNum(void)
{
    uint32_t deviceserial0, deviceserial1, deviceserial2;

    deviceserial0 = *(uint32_t *) DEVICE_ID1;
    deviceserial1 = *(uint32_t *) DEVICE_ID2;
    deviceserial2 = *(uint32_t *) DEVICE_ID3;

    deviceserial0 += deviceserial2;

    if (deviceserial0 != 0)
    {
        IntToUnicode(deviceserial0, &USBD_StringSerial[2], 8);
        IntToUnicode(deviceserial1, &USBD_StringSerial[18], 4);
    }
}

/**
  * @brief  Convert Hex 32Bits value into char
  * @param  value: value to convert
  * @param  pbuf: pointer to the buffer
  * @param  len: buffer length
  * @retval None
  */
static void IntToUnicode(uint32_t value, uint8_t * pbuf, uint8_t len)
{
    uint8_t idx = 0;

    for (idx = 0; idx < len; idx++)
    {
        if (((value >> 28)) < 0xA)
        {
            pbuf[2 * idx] = (value >> 28) + '0';
        }
        else
        {
            pbuf[2 * idx] = (value >> 28) + 'A' - 10;
        }

        value = value << 4;

        pbuf[2 * idx + 1] = 0;
    }
}

/*******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/
