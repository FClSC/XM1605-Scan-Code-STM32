#include <stdio.h>
#include <stdlib.h>
#include "stdarg.h"    
#include "string.h" 
#include "uart.h"


uint8_t Serial_Flag5=0;//��־λ
char UART5_RX_BUF[100];//ɨ���ô洢��


//���UART5_RX_BUF��0-6λ������Ҫ��  ��  123+321
//������    ɨ��ģ��   ����USART5 
//��Ƭ��   PC12 TX    PD2 RX   

void UART5_Init(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5, ENABLE);//����UART5��APB1ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
	//PC12   TX 
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	//PD2   RX
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.USART_BaudRate = 9600;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_Init(UART5, &USART_InitStructure);
	
	USART_ITConfig(UART5, USART_IT_RXNE, ENABLE);
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init(&NVIC_InitStructure);


	
	USART_Cmd(UART5, ENABLE);
}

/********************************
�������� : ����5 ����һ���ֽ�
������� : һ���ֽ�
������� ����
*********************************/
void USART5_SendBits(uint8_t data)
{
	UART5->DR = data;
	while(USART_GetFlagStatus(UART5,USART_FLAG_TXE) == RESET);
}
/********************************
�������� : ����5 ����һ������
������� : һ������
������� ����
*********************************/
void UART5_SendArray(uint8_t *array,uint8_t length)
{
	uint8_t i;
	for(i=0;i<length;i++)
	{
		USART5_SendBits(array[i]);
	}
}
/********************************
�������� : ����5 ����һ���ַ���
������� : һ���ַ���
������� ����
*********************************/
void UART5_SendString(uint8_t *String1)
{
	uint8_t i;
	for(i=0;String1[i]!='\0';i++)
	{
		USART5_SendBits(String1[i]);		
	}
}


//����5��ñ�־λ
uint8_t Serial5_GetRxFlag(void)
{
	if(Serial_Flag5 == 1)
	{
		Serial_Flag5=0;
		return 1;
	}
	return 0;
}
// @-------#$
void UART5_IRQHandler(void)
{
	static uint8_t RxState = 0;
	static uint8_t pRxPacket = 0;	
    if (USART_GetITStatus(UART5, USART_IT_RXNE) == SET)
	{
        uint8_t RxData = USART_ReceiveData(UART5);
        if(RxState==0)
		{
			if(RxData=='@'&&Serial_Flag5==0)
			{
				RxState=1;
				pRxPacket=0;		
			}			
		}
		else if (RxState==1)
		{
			if(RxData=='#')
			{
				RxState=2;
			}
			else
			{
				UART5_RX_BUF[pRxPacket]=RxData;
				pRxPacket++;
			}
		}
		else if(RxState==2)
		{
			if(RxData=='$')
			{
				RxState=0;
				UART5_RX_BUF[pRxPacket]='\0'; //�洢��Ϻ�Ϊ�ַ�����ʽ�ڻ�����
				Serial_Flag5=1;				
			}
			
			
		}
        USART_ClearITPendingBit(UART5, USART_IT_RXNE);
    }
}


void UART5_Start_Scan(void)//����ɨ��ָ��
{

   uint8_t Order[9]={0x7E,0x00,0x08,0x01,0x00,0x02,0x01,0xAB,0xCD};
   UART5_SendArray(Order,9);

}


void UART5_ParseCode(const char *Buf, int16_t *code1, int16_t *code2)
{
    // ֱ�ӽ�����һ����λ��
    *code1 = (Buf[0] - '0') * 100 + 
            (Buf[1] - '0') * 10 + 
            (Buf[2] - '0');

    // ���� '+' �Ž����ڶ�����λ��
    *code2 = (Buf[4] - '0') * 100 + 
            (Buf[5] - '0') * 10 + 
            (Buf[6] - '0');
	
}
