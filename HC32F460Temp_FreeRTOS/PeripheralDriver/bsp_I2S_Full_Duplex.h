#ifndef BSP_I2S_FULL_DUPLEX_H
#define BSP_I2S_FULL_DUPLEX_H
/*******************************************************************************
 * Include files
 ******************************************************************************/
#include "hc32_ddl.h"
#include "wm8731.h"
#include "User_Gpio.h"
#include "data_sound_i2s.h"
#include "System_InterruptCFG_Def.h"
/*******************************************************************************
 * Local type definitions ('typedef')
 ******************************************************************************/

/*******************************************************************************
 * Local pre-processor symbols/macros ('#define')
 ******************************************************************************/
/* Define if need play by speaker*/
#define SPEAKER_ON      1
/* Define if use exclk */
#define EXCK_ON         1
/* Select Record source */
#define RECORD_MIC      1
/* Define I2C unit used for the example */
#define I2C_CH                          M4_I2C2
/* Define port and pin for SDA and SCL */
#define I2C2_SCL_PORT                   PortD
#define I2C2_SCL_PIN                    Pin00
#define I2C2_SDA_PORT                   PortD
#define I2C2_SDA_PIN                    Pin01
/* Define I2S unit used for the example */
#define I2S_CH                          M4_I2S3
/* Define port and pin for i2s1 function */
#ifdef EXCK_ON
/* if exck enable*/
#define I2S3_PORT                       PortB
#define I2S3_WS_PIN                     Pin13
#define I2S3_SD_PIN                     Pin14
#define I2S3_SD_IN_PIN                  Pin15
#define I2S3_CK_PIN                     Pin12
#define I2S3_EXCK_PORT                  PortB
#define I2S3_EXCK_PIN                   Pin10
#else
/* if exck disable */
#define I2S3_PORT                       PortB
#define I2S3_WS_PIN                     Pin13
#define I2S3_SD_PIN                     Pin14
#define I2S3_SD_IN_PIN                  Pin15
#define I2S3_CK_PIN                     Pin10
#define I2S3_MCK_PORT                   PortB
#define I2S3_MCK_PIN                    Pin12
#endif

#define SPK_EN_PORT                     PortB
#define SPK_EN_PIN                      Pin00

#define SPEAKER_EN()                    PORT_SetBits(SPK_EN_PORT, SPK_EN_PIN)
#define SPEAKER_DISEN()                 PORT_ResetBits(SPK_EN_PORT, SPK_EN_PIN)

/* LED0 Port/Pin definition */
#define  LED0_PORT                      PortE
#define  LED0_PIN                       Pin06
/* LED0~1 toggle definition */
#define  LED0_TOGGLE()                  PORT_Toggle(LED0_PORT, LED0_PIN)

//#define DMA2_UNIT                (M4_DMA2)
//#define DMA2_CH0                 (DmaCh0)

#define RECORDER_WAVFILELEN             16000ul

#define RX_DMA_CLK				(PWC_FCG0_PERIPH_DMA2)
#define RX_DMA_UNIT             (M4_DMA2)
#define RX_DMA_CH               (DmaCh0)
#define RX_DMA_TRG				(EVT_I2S3_RXIRQOUT)
#define TIMERA_COUNT_TRG		RX_DMA_TRG

#define TX_DMA_CLK				(PWC_FCG0_PERIPH_DMA1)
#define TX_DMA_UNIT             (M4_DMA1)
#define TX_DMA_CH               (DmaCh0)
#define TX_DMA_TRG				(EVT_I2S3_TXIRQOUT)

#define MaxLlpnum (2u)
typedef union audio_buffer
{
    uint8_t Data8bit[RECORDER_WAVFILELEN*2];
    uint16_t Data16bit[RECORDER_WAVFILELEN];
}AudioBuff_type;


#ifdef __cplusplus
extern "C" {
#endif

int32_t i2s_record_duplex_init(void);

#ifdef __cplusplus
};
#endif


#endif

