/*************************************************************/
//2014.03.06�޸İ�
/*************************************************************/
#include "STC15F2K60S2.h"
#include "intrins.h"
#include "pin.h"
#include "spi.h"
#include "main.h"
#include "uart.h"


unsigned char SPI_Init()
{
	SPDAT=0;
	SPSTAT=SPIF|WCOL;
	SPI_CS=1;
	SPCTL=0xD0;//SSIG=1,SPEN=1,DORD=0,MSTR=1,CPOL=0,CPHA=0,SPR1=0,SPR0=0
	//SPCTL=0xD1;
	AUXR1=0x00;	//PCA_P4=0,SPI_P4=0,S2_P4=0,GF2=0,ADRJ=0,DPS=0
	return OK;
}

unsigned char wait_SPSTAT(unsigned char Data)
{
	unsigned int i;
	do
	{
		i++;
		if(i>200)
		{
			return 0;
		}
	}
	while(!(SPSTAT & Data));
	return 1;
}
/*******************************************************************************************************/
/*���ƣ�																							   */
/*���ܣ�SPI�ӿڶ�ȡ����	SPI read function															   */
/*����:																								   */
/*		N/A																							   */
/*�����																							   */
/*		addr:	��ȡFM175XX�ڵļĴ�����ַ[0x00~0x3f]	reg_address									   */
/*		rddata:	��ȡ������							reg_data										   */
/*******************************************************************************************************/
unsigned char SPIRead(unsigned char addr)				//SPI������
{
	unsigned char data reg_value,send_data;
	SPI_CS=0;

	send_data=addr<<1;
	send_data|=0x80;
	SPSTAT=0xC0;//SPIF=1,WCOL=1
	SPDAT=send_data;
	wait_SPSTAT(0x80);

	SPSTAT=0xC0;//SPIF=1,WCOL=1
	SPDAT=0x00;
#if 1
	wait_SPSTAT(0x80);
	reg_value = SPDAT;
#else
	if(1 == wait_SPSTAT(0x80))
		reg_value = SPDAT;
	else
		reg_value = 0x01;
#endif
	SPI_CS=1;
	return(reg_value);
}

void SPIRead_Sequence(unsigned char sequence_length,unsigned char addr,unsigned char *reg_value)	
//SPI����������,����READ FIFO����
{
	unsigned char data i,send_data;
//	unsigned char i,send_data;
  
Debug_Send_string1("SPIRead_Sequence\n",0);

	if (sequence_length==0) //|| sequence_length>0x20)
		return;
	
	SPI_CS=0;

	send_data=(addr)<<1;
	send_data|=0x80;
	SPSTAT=0xC0;//SPIF=1,WCOL=1
	SPDAT=send_data;
	wait_SPSTAT(0x80);
  
 Debug_Send_char1('0');
 Debug_Send_char1( sequence_length);
 
	for (i=0;i<sequence_length;i++)
	{
//		Send_char1(i);
    SPSTAT=0xC0;//SPIF=1,WCOL=1
		if (i==sequence_length-1)
			SPDAT=0x00;
		else
			SPDAT=send_data;

		wait_SPSTAT(0x80);
		*(reg_value+i)=SPDAT;
 	}
	SPI_CS=1;
  
Debug_Send_string1("end\n",0); 
 
	return;
}
/*******************************************************************************************************/
/*���ƣ�SPIWrite																					   */
/*���ܣ�SPI�ӿ�д������	  SPI write function														   */
/*����:																								   */
/*		add:	д��FM17XX�ڵļĴ�����ַ[0x00~0x3f]	  reg_address									   */
/*		wrdata:   Ҫд�������						  reg_data										   */
/*�����																							   */
/*		N/A																							   */
/*******************************************************************************************************/
void SPIWrite(unsigned char addr,unsigned char wrdata)	//SPIд����
{
	unsigned char data send_data;
	SPI_CS=0;				   
	send_data=addr<<1;
	send_data=send_data&0x7e;
	
  	SPSTAT=0xC0;//SPIF=1,WCOL=1
	SPDAT=send_data;
	wait_SPSTAT(0x80);

	SPSTAT=0xC0;
	SPDAT=wrdata;
	wait_SPSTAT(0x80);

	SPI_CS=1;
	return ;	
}

void SPIWrite_Sequence(unsigned char sequence_length,unsigned char addr,unsigned char *reg_value)
//SPI����д����,����WRITE FIFO����
{
	unsigned char data send_data,i;
	if(sequence_length==0)
		return;

	SPI_CS=0;
		   
	send_data=addr<<1;
	send_data=send_data&0x7e;
	
  	SPSTAT=0xC0;//SPIF=1,WCOL=1
	SPDAT=send_data;
	wait_SPSTAT(0x80);

	for (i=0;i<sequence_length;i++)
	{
	SPSTAT=0xC0;
	SPDAT=*(reg_value+i);
	wait_SPSTAT(0x80);
	}

	SPI_CS=1;
	return ;	
}
