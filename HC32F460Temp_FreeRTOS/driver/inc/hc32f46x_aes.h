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
/** \file hc32f46x_aes.h
 **
 ** A detailed description is available at
 ** @link AesGroup Aes description @endlink
 **
 **   - 2018-10-20  1.0 Wuze First version for Device Driver Library of Aes.
 **
 ******************************************************************************/
#ifndef __HC32F46X_AES_H__
#define __HC32F46X_AES_H__

/*******************************************************************************
 * Include files
 ******************************************************************************/
#include "hc32_common.h"
#include "ddl_config.h"

#if (DDL_AES_ENABLE == DDL_ON)

/* C binding of definitions if building with C++ compiler */
#ifdef __cplusplus
extern "C"
{
#endif

/**
 *******************************************************************************
 ** \defgroup AesGroup Advanced Encryption Standard(AES)
 **
 ******************************************************************************/
//@{

/*******************************************************************************
 * Global type definitions ('typedef')
 ******************************************************************************/
/* AES padding type. */
typedef enum en_aes_padding
{
    AesPadding_NONE     = 0,        ///< No padding, the size of source data in bytes must be N * 16 (N >= 1).

    AesPadding_PKCS5    = 1,        ///< Size(in bytes) is N, N % 16 = x:
                                    ///< 1. x == 0, padding 16(count) 0x10(data),
                                    ///< the size of source data in bytes is N + 16
                                    ///< 2. x != 0, padding (16 - x)(count) (16 - x)(data),
                                    ///< the size of source data in bytes is N + (16 - x)

    AesPadding_PKCS7    = 2,        ///< Same as AesPadding_PKCS5

    AesPadding_ZEROS    = 3,        ///< Size(in bytes) is N, N % 16 = x:
                                    ///< 1. x == 0, padding 16(count) 0x00(data),
                                    ///< the size of source data in bytes is N + 16
                                    ///< 2. x != 0, padding (16 - x)(count) 0x00(data),
                                    ///< the size of source data in bytes is N + (16 - x)

    AesPadding_ANSIX923 = 4,        ///< Size(in bytes) is N, N % 16 = x
                                    ///< 1. x == 0, padding 15(count) 0x00(data) and 1(count) 0x10(data),
                                    ///< the size of source data in bytes is N + 16
                                    ///< 2. x != 0, padding (15 - x)(count) 0x00(data) and 1(count) (16 - x)(data),
                                    ///< the size of source data in bytes is N + (16 - x)
} en_aes_padding_t;

/*******************************************************************************
 * Global pre-processor symbols/macros ('#define')
 ******************************************************************************/

/*******************************************************************************
 * Global variable definitions ('extern')
 ******************************************************************************/

/*******************************************************************************
 * Global function prototypes (definition in C source)
 ******************************************************************************/
void AES_Init(void);
void AES_DeInit(void);

en_result_t AES_Encrypt(const uint8_t *pu8Plaintext,
                        uint32_t u32PlaintextSize,
                        en_aes_padding_t enPadding,
                        const uint8_t *pu8Key,
                        uint8_t *pu8Ciphertext,
                        uint32_t *pu32CiphertextSize);

en_result_t AES_Decrypt(const uint8_t *pu8Ciphertext,
                        uint32_t u32CiphertextSize,
                        const uint8_t *pu8Key,
                        uint8_t *pu8Plaintext);

//@} // AesGroup

#ifdef __cplusplus
}
#endif

#endif /* DDL_AES_ENABLE */

#endif /* __HC32F46X_AES_H__ */

/*******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/
