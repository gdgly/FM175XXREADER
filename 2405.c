

#include "main.h"
#include "24Cxx.h"
#include "uart.h"

void WriteTimeOn(void)
{
	writeMem(0x10,I2C_2405,0xff);//0x80);//��WRTC1=1      
	writeMem(0x0f,I2C_2405,0xff);//0x84);//��WRTC2,WRTC3=1 
}

/******дSD2400��ֹ����******/
void WriteTimeOff(void)
{
	writeMem(0x0f,I2C_2405,0x00);
	writeMem(0x10,I2C_2405,0x00);
}

void EN_charge(void)
{
	unsigned char charge=0;
	unsigned char str[0x19];
	read_mul(0x00,I2C_2405,str,0x19);
	Send_string1(str,0x19);
	//54 15 A0 02 30 01 07 00 00 00 00 00 00 00 00 40 00 40 00 00 00 00 1C 00 82
	//45 32 A0 02 30 01 07 00 00 00 00 00 00 00 00 40 00 40 1F FF FF FF FF FF FF
	//55 46 A3 02 30 01 07 00 00 00 00 00 00 00 00 40 00 40 00 00 00 00 1B 00 82
	charge=str[0x18];
//	charge=readMem(0x10,I2C_2405);
	if(charge!=0x82)
	{
		WriteTimeOn();
		//writeMem(0x10,I2C_2405,0xff);//0x80);//��WRTC1=1      
		//writeMem(0x0f,I2C_2405,0xff);//0x84);//��WRTC2,WRTC3=1  
		
		writeMem(0x18,I2C_2405,0x82);
		
		WriteTimeOff();
		//writeMem(0x0f,I2C_2405,0x00);
		//writeMem(0x10,I2C_2405,0x00);
		Send_string1("\n���ʹ�ܼĴ���0x18���޸�Ϊ0x82.\n",0);
	}
	else
		Send_string1("\n���ʹ���Ѵ�.\n",0);
#if 0
	unsigned char n,i,charge=0;
/**
	xi2cStart();//"������ʼλ"
	xwriteAddr(xMEMWRITE);
	xwriteData(0x18);//����д��ʼ��ַ
	xi2cStop();
**/
	xi2cStart();//"������ʼλ"
	xwriteAddr(xMEMREAD);//"���Ͷ�������"
//	charge=xreadByte(LASTBYTE);
/**
	for(n=0;n<0x20;n++)
	{
    UART0_SendByte(hex[n>>4]);
    UART0_SendByte(hex[n&0x0f]);
    UART0_SendByte(' ');
	}
	UART0_SendByte('\r');
	UART0_SendByte('\n');	
**/
	for(n=0;n<0x20;n++)
	{
		if(n==0x1f)
			i=xreadByte(LASTBYTE);
		else
			i=xreadByte(!LASTBYTE);
		if(n==0x18)
			charge=i;
/**
    UART0_SendByte(hex[i>>4]);
    UART0_SendByte(hex[i&0x0f]);
    UART0_SendByte(' ');
**/
	}
	xi2cStop();
	if(charge!=0x82)
	{
		WriteTimeOn();
		xi2cStart();
		xwriteAddr(xMEMWRITE);
		xwriteData(0x18);//����д��ʼ��ַ
		xwriteData(0x82);//charge
		xi2cStop();
		WriteTimeOff();
	#if	LOGO_EN==1
		UART0SendStr("\n���ʹ�ܼĴ���0x18���޸�Ϊ0x82.\n",0);
	#endif
	}
	#if	LOGO_EN==1
	else
		UART0SendStr("\n���ʹ���Ѵ�.\n",0);
	#endif
#endif
}

/******дSD2400ʵʱ���ݼĴ���******/
void XI2CWriteDate(uchar *time_data)
{
  WriteTimeOn();

	time_data[2]|=0x80;//��ʮ��Сʱ��
	write_mul(0,I2C_2405,time_data,7);

	writeMem(0x12,I2C_2405,00);

	WriteTimeOff();
}
