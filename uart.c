//�����շ�����

#include "STC15F2K60S2.h"
#include "pin.h"
#include "uart.h"
#include "main.h"
				 
#define UART_BAUD 38400//38400	 //115200bps
#define ETU_WIDTH 97 //103us
#define SYS_CLK 22118400 //33.1776Mhz

 struct status data uart_status;
 struct uart_buff send_buff;
 struct uart_buff rece_buff;
 unsigned char uart_data[256];
/*********************************************/
/*��������	    timer_uart_init
/*���ܣ�	    ʱ���봮�ڳ�ʼ��
				��ʼ������0Ϊ19200BPS
				1��ֹͣλ��8������λ����У��λ
				�����ж�����
/*���������	��
/*����ֵ��	    ��
/*********************************************/
void Uart1_Init(void)
{
	SCON=0x50;				    				//SM0:0 SM1:1 SM2:0 REN:1 TB8:0 RB8:0 TI:0 RI:0
	PCON=0x00;									//SMOD=0  
	AUXR |= 0x40;		//��ʱ��1ʱ��ΪFosc,��1T
	AUXR &= 0xFE;		//����1ѡ��ʱ��1Ϊ�����ʷ�����
	TMOD &= 0x0F;		//�趨��ʱ��1Ϊ16λ�Զ���װ��ʽ

	TL1=(65536-((long)SYS_CLK/4/(long)UART_BAUD));	  
	TH1=(65536-((long)SYS_CLK/4/(long)UART_BAUD))>>8;

	ET1 = 0;		//��ֹ��ʱ��1�ж�
	TR1 = 1;		//������ʱ��1

//	INT1=1;//�����жϽ�Ϊ�ߵ�ƽ
//	IT1=1;//�������½���
//	EX1=1;//�ж�ʹ��

	ES=1;					    				//������0�ж�
	EA=1;					    				//�����ж�
}

void Debug_Send_char1(unsigned char Data)//���͵����ֽ�
{
	;
// 	ES=0;		//�ش����ж�
// 	CON1=1;
//   TI=0;
// /***/
//   if(SM0)
//   {
//     ACC=Data;
//     TB8=P;//żУ��
//   }
// /***/  
//   SBUF=Data;
//   while(!TI);
//   TI=0;
// 	CON1=0;
// 	ES=1;		//�������ж�
}
void Debug_Send_string1(unsigned char *pz,unsigned int Len)  //��pz��ʼ����numberλ
{
  if(Len==0)
    while(*pz)
    {
      if(*pz=='\n')
        Debug_Send_char1('\r');
      Debug_Send_char1(*pz++);
    }
	else
    while(Len--)
      Debug_Send_char1(*pz++);
}

void Send_char1(unsigned char Data)//���͵����ֽ�
{
	ES=0;		//�ش����ж�
	CON1=1;
  TI=0;
/***/
  if(SM0)
  {
    ACC=Data;
    TB8=P;//żУ��
  }
/***/  
  SBUF=Data;
  while(!TI);
  TI=0;
	CON1=0;
	ES=1;		//�������ж�
}
void Send_string1(unsigned char *pz,unsigned int Len)  //��pz��ʼ����numberλ
{
  if(Len==0)
    while(*pz)
    {
      if(*pz=='\n')
        Send_char1('\r');
      Send_char1(*pz++);
    }
	else
    while(Len--)
      Send_char1(*pz++);
}

void Send_char2(unsigned char Data)//���͵����ֽ�
{
	IE2 &= ~0x01;		//�ش����ж�
  S2CON &= ~S2TI;         //���S2TIλ
  S2BUF=Data;
  while(!(S2CON & S2TI));
  S2CON &= ~S2TI;         //���S2TIλ
	IE2 |= 0x01;		//�������ж�
}
void Send_string2(unsigned char *pz,unsigned int Len)  //��pz��ʼ����numberλ
{
  if(Len==0)
    while(*pz)
    {
      if(*pz=='\n')
        Send_char2('\r');
      Send_char2(*pz++);
    }
	else
    while(Len--)
      Send_char2(*pz++);
}
void Uart2_Init(void)
{
  P_SW2 &= ~S2_S0;            //S2_S0=0 (P1.0/RxD2, P1.1/TxD2)
//  P_SW2 |= S2_S0;             //S2_S0=1 (P4.6/RxD2_2, P4.7/TxD2_2)

//#if (PARITYBIT == NONE_PARITY)
  S2CON = 0x50;               //8λ�ɱ䲨����
//#elif (PARITYBIT == ODD_PARITY) || (PARITYBIT == EVEN_PARITY) || (PARITYBIT == MARK_PARITY)
//  S2CON = 0xda;               //9λ�ɱ䲨����,У��λ��ʼΪ1
//#elif (PARITYBIT == SPACE_PARITY)
//  S2CON = 0xd2;               //9λ�ɱ䲨����,У��λ��ʼΪ0
//#endif

  T2L = (65536 - (SYS_CLK/4/115200));   //���ò�������װֵ0xe8;//
  T2H = (65536 - (SYS_CLK/4/115200))>>8;//0xff;//
  AUXR |= 0x14;                //T2Ϊ1Tģʽ, ��������ʱ��2
  IE2 |= 0x01;                 //ʹ�ܴ���2�ж�
  EA = 1;
}
void Hex_Print(unsigned char uc_data)
{
	unsigned char HEX[]="0123456789ABCDEF";
	Send_char1(HEX[(uc_data>>4)&0x0f]);
	Send_char1(HEX[(uc_data>>0)&0x0f]);
}

void Uart_Send(void)
{
	int i = 0;
	
	Send_char1('\r');
	Send_char1('\n');
	for (i = 0; i < send_buff.length; i ++) {
		Hex_Print(send_buff.buff[i]);
	}
	Send_char1('\r');
	Send_char1('\n');
	
	return;
}
