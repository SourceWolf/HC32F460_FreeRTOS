#ifndef HW_I2C_SLAVE_DMA_H
#define HW_I2C_SLAVE_DMA_H
#include "hc32_ddl.h"
#include "bsp_i2c_dma.h"

#define I2C_SLAVE_SCL_PORT   PortC//PortA//PortC
#define I2C_SLAVE_SCL_Pin    Pin04//Pin01//Pin04
#define I2C_SLAVE_SDA_PORT   PortC//PortA//PortC
#define I2C_SLAVE_SDA_Pin    Pin05//Pin00//Pin05
#define I2C_SLAVE_SCL_FUNC		Func_I2c1_Scl
#define I2C_SLAVE_SDA_FUNC		Func_I2c1_Sda

#define DMA_SLRX_CH       (DmaCh2)
#define DMA_SLTX_CH       (DmaCh3)
#define DMA_SLRX_CH_TRIG  EVT_I2C1_RXI
#define DMA_SLTX_CH_TRIG  EVT_I2C1_TXI
#define DMA_SLTX_INT      INT_DMA1_TC3
#define DMA_SLRX_INT      INT_DMA1_TC2
#define DMA_Mode_SLRXsrc  AddressFix
#define DMA_Mode_SLRXdes  AddressIncrease
#define DMA_Mode_SLTXsrc  AddressIncrease
#define DMA_Mode_SLTXdes  AddressFix
#define DataWidth       Dma8Bit

#define DMA_CH2_IRQn       Int024_IRQn
#define DMA_CH3_IRQn       Int025_IRQn

#define TEST_DATA_LEN                   256u
#define SLAVE_ADDRESS                   0x06u

#define GENERATE_START                  0x00u
#define GENERATE_RESTART                0x01u

#define ADDRESS_W                       0x00u
#define ADDRESS_R                       0x01u



uint8_t Hw_I2C_Slave_Init(M4_I2C_TypeDef* I2Cx);

#endif
