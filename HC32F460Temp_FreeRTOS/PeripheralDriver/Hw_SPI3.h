#ifndef HW_SPI3_H
#define HW_SPI3_H
#include "hc32_ddl.h"
#define SPI3_TX_IRQn            Int004_IRQn
#define SPI3_RX_IRQn            Int005_IRQn
#define SPI3_ERR_IRQn           Int006_IRQn
#define SPI3_IDEL_IRQn          Int007_IRQn
#define DMA2_CH0_IRQn           Int003_IRQn
//unsigned char SPI_DATA;
/* Choose SPI master or slave mode */
#define SPI_MASTER_MODE
//#define SPI_SLAVE_MODE
/* SPI_SCK Port/Pin definition */
#define SPI3_SCK_PORT                    PortE
#define SPI3_SCK_PIN                     Pin00
#define SPI3_SCK_FUNC                    Func_Spi3_Sck

/* SPI_NSS Port/Pin definition */
#define SPI3_NSS_PORT                    PortE
#define SPI3_NSS_PIN                     Pin01
#define SPI3_NSS_FUNC                    Func_Spi3_Nss0

/* SPI_MOSI Port/Pin definition */
#define SPI3_MOSI_PORT                   PortE
#define SPI3_MOSI_PIN                    Pin02
#define SPI3_MOSI_FUNC                   Func_Spi3_Mosi

/* SPI_MISO Port/Pin definition */
#define SPI3_MISO_PORT                   PortE
#define SPI3_MISO_PIN                    Pin03
#define SPI3_MISO_FUNC                   Func_Spi3_Miso

/* SPI unit and clock definition */
#define SPI3_UNIT                        M4_SPI3
#define SPI3_UNIT_CLOCK                  PWC_FCG1_PERIPH_SPI3
#define SPI3_TX_INT_SOURCE               INT_SPI3_SRTI
#define SPI3_RX_INT_SOURCE               INT_SPI3_SRRI
#define SPI3_ERR_INT_SOURCE              INT_SPI3_SPEI
#define SPI3_ERR_IDEL_SOURCE             INT_SPI3_SPII

#define SPI3_DMA_UNIT2                (M4_DMA2)
#define SPI3_DMA_CH                  (DmaCh0)
#define SPI3_DMA_CLK                 PWC_FCG0_PERIPH_DMA2
#define SPI3_DMA_TRNCNT              (50u)//´«Êä´ÎÊý
#define DMA_BLKSIZE             (1u)
#define SPI3_DMA_RPT_SIZE            (50u)
#define DMA_INT_SRC             INT_DMA2_BTC0
#define DMA_Trg_Src             EVT_SPI3_SRRI

void Hw_SPI3_Init(void);
void Hw_SPI3_TEST(void);
void Hw_SPI3_DMA_Init(void);
#endif
