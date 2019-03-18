

#include "main.h"
#include "intrins.h"
#include "24Cxx.h"

  /****IIC*******/
#define SDA_IO_1()      		_24_SDA=1
#define SDA_IO_0()      		_24_SDA=0
#define SDA_IO_OUT()				_24_SDA=1

#define Read_GPIO_SDA_IO()	_24_SDA
#define SDA_IO_IN()					_24_SDA=1

#define SCL_IO_1()      		_24_SCL=1
#define SCL_IO_0()      		_24_SCL=0

#define _24WP_IO_1()      	_24_WP=1
#define _24WP_IO_0()      	_24_WP=0

unsigned int type_I2C=512;
unsigned int add_end_I2C=0xffff;
unsigned int PageLen_I2C=128;

#define I2Cmode 0  //0,Ϊģ��I2C ; 1,ΪӲ��I2C

#if I2Cmode ==0

uint8 Ack_Err;//����Ӧ���ۼ���������λû�������Stop()�������λ������ͱ����������I2C����
uint8 I2C_Delay_EN=0;//=0;
void delayus(void)
{
	_nop_();
	_nop_();
}
void delay4us(void)//'b'us
{
  if(I2C_Delay_EN)
  {
		delayus();
		delayus();
		delayus();
		delayus();
  }
}
void i2cStart()//��ʼ���� ʱ���߱��ָߵ�ƽ�ڼ������ߵ�ƽ�Ӹߵ��͵�������ΪI2C ���ߵ���ʼ�ź�
{
	if(Ack_Err<0xFF)
	  Ack_Err++; 
	SDA_IO_1();delay4us();//_nop_ ();_nop_ ();_nop_ ();_nop_ (); 
	SCL_IO_1();delay4us();//_nop_ ();_nop_ ();_nop_ ();_nop_ (); 
	SDA_IO_0();delay4us();//_nop_ ();_nop_ ();_nop_ ();_nop_ (); 
	SCL_IO_0();delay4us();//_nop_ ();_nop_ ();_nop_ ();_nop_ (); 
}
void i2cStop()//��������	 ʱ���߱��ָߵ�ƽ�ڼ������ߵ�ƽ�ӵ͵��ߵ�������ΪI2C ���ߵ�ֹͣ�ź�
{
	if(Ack_Err<0xFF)
    Ack_Err=0x00; 
	SDA_IO_0();delay4us();//_nop_ ();_nop_ ();_nop_ ();_nop_ ();
	SCL_IO_1();delay4us();//_nop_ ();_nop_ ();_nop_ ();_nop_ (); 
	SDA_IO_1();delay4us();//_nop_ ();_nop_ ();_nop_ ();_nop_ (); 
	SCL_IO_0();delay4us();//_nop_ ();_nop_ ();_nop_ ();_nop_ (); 
}
void Ack()//����Ӧ��
{
	SDA_IO_0();delay4us();//_nop_ ();_nop_ ();_nop_ ();_nop_ ();
	SCL_IO_1();delay4us();//_nop_ ();_nop_ ();_nop_ ();_nop_ (); 
	SCL_IO_0();delay4us();//_nop_ ();_nop_ ();_nop_ ();_nop_ ();
}
void NoAck()//��Ӧ��
{
	SDA_IO_1();delay4us();//_nop_ ();_nop_ ();_nop_ ();_nop_ (); 
	SCL_IO_1();delay4us();//_nop_ ();_nop_ ();_nop_ ();_nop_ (); 
	SCL_IO_0();delay4us();//_nop_ ();_nop_ ();_nop_ ();_nop_ (); 
}
uint8 TestAck()//�ӻ�Ӧ��
{
	uint8 ErrorBit;
	SDA_IO_1();delay4us();//_nop_ ();_nop_ ();_nop_ ();_nop_ (); 
	SCL_IO_1();delay4us();//_nop_ ();_nop_ ();_nop_ ();_nop_ ();
	SDA_IO_IN();delay4us();//SDA_IO_INIT_IN();
	if(Read_GPIO_SDA_IO())
		ErrorBit=1;//I2C_ACK;
	else
		ErrorBit=0;//I2C_ACK;
	delay4us();//_nop_ ();_nop_ ();_nop_ ();_nop_ ();
	SDA_IO_OUT();//SDA_IO_INIT();
	SCL_IO_0();delay4us();//_nop_ ();_nop_ ();_nop_ ();_nop_ ();
	if(Ack_Err==0xFF)
		return(0);
	else
		return(ErrorBit);//0);//
}
void writeByte(uint8 Data)//д8λ
{
	uint8 temp;
	for(temp=8;temp!=0;temp--)
	{
		if(Data&0x80)
			SDA_IO_1();//SDA_pin=(bit)(input&0x80);
		else
			SDA_IO_0();
		delay4us();//_nop_ ();_nop_ ();_nop_ ();_nop_ (); 
		SCL_IO_1();delay4us();//_nop_ ();_nop_ ();_nop_ ();_nop_ ();
		Data<<=1;
		SCL_IO_0();delay4us();//_nop_ ();_nop_ ();_nop_ ();_nop_ ();		 
	}
}
unsigned char readByte()//��8λ
{
	uint8 temp,rbyte=0; 
	for(temp=8;temp!=0;temp--)  
	{ 
		SDA_IO_1();delay4us();//_nop_ ();_nop_ ();_nop_ ();_nop_ ();
		rbyte<<=1; 
		SCL_IO_1();delay4us();//_nop_ ();_nop_ ();_nop_ ();_nop_ ();
		SDA_IO_IN();delay4us();//SDA_IO_INIT_IN();
		if(Read_GPIO_SDA_IO())//I2C_ACK;
			rbyte|=1;
		delay4us();//_nop_ ();_nop_ ();_nop_ ();_nop_ ();
		SDA_IO_OUT();//SDA_IO_INIT();
		//rbyte=rbyte|((uchar)(I2C_ACK));delay4us();//_nop_ ();_nop_ ();_nop_ ();_nop_ (); 
		SCL_IO_0();delay4us();//_nop_ ();_nop_ ();_nop_ ();_nop_ (); 
	}
	return(rbyte); 
}
uint8 readMem(uint16 page_start ,uint8 c_addr)
{
  uint8 Data;
  Ack_Err=0x00;
	do
	{
		i2cStart();
		writeByte(c_addr);
//		writeByte(((c_Address<<1)&0x0E)|ReadDeviceAdd);//��P2,P1,P0��������ֺϲ�
	}
	while(TestAck());//�ȴ�Ӧ��
	if(Ack_Err==0xFF)
  {
    i2cStop();
    return 0x77;
  }
  if(c_addr!=I2C_2405)
	{
		writeByte((page_start>>8)&0xff);//д��8λ��ַ
		TestAck();
	}
	writeByte(page_start&0xff);//д��8λ��ַ
	TestAck();
	i2cStart();
//	writeByte(((c_Address<<1)&0x0E)|ReadDeviceAdd);//��P2,P1,P0��������ֺϲ�
	writeByte(c_addr+1);
	TestAck();
	Data=readByte();
	NoAck();//������Ӧ��
	i2cStop();//����ֹͣ
  return(Data);
}
void read_mul(uint16 page_start, uint8 c_addr, uint8 *message_dat,uint16 Len)
{
  Ack_Err=0x00;
	if(Len==0)return;
	do
	{
		i2cStart();
		writeByte(c_addr);
//		writeByte(((c_Address<<1)&0x0E)|ReadDeviceAdd);//��P2,P1,P0��������ֺϲ�
	}
	while(TestAck());//�ȴ�Ӧ��
	if(Ack_Err==0xFF)
  {
    i2cStop();
    return;
  }
  if(c_addr!=I2C_2405)
	{
		writeByte((page_start>>8)&0xff);//д��8λ��ַ
		TestAck();
	}
	writeByte(page_start&0xff);//д��8λ��ַ
	TestAck();
	i2cStart();
//	writeByte(((c_Address<<1)&0x0E)|ReadDeviceAdd);//��P2,P1,P0��������ֺϲ�
	writeByte(c_addr+1);
	TestAck();
	while(Len!=1)
	{
		*message_dat=readByte();
		Ack();//����Ӧ��
		message_dat++;
		Len--;
	}
	*message_dat=readByte();
	NoAck();//������Ӧ��
	i2cStop();//����ֹͣ
}
void writeMem(uint16 page_start, uint8 c_addr ,uint8 message_in)
{
  if(readMem(page_start,c_addr)==message_in)return;
  
  _24WP_IO_0();
	
  Ack_Err=0x00;
	do
	{
		i2cStart();
		writeByte(c_addr);
//		writeByte(((c_Address<<1)&0x0E)|ReadDeviceAdd);//��P2,P1,P0��������ֺϲ�
	}
	while(TestAck());//�ȴ�Ӧ��      
	if(Ack_Err==0xFF)
  {
    i2cStop();
    return;
  }
  if(c_addr!=I2C_2405)
	{
		writeByte((page_start>>8)&0xff);//д��8λ��ַ
		TestAck();
	}
  writeByte(page_start&0xff);//����д����ʼ��ַ
	TestAck();
  writeByte(message_in);
	TestAck();
  i2cStop();//����ֹͣλ
	
  Ack_Err=0x00;
	do//
	{
		i2cStart();
		writeByte(c_addr);
//		writeByte(((c_Address<<1)&0x0E)|ReadDeviceAdd);//��P2,P1,P0��������ֺϲ�
	}
	while(TestAck());//�ȴ�Ӧ��
	i2cStop();
	
  _24WP_IO_1();
}
void write_mul(uint16 page_start, uint8 c_addr ,uint8 *message_dat,uint8 Len)  //ҳд,�����
{
  uint8 i;
	if(Len==0)return;
  for(i=0;i<Len;)
    if(readMem(page_start+i,c_addr)==message_dat[i])i++;
    else break;
  if(i==Len)return;
  
  _24WP_IO_0();
	
  Ack_Err=0x00;
	do
	{
		i2cStart();
		writeByte(c_addr);
//		writeByte(((c_Address<<1)&0x0E)|ReadDeviceAdd);//��P2,P1,P0��������ֺϲ�
	}
	while(TestAck());//�ȴ�Ӧ��      
	if(Ack_Err==0xFF)
  {
    i2cStop();
    return;
  }
  if(c_addr!=I2C_2405)
	{
		writeByte((page_start>>8)&0xff);//д��8λ��ַ
		TestAck();
	}
  writeByte(page_start&0xff);//����д����ʼ��ַ
	TestAck();
	while(Len--)
	{
		writeByte(*message_dat++);
		TestAck();
	}
  i2cStop();//����ֹͣλ
	
  Ack_Err=0x00;
	do//
	{
		i2cStart();
		writeByte(c_addr);
//		writeByte(((c_Address<<1)&0x0E)|ReadDeviceAdd);//��P2,P1,P0��������ֺϲ�
	}
	while(TestAck());//�ȴ�Ӧ��
	i2cStop();
	
  _24WP_IO_1();
}
#endif

#if I2Cmode ==1
                                                                        /* �������ں�I2C�жϴ�����Ϣ��  */
                                                                        /* ȫ�ֱ���                     */
volatile uint8_t     I2C_sla;                                           /* I2C�����ӵ�ַ                */
volatile uint32_t    I2C_suba;                                          /* I2C�����ڲ��ӵ�ַ            */
volatile uint8_t     I2C_suba_num;                                      /* I2C�ӵ�ַ�ֽ���              */
volatile uint8_t     *I2C_buf;                                          /* ���ݻ�����ָ��               */
volatile uint32_t    I2C_num;                                           /* Ҫ��ȡ/д������ݸ���        */
volatile uint8_t     I2C_end;                                           /* I2C���߽�����־������������  */
                                                                        /* ��1                          */
volatile uint8_t     I2C_suba_en;                                       /*  �ӵ�ַ���ơ�
                                                                         0--�ӵ�ַ�Ѿ�������߲���Ҫ�ӵ�ַ
                                                                         1--��ȡ����
                                                                         2--д����
                                                                         */

/*********************************************************************************************************
** Function name:       I2C2_IRQHandler
** Descriptions:        Ӳ��I2C�жϷ������
** input parameters:    ��
** Output parameters:   ��
** Returned value:      ע�⴦���ӵ�ַΪ2�ֽڵ������
*********************************************************************************************************/
//void  myI2C2IRQHandler(void)
void  I2C2_IRQHandler(void)
{
	/*
         *��ȡI2C״̬�Ĵ���I2DAT,����ȫ�ֱ��������ý��в��������������־,����ж��߼�,�жϷ���
         */
	switch (LPC_I2C2->I2STAT & 0xF8)
	{   /* 
	     *  ����״̬�������Ӧ�Ĵ���     
	     */
		case 0x08:/* �ѷ�����ʼ����,�����ͺ�����  */
		{
																									/* �ն���,װ��SLA+W����SLA+R    */
			if(I2C_suba_en == 1)/* SLA+R */							/* ָ���ӵ�ַ��                 */
				LPC_I2C2->I2DAT = I2C_sla & 0xFE;           /* ��д���ַ                   */
			else                                        /* SLA+W                        */
				LPC_I2C2->I2DAT = I2C_sla;                  /* ����ֱ�ӷ��ʹӻ���ַ         */
																									/* ����SIλ                     */
			LPC_I2C2->I2CONCLR = (1 << 3)|                /* SI                           */
												 (1 << 5);                /* STA                          */
		}
    break;
		case 0x10:/*�ѷ����ظ���ʼ����            */
		{
                                                  /* �����ͺ������ն���           */
                                                  /* װ��SLA+W����SLA+R           */
			LPC_I2C2->I2DAT = I2C_sla;                  	/* �������ߺ��ط��ӵ�ַ       */
			LPC_I2C2->I2CONCLR = 0x28;                  	/* ����SI,STA                   */
		}
		break;
		case 0x18:
		case 0x28:/* �ѷ���I2DAT�е�����,�ѽ���ACK*/
		{
			if (I2C_suba_en == 0)
			{
				if (I2C_num > 0)
				{
					LPC_I2C2->I2DAT = *I2C_buf++;
					LPC_I2C2->I2CONCLR = 0x28;                                /* ����SI,STA                   */
					I2C_num--;
					delay_ms(1);
				}
				else                                                        /* û�����ݷ�����               */
				{                                                           /* ֹͣ����                     */
					LPC_I2C2->I2CONSET = (1 << 4);                            /* STO                          */
					LPC_I2C2->I2CONCLR = 0x28;                                /* ����SI,STA                   */
					I2C_end = 1;                                            /* �����Ѿ�ֹͣ                 */
				}
			}
			if(I2C_suba_en == 1)
			{                                          /* ����ָ����ַ��,��������������*/
				if (I2C_suba_num == 2)
				{
					LPC_I2C2->I2DAT = ((I2C_suba >> 8) & 0xff);
					LPC_I2C2->I2CONCLR = 0x28;                                /* ����SI,STA                   */
					I2C_suba_num--;
					break;
				}
				if(I2C_suba_num == 1)
				{
					LPC_I2C2->I2DAT = (I2C_suba & 0xff);
					LPC_I2C2->I2CONCLR = 0x28;                                /* ����SI,STA                   */
					I2C_suba_num--;
					break;
				}
				if (I2C_suba_num == 0)
				{
					LPC_I2C2->I2CONCLR = 0x08;
					LPC_I2C2->I2CONSET = 0x20;
					I2C_suba_en = 0;                                        /* �ӵ�ַ������                 */
					break;
				}
			}
			if (I2C_suba_en == 2)
			{                                         /* ָ���ӵ�ַд,�ӵ�ַ��δָ��, */
                                                                        /* �����ӵ�ַ                 */
				if (I2C_suba_num > 0)
				{
					if (I2C_suba_num == 2)
					{
						LPC_I2C2->I2DAT = ((I2C_suba >> 8) & 0xff);
						LPC_I2C2->I2CONCLR = 0x28;
						I2C_suba_num--;
						break;
					}
					if (I2C_suba_num == 1)
					{
						LPC_I2C2->I2DAT    = (I2C_suba & 0xff);
						LPC_I2C2->I2CONCLR = 0x28;
						I2C_suba_num--;
						I2C_suba_en  = 0;
						break;
					}
				}
			}
		}
		break;
		case 0x40: 
		{			/* �ѷ���SLA+R,�ѽ���ACK        */
			if (I2C_num <= 1)
			{                                             /* ��������һ���ֽ�           */
				 LPC_I2C2->I2CONCLR = 1 << 2;                                 /* �´η��ͷ�Ӧ���ź�           */
			}
			else
			{
				 LPC_I2C2->I2CONSET = 1 << 2;                                 /* �´η���Ӧ���ź�             */
			}
			LPC_I2C2->I2CONCLR = 0x28;                                        /* ����SI,STA                   */
		}
		break;
		case 0x20:                                                      /* �ѷ���SLA+W,�ѽ��շ�Ӧ��     */
		case 0x30:                                                      /* �ѷ���I2DAT�е����ݣ��ѽ���  */
                                                                        /* ��Ӧ��                       */
		case 0x38:                                                      /* ��SLA+R/W�������ֽ��ж�ʧ��  */
                                                                        /* ��                           */
		case 0x48:                                                      /* �ѷ���SLA+R,�ѽ��շ�Ӧ��     */
		{
			LPC_I2C2->I2CONCLR = 0x28;
			I2C_end = 0xFF;
		}
		break;
		case 0x50:                                                      /* �ѽ��������ֽڣ��ѷ���ACK    */
    {
			*I2C_buf++ = LPC_I2C2->I2DAT;
			I2C_num--;
			if (I2C_num == 1)                                               /* �������һ���ֽ�             */
			{   
				LPC_I2C2->I2CONCLR = 0x2C;                                    /* STA,SI,AA = 0                */
			} 
			else
			{   
				LPC_I2C2->I2CONSET = 0x04;                                    /* AA=1                         */
				LPC_I2C2->I2CONCLR = 0x28;
			}
		}
		break;
		case 0x58:                                                      /* �ѽ��������ֽڣ��ѷ��ط�Ӧ�� */
    {
			*I2C_buf++ = LPC_I2C2->I2DAT;                                     /* ��ȡ���һ�ֽ�����           */
			LPC_I2C2->I2CONSET = 0x10;                                        /* ��������                     */
			LPC_I2C2->I2CONCLR = 0x28;
			I2C_end = 1;
		}
		break;
		default:
		break;
	}
}

void I2C2Init(uint32_t ulFi2c2)
{
	LPC_SC->PCONP |= (1UL << 26);                                      /* ��I2C2ģ�鹦�ʿ���λ       */
//	LPC_IOCON->P0_27 = 0x01 | (0 << 6)| (1 << 8) | (1 << 9);           /* P0.27����ΪSDA0����          */
//	LPC_IOCON->P0_28 = 0x01 | (0 << 6)| (1 << 8) | (1 << 9);           /* P0.28����ΪSCL0����          */
	
	LPC_PINCON->PINSEL0 &= ~(3UL << ((SDA_IO%16) * 2));
	LPC_PINCON->PINSEL0 |= (2UL << ((SDA_IO%16) * 2));
//	LPC_PINCON->PINMODE0 |= (2UL << ((SDA_IO%16) * 2));
//	LPC_PINCON->PINMODE_OD0 |= (1UL << SDA_IO);
	
	LPC_PINCON->PINSEL0 &= ~(3UL << ((SCL_IO%16) * 2));
	LPC_PINCON->PINSEL0 |= (2UL << ((SCL_IO%16) * 2));
//	LPC_PINCON->PINMODE0 |= (2UL << ((SCL_IO%16) * 2));
//	LPC_PINCON->PINMODE_OD0 |= (1UL << SCL_IO);
	
//	LPC_SC->PCLKSEL1	|= ((1<<20)|(1<<21));
	
	if (ulFi2c2 > 400000) {
			ulFi2c2 = 400000;
	}
	LPC_I2C2->I2SCLH = (((SystemFrequency / ulFi2c2) / 4)+1)/2;                   /* ����I2Cʱ��Ϊfi0c            */
	LPC_I2C2->I2SCLL = (SystemFrequency / ulFi2c2) / 4/2;
	NVIC_EnableIRQ(I2C2_IRQn);                                          /* ʹ��I2C�ж�                  */
	NVIC_SetPriority(I2C2_IRQn, 3);                                     /* ����I2C2�ж����ȼ���ʹ���ж� */
	LPC_I2C2->I2CONCLR = 0x2C;
	LPC_I2C2->I2CONSET = 0x40;                                            /* ʹ����I2C                    */
}

/*********************************************************************************************************
** Function name:           Wait_I2c_End
** Descriptions:            �����ʱ���ṩ��I2C���ߵȴ���ʱʹ��
** input parameters:        dly        ��ʱ������ֵԽ����ʱԽ��
** Output parameters:       NONE
** Returned value:          NONE
*********************************************************************************************************/
uint8_t  Wait_I2c_End(uint32_t  dly)
{
	uint32_t  i;
	if( I2C_end==1 ) return (1);
	for(; dly>0; dly--)
		for(i=0; i<5000; i++)
			if( I2C_end==1 )
				return (1);
	return (0);
}
#if 0
/*********************************************************************************************************
** Function name:       ISendByte
** Descriptions:        �����ӵ�ַ��������1�ֽ����ݡ�
** input parameters:    sla     ������ַ
**                      dat     Ҫ���͵�����
** Output parameters:   ����ֵΪ0ʱ��ʾ����Ϊ1ʱ��ʾ������ȷ��
** Returned value:      NONE
*********************************************************************************************************/
uint8_t  ISendByte(uint8_t sla, uint8_t dat)
{                                                                       /* ��������                     */
	I2C_sla     = sla;                                                  /* д������������ַ             */
	I2C_buf     = &dat;                                                 /* �����͵�����                 */
	I2C_num     = 1;                                                    /* ����1�ֽ�����                */
	I2C_suba_en = 0;                                                    /* ���ӵ�ַ                     */
	I2C_end     = 0;
	LPC_I2C2->CONCLR = 0x2C;
	LPC_I2C2->CONSET = 0x60;                                            /* ����Ϊ����������������       */
	return( Wait_I2c_End(20));
}
/*********************************************************************************************************
** Function name:       IRcvByte
** Descriptions:        �����ӵ�ַ������ȡ1�ֽ����ݡ�
** input parameters:    sla     ������ַ
**                      dat     Ҫ���͵�����
** Output parameters:   ����ֵΪ0ʱ��ʾ����Ϊ1ʱ��ʾ������ȷ��
** Returned value:      NONE
*********************************************************************************************************/
uint8_t  IRcvByte(uint8_t sla, uint8_t *dat)
{                                                                       /* ��������                     */
	I2C_sla     = sla+1;                                                /* ��������������ַ             */
	I2C_buf     = dat;
	I2C_num     = 1;
	I2C_suba_en = 0;                                                    /* ���ӵ�ַ                     */
	I2C_end     = 0;
	LPC_I2C2->CONCLR = 0x2C;
	LPC_I2C2->CONSET = 0x60;                                            /* ����Ϊ����������������       */
	return( Wait_I2c_End(20));
}
#endif
/*********************************************************************************************************
** Function name:       readMem
** Descriptions:        �����ӵ�ַ���������ַ��ʼ��ȡN�ֽ�����
** input parameters:    sla         ������ַ
**                      suba_type   �ӵ�ַ�ṹ    1�����ֽڵ�ַ    3��8+X�ṹ    2��˫�ֽڵ�ַ
**                      suba        �����ӵ�ַ
**                      s           ���ݽ��ջ�����ָ��
**                      num         ��ȡ�ĸ���
** Output parameters:   ����ֵΪ0ʱ��ʾ����Ϊ1ʱ��ʾ������ȷ��
** Returned value:      NONE
*********************************************************************************************************/
uint8 readMem(uint16 page_start ,uint8 c_addr)
{
	uint8 Data[1]={0xff};
	read_mul(page_start, c_addr, Data,1);
	return(Data[0]);
}
/*********************************************************************************************************
** Function name:       read_mul
** Descriptions:        �����ӵ�ַ���������ַ��ʼ��ȡN�ֽ�����
** input parameters:    sla         ������ַ
**                      suba_type   �ӵ�ַ�ṹ    1�����ֽڵ�ַ    3��8+X�ṹ    2��˫�ֽڵ�ַ
**                      suba        �����ӵ�ַ
**                      s           ���ݽ��ջ�����ָ��
**                      num         ��ȡ�ĸ���
** Output parameters:   ����ֵΪ0ʱ��ʾ����Ϊ1ʱ��ʾ������ȷ��
** Returned value:      NONE
*********************************************************************************************************/
//uint8_t read_mul(uint8_t sla, uint8_t suba_type, uint32_t suba, uint8_t *s, uint32_t num)
void read_mul(uint16 page_start, uint8 c_addr, uint8 *message_dat,uint16 Len)
{
	uint8 suba_type=2;
	
	if (Len > 0)                                                      /* �ж�num�����ĺϷ���          */
	{                                                                 /* ��������                     */
		if (suba_type == 1)
		{                                                               /* �ӵ�ַΪ���ֽ�               */
			I2C_sla         = c_addr + 1;                                 /* �������Ĵӵ�ַ��R=1          */
			I2C_suba        = page_start;                                 /* �����ӵ�ַ                   */
			I2C_suba_num    = 1;                                        	/* �����ӵ�ַΪ1�ֽ�            */
		}
		if (suba_type == 2)
		{                                                               /* �ӵ�ַΪ2�ֽ�                */
			I2C_sla         = c_addr + 1;                                 /* �������Ĵӵ�ַ��R=1          */
			I2C_suba        = page_start;                                 /* �����ӵ�ַ                   */
			I2C_suba_num    = 2;                                        	/* �����ӵ�ַΪ2�ֽ�            */
		}
		if (suba_type == 3)
		{                                                               /* �ӵ�ַ�ṹΪ8+X              */
			I2C_sla         = c_addr + ((page_start >> 7 )& 0x0e) + 1;    /* �������Ĵӵ�ַ��R=1          */
			I2C_suba        = page_start & 0x0ff;                         /* �����ӵ�ַ                   */
			I2C_suba_num    = 1;                                        	/* �����ӵ�ַΪ8+x              */
		}
		I2C_buf     = message_dat;                                      /* ���ݽ��ջ�����ָ��           */
		I2C_num     = Len;                                              /* Ҫ��ȡ�ĸ���                 */
		I2C_suba_en = 1;                                                /* ���ӵ�ַ��                   */
		I2C_end     = 0;
																																		/* ���STA,SI,AA��־λ          */
		LPC_I2C2->I2CONCLR = (1 << 2)|                                  /* AA                           */
											 (1 << 3)|                                    /* SI                           */
											 (1 << 5);                                    /* STA                          */
																																		/* ��λSTA,����I2C����          */
		LPC_I2C2->I2CONSET = (1 << 5)|                                  /* STA                          */
											 (1 << 6);                                    /* I2CEN                        */
		Wait_I2c_End(20);																								/* �ȴ�I2C�������              */
//		delay_ms(20);
//		delay_ms(20);
//		return( Wait_I2c_End(20));
	}
//	delay_ms(20);
//	return (0);
}

void writeMem(uint16 page_start, uint8 c_addr ,uint8 message_in)
{
	write_mul(page_start, c_addr ,&message_in,1);
}
/*********************************************************************************************************
** Function name:       I2C_WriteNByte
** Descriptions:        �����ӵ�ַ����д��N�ֽ�����
** input parameters:    sla         ������ַ
**                      suba_type   �ӵ�ַ�ṹ    1�����ֽڵ�ַ    3��8+X�ṹ    2��˫�ֽڵ�ַ
**                      suba        �����ӵ�ַ
**                      s           ��Ҫд������ݵ�ָ��
**                      num         ��Ҫд������ݵĸ���
** Output parameters:   ����ֵΪ0ʱ��ʾ����Ϊ1ʱ��ʾ������ȷ��
** Returned value:      NONE
*********************************************************************************************************/
//uint8_t write_mul(uint8_t sla, uint8_t suba_type, uint32_t suba, uint8_t *s, uint32_t num)
void write_mul(uint16 page_start, uint8 c_addr ,uint8 *message_dat,uint8 Len)  //ҳд,�����
{
	uint8 suba_type=2;
	
	if (Len > 0)                                                        /* �����ȡ�ĸ���Ϊ0���򷵻ش���*/
	{                                                                   /* ���ò���                     */
		if (suba_type == 1){                                            	/* �ӵ�ַΪ���ֽ�               */
			I2C_sla         = c_addr;                                      	/* �������Ĵӵ�ַ               */
			I2C_suba        = page_start;                                  	/* �����ӵ�ַ                   */
			I2C_suba_num    = 1;                                        		/* �����ӵ�ַΪ1�ֽ�            */
		}

		if (suba_type == 2){                                            	/* �ӵ�ַΪ2�ֽ�                */
			I2C_sla         = c_addr;                                      	/* �������Ĵӵ�ַ               */
			I2C_suba        = page_start;                                  	/* �����ӵ�ַ                   */
			I2C_suba_num    = 2;                                        		/* �����ӵ�ַΪ2�ֽ�            */
		}

		if (suba_type == 3){                                            	/* �ӵ�ַ�ṹΪ8+X              */
			I2C_sla         = c_addr + ((page_start >> 7 )& 0x0e);         	/* �������Ĵӵ�ַ               */
			I2C_suba        = page_start & 0x0ff;                          	/* �����ӵ�ַ                   */
			I2C_suba_num    = 1;                                        		/* �����ӵ�ַΪ8+X              */
		}

		I2C_buf     = message_dat;                                      	/* ����                         */
		I2C_num     = Len;                                              	/* ���ݸ���                     */
		I2C_suba_en = 2;                                                	/* ���ӵ�ַ��д����             */
		I2C_end     = 0;                                                                        
		LPC_I2C2->I2CONCLR = (1 << 2)|                                    /* ���STA,SI,AA��־λ          */
											 (1 << 3)|
											 (1 << 5);
		LPC_I2C2->I2CONSET = (1 << 5)|                                    /* ��λSTA,����I2C����          */
											 (1 << 6);                                    	/* I2CEN                        */
		Wait_I2c_End(20);
		delay_ms(20);//�˴���ʱ��������,�������д���ɹ�
//		return( Wait_I2c_End(20));                                      /* �ȴ�I2C�������              */
	}
	delay_ms(20);//�˴���ʱ��������,�������д���ɹ�
//	return (0);
}

#endif

void fast_write_mul(uint16 page_start,uint8 c_addr,unsigned char *message_dat,uint16 Len)//����дI2C,�����
{
	uint16 i;
  uint8 m;
	if(page_start+Len>add_end_I2C)
    Len=add_end_I2C-page_start+1;
  m=PageLen_I2C-page_start%PageLen_I2C;
  if(Len>m)//��д����ǰҳ,�õ�ַΪ��һҳ��ҳ��
  {
    write_mul(page_start,c_addr,&message_dat[0],m);
    page_start+=m;
    Len-=m;
  }
  else
    m=0;
  for(i=0;i<Len/PageLen_I2C;i++)//дһ��ҳ
  {
    write_mul(page_start,c_addr,&message_dat[i*PageLen_I2C+m],PageLen_I2C);
    page_start+=PageLen_I2C;
  }
  Len%=PageLen_I2C;
  if(Len)//д��ʣ�²����һҳ
    write_mul(page_start,c_addr,&message_dat[i*PageLen_I2C+m],Len);
}
