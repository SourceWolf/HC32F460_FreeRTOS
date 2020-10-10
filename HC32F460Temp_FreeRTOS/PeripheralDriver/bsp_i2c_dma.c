#include "bsp_i2c_dma.h"
M4_I2C_TypeDef* pstc_int_I2Cx = NULL;
uint8_t * p_int_data = NULL;
void DMA_TX_TC_Callback(void)
{
    uint32_t u32TimeOut;
    u32TimeOut = TIMEOUT;
	do{
		I2C_GenerateStop(pstc_int_I2Cx, Enable);//����ֹͣλ
		if(0 == (u32TimeOut--)) 
			{
				return;
			}
	}while(Reset == I2C_GetStatus(pstc_int_I2Cx, I2C_SR_STOPF));//�ȴ�ֹͣ;
}
void DMA_RX_TC_Callback(void)
{
    uint32_t u32TimeOut;
    pstc_int_I2Cx->CR3_f.FACKEN = 1;
    u32TimeOut = TIMEOUT;
		while(Reset == I2C_GetStatus(pstc_int_I2Cx, I2C_SR_RFULLF))
		{
			if(0 == (u32TimeOut--))
			{
				I2C_GenerateStop(pstc_int_I2Cx, Enable);//ֹͣ�ͷ�����
				return;
			}
		}

		I2C_NackConfig(pstc_int_I2Cx, Enable);//NAK
    *p_int_data = pstc_int_I2Cx->DRR_f.DR;   
    u32TimeOut = TIMEOUT;
	do{
		I2C_GenerateStop(pstc_int_I2Cx, Enable);//����ֹͣλ
		if(0 == (u32TimeOut--)) 
			{
				return;
			}
	}while(Reset == I2C_GetStatus(pstc_int_I2Cx, I2C_SR_STOPF));//�ȴ�ֹͣ;
}
uint8_t  bsp_I2C_DMA_Init(M4_I2C_TypeDef* pstcI2Cx,uint32_t baudrate)
{
	stc_clk_freq_t freq_clk;
	stc_i2c_init_t stcI2cInit;	
	CLK_GetClockFreq(&freq_clk);	
	if(pstcI2Cx == I2C1_UNIT)
	{
		PWC_Fcg1PeriphClockCmd(I2C1_CLK,Enable);
	}
	else if(pstcI2Cx == I2C2_UNIT)
	{
		PWC_Fcg1PeriphClockCmd(I2C2_CLK,Enable);
	}
	else if(pstcI2Cx == I2C3_UNIT)
	{
		PWC_Fcg1PeriphClockCmd(I2C3_CLK,Enable);
	}
	else
	{
		return 1;
	}
    I2C_DeInit(pstcI2Cx);
    
    MEM_ZERO_STRUCT(stcI2cInit);
    stcI2cInit.enI2cMode = I2cMaster;
    stcI2cInit.u32Baudrate = baudrate;
	stcI2cInit.u32Pclk3 = freq_clk.pclk3Freq;
    I2C_Init(pstcI2Cx, &stcI2cInit);
    
    I2C_Cmd(pstcI2Cx, Enable);
    bsp_dma_init(I2C_DMA_UNIT, DMA_RX_CH, DMA_Mode_RXsrc, DMA_Mode_RXdes, DataWidth);
    bsp_dma_init(I2C_DMA_UNIT, DMA_TX_CH, DMA_Mode_TXsrc, DMA_Mode_TXdes, DataWidth);
	return 0;
}

inline uint8_t I2C_DMA_Write_data(M4_I2C_TypeDef* pstcI2Cx,uint8_t DeviceAddr,uint8_t addr, const uint8_t *data, uint16_t len)
{
	uint32_t u32TimeOut;
	uint8_t pos;
	if(Set == I2C_GetStatus(pstcI2Cx, I2C_SR_NACKDETECTF))//����I2Cǰ�������һ�ζ�дʧ�ܵ�NAK��־�������޷�д����
	{
		I2C_ClearStatus(pstcI2Cx,I2C_CLR_NACKFCLR);
	}
	if(Set == I2C_GetStatus(pstcI2Cx, I2C_SR_BUSY))
	{
		if(Set == I2C_GetStatus(pstcI2Cx, I2C_SR_ARLOF))//�ٲ�ʧ��
		{
//			I2C_ClearStatus(I2C_CH, I2C_CLR_ARLOFCLR);
			pstcI2Cx->CLR = 0xFFFF;
			I2C_GenerateStop(pstcI2Cx, Enable);//ֹͣ�ͷ�����
		}
		return I2C_BUSY;
	}
	I2C_GenerateStart(pstcI2Cx , Enable);
	u32TimeOut = TIMEOUT;
	while(Reset == I2C_GetStatus(pstcI2Cx, I2C_SR_TEMPTYF))//���ݼĴ���Ϊ��
	{
		if(0==u32TimeOut--)
		{
			if(Set == I2C_GetStatus(pstcI2Cx, I2C_SR_BUSY))
			{
				I2C_GenerateStop(pstcI2Cx, Enable);//ֹͣ�ͷ�����
			}			
			return I2C_TIMEROUT;
		}
	}
	I2C_SendData(pstcI2Cx, DeviceAddr<<1);
	u32TimeOut = TIMEOUT;
	while(Reset == I2C_GetStatus(pstcI2Cx, I2C_SR_TENDF))//���ݷ������
	{
		if(0==(u32TimeOut--))
		{
			if(Set == I2C_GetStatus(pstcI2Cx, I2C_SR_BUSY))
			{
				I2C_GenerateStop(pstcI2Cx, Enable);//ֹͣ�ͷ�����
			}			
			return I2C_TIMEROUT;
		}
	}
	u32TimeOut = TIMEOUT;
	while(Set == I2C_GetStatus(pstcI2Cx, I2C_SR_NACKDETECTF))//�ȴ�Ӧ��
	{
		if(0 == (u32TimeOut--)) 
		{
			if(Set == I2C_GetStatus(pstcI2Cx, I2C_SR_BUSY))
			{
				I2C_GenerateStop(pstcI2Cx, Enable);//ֹͣ�ͷ�����
			}			
			return I2C_BADADDR;
		}
	}
    bsp_interrupt_callback_regist(DMA_TX_INT,Int001_IRQn,(void *)DMA_TX_TC_Callback);
    pstc_int_I2Cx = pstcI2Cx;
    bsp_dma_SetDesAddr(I2C_DMA_UNIT, DMA_TX_CH, (uint32_t)&(pstcI2Cx->DTR));
    bsp_dma_SetSrcAddr(I2C_DMA_UNIT, DMA_TX_CH, (uint32_t)data);
    bsp_dma_set_count(I2C_DMA_UNIT,DMA_TX_CH,len+2);//��֪��ΪʲôҪ+2���ܷ�����������
    bsp_dma_ch_enable(I2C_DMA_UNIT,DMA_TX_CH,Enable);
    bsp_dma_set_TrigSrc(I2C_DMA_UNIT,DMA_TX_CH,EVT_I2C1_TXI);//��ʵ��I2C1,���������ٲ���    
	I2C_SendData(pstcI2Cx, addr);
	u32TimeOut = TIMEOUT;
	while(Reset == I2C_GetStatus(pstcI2Cx, I2C_SR_TENDF))//���ݷ������
	{
		if(0==(u32TimeOut--))
		{
			if(Set == I2C_GetStatus(pstcI2Cx, I2C_SR_BUSY))
			{
				I2C_GenerateStop(pstcI2Cx, Enable);//ֹͣ�ͷ�����
			}			
			return I2C_TIMEROUT;
		}
	}
	u32TimeOut = TIMEOUT;
	while(Set == I2C_GetStatus(pstcI2Cx, I2C_SR_NACKDETECTF))//�ȴ�Ӧ��
	{
		if(0 == (u32TimeOut--)) 
		{
			if(Set == I2C_GetStatus(pstcI2Cx, I2C_SR_BUSY))
			{
				I2C_GenerateStop(pstcI2Cx, Enable);//ֹͣ�ͷ�����
			}			
			return I2C_TIMEROUT;
		}
	}
	return I2C_RET_OK;
}
inline uint8_t I2C_DMA_Read_data(M4_I2C_TypeDef* pstcI2Cx,uint8_t DeviceAddr,uint8_t addr, uint8_t *data, uint16_t len)
{
	uint32_t u32TimeOut;
	uint8_t pos;	
	pstcI2Cx->CR3_f.FACKEN = 0;//�Զ�дACK
	if(Set == I2C_GetStatus(pstcI2Cx, I2C_SR_NACKDETECTF))//����I2Cǰ�������һ�ζ�дʧ�ܵ�NAK��־�������޷�д����
	{
		I2C_ClearStatus(pstcI2Cx,I2C_CLR_NACKFCLR);
	}
	if(Set == I2C_GetStatus(pstcI2Cx, I2C_SR_BUSY))
	{
		return I2C_BUSY;
	}
	I2C_GenerateStart(pstcI2Cx , Enable);
	u32TimeOut = TIMEOUT;
	while(Reset == I2C_GetStatus(pstcI2Cx, I2C_SR_TEMPTYF))//���ݼĴ���Ϊ��
	{
		if(0==u32TimeOut--)
		{
			if(Set == I2C_GetStatus(pstcI2Cx, I2C_SR_BUSY))
			{
				I2C_GenerateStop(pstcI2Cx, Enable);//ֹͣ�ͷ�����
			}			
			return I2C_TIMEROUT;
		}
	}
	I2C_SendData(pstcI2Cx, DeviceAddr<<1);
	u32TimeOut = TIMEOUT;
	while(Reset == I2C_GetStatus(pstcI2Cx, I2C_SR_TENDF))//���ݷ������
	{
		if(0==(u32TimeOut--))
		{
			if(Set == I2C_GetStatus(pstcI2Cx, I2C_SR_BUSY))
			{
				I2C_GenerateStop(pstcI2Cx, Enable);//ֹͣ�ͷ�����
			}			
			return I2C_TIMEROUT;
		}
	}
	u32TimeOut = TIMEOUT;
	while(Set == I2C_GetStatus(pstcI2Cx, I2C_SR_NACKDETECTF))//�ȴ�Ӧ��
	{
		if(0 == (u32TimeOut--)) 
		{
			if(Set == I2C_GetStatus(pstcI2Cx, I2C_SR_BUSY))
			{
				I2C_GenerateStop(pstcI2Cx, Enable);//ֹͣ�ͷ�����
			}			
			return I2C_BADADDR;
		}
	}
	I2C_SendData(pstcI2Cx, addr);
	u32TimeOut = TIMEOUT;
	while(Reset == I2C_GetStatus(pstcI2Cx, I2C_SR_TENDF))//���ݷ������
	{
		if(0==(u32TimeOut--))
		{
			if(Set == I2C_GetStatus(pstcI2Cx, I2C_SR_BUSY))
			{
				I2C_GenerateStop(pstcI2Cx, Enable);//ֹͣ�ͷ�����
			}			
			return I2C_TIMEROUT;
		}
	}
	u32TimeOut = TIMEOUT;
	while(Set == I2C_GetStatus(pstcI2Cx, I2C_SR_NACKDETECTF))//�ȴ�Ӧ��
	{
		if(0 == (u32TimeOut--)) 
		{
			if(Set == I2C_GetStatus(pstcI2Cx, I2C_SR_BUSY))
			{
				I2C_GenerateStop(pstcI2Cx, Enable);//ֹͣ�ͷ�����
			}			
			return I2C_TIMEROUT;
		}
	}
	I2C_ClearStatus(pstcI2Cx, I2C_CLR_STARTFCLR);
	I2C_GenerateReStart(pstcI2Cx , Enable);//����Restart;
	u32TimeOut = TIMEOUT;
	while((Reset == I2C_GetStatus(pstcI2Cx, I2C_SR_BUSY)) ||
            (Reset == I2C_GetStatus(pstcI2Cx, I2C_SR_STARTF)))
    {
        if(0 == (u32TimeOut--)) 
		{
			I2C_GenerateStop(pstcI2Cx, Enable);//ֹͣ�ͷ�����
			return I2C_RET_ERROR;
		}
    }
	u32TimeOut = TIMEOUT;
	while(Reset == I2C_GetStatus(pstcI2Cx, I2C_SR_TEMPTYF))//���ݼĴ���Ϊ��
	{
		if(0==u32TimeOut--)
		{
			if(Set == I2C_GetStatus(pstcI2Cx, I2C_SR_BUSY))
			{
				I2C_GenerateStop(pstcI2Cx, Enable);//ֹͣ�ͷ�����
			}			
			return I2C_TIMEROUT;
		}
	}
    bsp_interrupt_callback_regist(DMA_RX_INT,Int002_IRQn,(void *)DMA_RX_TC_Callback);
    pstc_int_I2Cx = pstcI2Cx;
    p_int_data = (uint8_t*)data+len-1;
    bsp_dma_SetDesAddr(I2C_DMA_UNIT, DMA_RX_CH, (uint32_t)data);
    bsp_dma_SetSrcAddr(I2C_DMA_UNIT, DMA_RX_CH, (uint32_t)&(pstcI2Cx->DRR));
    bsp_dma_set_count(I2C_DMA_UNIT,DMA_RX_CH,len-1);
    bsp_dma_ch_enable(I2C_DMA_UNIT,DMA_RX_CH,Enable);
    bsp_dma_set_TrigSrc(I2C_DMA_UNIT,DMA_RX_CH,EVT_I2C1_RXI);//��ʵ��I2C1,���������ٲ���  
	I2C_SendData(pstcI2Cx, (DeviceAddr<<1)|0x01);
    u32TimeOut = TIMEOUT;
	while(Set == I2C_GetStatus(pstcI2Cx, I2C_SR_NACKDETECTF))//�ȴ�Ӧ��
	{
		if(0 == (u32TimeOut--)) 
		{
			if(Set == I2C_GetStatus(pstcI2Cx, I2C_SR_BUSY))
			{
				I2C_GenerateStop(pstcI2Cx, Enable);//ֹͣ�ͷ�����
			}			
			return I2C_TIMEROUT;
		}
	}
    
//	for(pos = 0;pos<len;pos++)
//	{
//		u32TimeOut = TIMEOUT;
//		while(Reset == I2C_GetStatus(pstcI2Cx, I2C_SR_RFULLF))
//		{
//			if(0 == (u32TimeOut--))
//			{
//				I2C_GenerateStop(pstcI2Cx, Enable);//ֹͣ�ͷ�����
//				return I2C_TIMEROUT;
//			}
//		} 
//#ifndef AUTOACK	
//		if(pos == (len-1))
//		{
//			I2C_NackConfig(pstcI2Cx, Enable);
//		}
//        else
//		{
//			I2C_NackConfig(pstcI2Cx, Disable);//ACK
//		}
//#endif        
//		data[pos] = pstcI2Cx->DRR_f.DR;//I2C_ReadData(I2C1_UNIT);
//#ifdef AUTOACK	
//		if(pos == (len-1))
//		{
//			I2C_NackConfig(pstcI2Cx, Enable);//NAK
//		}
//		else
//		{
//			I2C_NackConfig(pstcI2Cx, Disable);//ACK
//		}
//#endif		
//	}
//	u32TimeOut = TIMEOUT;
//	do{
//		I2C_GenerateStop(pstcI2Cx, Enable);//����ֹͣλ
//		if(0 == (u32TimeOut--)) 
//			{
//				return I2C_TIMEROUT;
//			}
//	}while(Reset == I2C_GetStatus(pstcI2Cx, I2C_SR_STOPF));//�ȴ�ֹͣ
	return I2C_RET_OK;
}