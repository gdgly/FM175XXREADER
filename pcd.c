#include <intrins.h>
#include "STC15F2K60S2.h"
#include "main.h"
#include "pcd.h"
#include "spi.h"
#include "uart.h"
#include <string.h>

#define u8 unsigned char
#define u16 unsigned int

u8 fcnt;				//��Ӧʱ��
unsigned char idata PCB = 0x0A;					//CPU��APDUָ������

void delay_us(unsigned int x)
{
	while(x)
	{
		_nop_();
		x--;
	}		
}

void pcddelay(unsigned int z)
{
	unsigned int x,y;
	for(x=z;x>0;x--)
	for(y=110;y>0;y--);	
}
						
//*************************************************************************
// ������	��RC522_Init
// ����		����ʼ������
// ���		����
// ����		����
// ����		����
//*************************************************************************
void PcdInit(void)
{		
	PcdReset();
	PcdConfigISOType('A');
	fcnt = 0;
}

//*************************************************************************
// ������	��PcdSwitchPCB(void)
// ����		���л������
// ���		��
// ����		��
// ����		���ɹ�����MI_OK
//*************************************************************************

void PcdSwitchPCB(void)
{
	switch(PCB)
	{
		case 0x00:
			PCB=0x0A;
			break;
		case 0x0A:
			PCB=0x0B;
			break;
		case 0x0B:
			PCB=0x0A;
			break;
		default:
			PCB=0x0A;
			break;
	}
}

//*************************************************************************
// ������	��PcdConfigISOType(u8 type)
// ����		������RC522�Ĺ�����ʽ
// ���		��type[IN]:��Ƭ����
// ����		����
// ����		���ɹ�����MI_OK
//*************************************************************************
s8 PcdConfigISOType(u8 type)
{	
	s8 status = MI_ERR;
	if(type == 'A')
	{
		status = MI_OK;
		ClearBitMask(Status2Reg,0x08);	//״̬2�Ĵ���
		WriteRawRC(ModeReg,0x3D);			//��Mifare��ͨѶ,CRC��ʼֵ0x6363
		WriteRawRC(RxSelReg,0x86);		//ѡ���ڲ�����������,�ڲ�ģ�ⲿ�ֵ����ź�,�������ݺ�,��ʱ6��λʱ��,����	 10
		WriteRawRC(RFCfgReg,0x7F);		//���ý�����,48dB�������  
	    WriteRawRC(TReloadRegL,0x30);   	//��ʱ���ĵ�8λ���� 
	    WriteRawRC(TReloadRegH,0x00);		//��ʱ���ĸ�8λ����,ʵ��ֵ��0xD3E�ⲿ����Ҫ�����ö�ʱ���Ĵ���
    	WriteRawRC(TModeReg,0x8D); 		//ʵ��ֵ��0X0D3E�ⲿ����Ҫ�����ö�ʱ���Ĵ���TPrescaler ��Ƶ��Ϊ0xD0	  //8D
		WriteRawRC(TPrescalerReg,0x3E);	
		delay_us(1000);
		PcdAntennaOn();
	}
	return status;
}

//*************************************************************************
// ������	��WriteRawRC
// ����		��дRC522�Ĵ���
// ���		��Address[IN]:�Ĵ�����ַ
// 			  value:д����ֵ
// ����		����
// ����		����
//*************************************************************************
void WriteRawRC(u8 Address, u8 value)
{  
    u8 i, ucAddr;

    SCK = 0;
    NSS = 0;
    ucAddr = ((Address<<1)&0x7E);

    for(i=0;i<8;i++)								//�ȷ��͵�ַ
    {	 
		MOSI = ((ucAddr&0x80)==0x80);
        SCK = 1;
        ucAddr <<= 1;
        SCK = 0;	   
    }

    for(i=0;i<8;i++)
    {	
        MOSI = ((value&0x80)==0x80);				//�ٷ��Ͳ�����
        SCK = 1;
        value <<= 1;
        SCK = 0;
    }
    NSS = 1;
    SCK = 1;
}
//*************************************************************************
// ������	��ReadRawRC
// ����		����RC522�Ĵ���
// ���		��Address[IN]:�Ĵ�����ַ
// ����		����
// ����		��������ֵ
//*************************************************************************
u8 ReadRawRC(u8 Address)
{

     u8 i, ucAddr;
     u8 ucResult=0;

     SCK = 0;
     NSS = 0;
     ucAddr = ((Address<<1)&0x7E)|0x80; 

     for(i=0;i<8;i++)
     {
		MOSI = ((ucAddr&0x80)==0x80);
		SCK = 1;
		ucAddr <<= 1;
		SCK = 0;
     }

     for(i=0;i<8;i++)
     {
		SCK = 1;
		ucResult <<= 1;
		ucResult|=MISO&0x01;
		SCK = 0;
     }

     NSS = 1;
     SCK = 1;
     return ucResult;
}

//*************************************************************************
// ������	��SetBitMask
// ����		��λ��1
// ���		��reg[IN]:�Ĵ�����ַ
// 			  mask[IN]:��λֵ
// ����		����
// ����		����
//*************************************************************************
void SetBitMask(u8 reg,u8 mask)  
{
    u8 tmp = 0x0;
    tmp = ReadRawRC(reg);
    WriteRawRC(reg,tmp | mask);  // set bit mask
}

//*************************************************************************
// ������	��ClearBitMask
// ����		��λ��0
// ���		��reg[IN]:�Ĵ�����ַ
// 			  mask[IN]:��λֵ
// ����		����
// ����		����
//*************************************************************************
void ClearBitMask(u8 reg,u8 mask)  
{
    u8 tmp = 0x00;
    tmp = ReadRawRC(reg);
    WriteRawRC(reg, tmp & ~mask); 					 	// clear bit mask
} 
//*************************************************************************
// ������	��PcdAntennaOff
// ����		���ر�����
// ���		����
// ����		����
// ����		����
//*************************************************************************
void PcdAntennaOff(void)
{
    ClearBitMask(TxControlReg, 0x03);					//��ֹTx1RFEn,Tx2RFEn
}

//*************************************************************************
// ������	��PcdAntennaOn  
// ����		����������,ÿ��������ر����շ���֮��Ӧ������1ms�ļ��
// ���		����
// ����		����
// ����		����
//*************************************************************************
void PcdAntennaOn(void)
{
    unsigned char i;
    i = ReadRawRC(TxControlReg);						//��ȡ�����Ϳ��ƼĴ���
    if (!(i & 0x03))									//���δ����,��
    {
        SetBitMask(TxControlReg, 0x03);					//����Tx1RFEn,Tx2RFEn
    }
}

//*************************************************************************
// ������	��PcdComMF522  
// ����		��ͨ��RC522��ISO14443��ͨѶ
// ���		��Command[IN]:RC522������
// 			  pDataIn[IN]:ͨ��RC522���͵���Ƭ������
// 			  InLenByte[IN]:�������ݵ��ֽڳ���
// 			  *pOutLenBit[OUT]:�������ݵ�λ����
// ����		��pDataOut[OUT]:���յ��Ŀ�Ƭ��������
// ����		����
//*************************************************************************
s8 PcdComCmd(u8 Command, u8 *pDataIn, u8 InLenByte, u8 *pDataOut, u16  *pOutLenBit)
{
    s8 status = MI_ERR;
    u8 irqEn   = 0x00;
    u8 waitFor = 0x00;
    u8 lastBits;
    u8 n;
    u16 i;
    switch(Command)
    {
       case PCD_AUTHENT:
          irqEn   = 0x12;
          waitFor = 0x10;
          break;
       case PCD_TRANSCEIVE:
          irqEn   = 0x77;
          waitFor = 0x30;							// ���ܵ����ݼ�����ִ�����	
          break;
       default:
         break;
    }
    WriteRawRC(ComIEnReg,irqEn|0x80);			    // �������ʱ���ж�������Ϊ�������ж�����
    ClearBitMask(ComIrqReg,0x80);					// ����λ���
    WriteRawRC(CommandReg,PCD_IDLE);				// ȡ����ǰ����
    SetBitMask(FIFOLevelReg,0x80);					// ���FIFO�еĶ�дָ��

    for (i=0; i<InLenByte; i++)
    {   
		WriteRawRC(FIFODataReg, pDataIn[i]);    		//����д��FIFO
	}
    WriteRawRC(CommandReg, Command);					//д������,���������е����ݷ��͵�����,�������Զ�������
   
    if (Command == PCD_TRANSCEIVE)						//�������Ϊ0C
    {    
		SetBitMask(BitFramingReg,0x80);  				//�൱����������STARTSENG
	}
    i = 2000;											//����ʱ��Ƶ�ʵ���������M1�����ȴ�ʱ��=600,����CPU�����ȴ�ʱ��=1200
	do 
    {
         n = ReadRawRC(ComIrqReg);						//��ȡ�жϱ�־,������ݷ���
         i--;
    }
    while ((i!=0) && !(n&0x20) && !(n&waitFor));		// ��ʱ��δ��ʱ,û�д���,0x01,0x30
    ClearBitMask(BitFramingReg,0x80);					// �൱���������STARTSENG

    if (i!=0)											// ��ʱʱ�䵽��i��û�еݼ���0
    {    
         if(!(ReadRawRC(ErrorReg)&0x1B))				// �ж����޳��ִ����־	 Buffer���,λ��ͻ,����CRC����,��żУ�����,
         {	
             status = MI_OK;							// ��ʼ��״̬ 
		 	 if (n & irqEn & 0x01)						// ����PCD_TRANSCEIVE, irq = 0x77,  ��ʱ��Ϊ0�жϲ���,��ʱ��Ϊ0ʱΪ����
			 {   
				status = MI_NOTAGERR;   				// ����������
			 }
             if (Command == PCD_TRANSCEIVE)				// ����Ƿ��ͽ���ָ��
             { 
               	n = ReadRawRC(FIFOLevelReg);			// ��ȡ���յ������ݵ��ֽ���
              	lastBits = ReadRawRC(ControlReg) & 0x07;// 2-0:RxLastBits,��ʾ���յ����һ���ֽڵ�λ��
                if (lastBits)							// ����λΪ0����������ֽ���Ч
                {   
					*pOutLenBit = (n-1)*8 + lastBits;   //pOutLenBit��¼�ܹ��յ���λ��
				}
                else
                {   
					*pOutLenBit = n*8;   				//��������λ��
				}
                if (n == 0)								//����û���жϲ���
                {   
					n = 1;   							//n��1
				}
                if (n > MAXRLEN)						// һ������ܽ��ܵ����ֽ���
                {   
					n = MAXRLEN;   						//������󳤶�,ֻ������󳤶ȵ�ֵ
				}
                for (i=0; i<n; i++)
                {   
					pDataOut[i] = ReadRawRC(FIFODataReg); //��FIFO��ȡ����   
				}
            }
         }
         else
         {   
		 	status = MI_ERR;   							//�д���
		 }
   }
   SetBitMask(ControlReg,0x80);           	//ֹͣ��ʱ��
   WriteRawRC(CommandReg,PCD_IDLE); 		//���ָ��
   return status;								//����״̬
} 

//*************************************************************************
// ������	��PcdReset
// ����		����λRC522
// ���		����
// ����		����
// ����		���ɹ�����MI_OK
//*************************************************************************
s8 PcdReset(void)
{
    MFRST=0;
	P16 = 0;
	delay_us(1000);
    MFRST=1;
	P16 = 1;
	delay_us(1000);
    WriteRawRC(CommandReg,PCD_RESETPHASE);
    delay_us(100);
	WriteRawRC(ModeReg,0x3D);            	//��Mifare��ͨѶ��CRC��ʼֵ0x6363
    WriteRawRC(TModeReg,0x8D);			 	//��ʱ��ģʽ�Ĵ���,��ʱ����ֵ����
    WriteRawRC(TReloadRegL,0x30);        	//��ʱ���ĵ�8λ���� 
    WriteRawRC(TReloadRegH,0x00);			//��ʱ���ĸ�8λ����
    WriteRawRC(TPrescalerReg,0x3E);		 	//ʵ��ֵ��0X0D3E�ⲿ����Ҫ�����ö�ʱ���Ĵ���
	WriteRawRC(TxAutoReg,0x40);			 	//����Ҫ,�����߼�1,ǿ��100%ASK����
	PCB=0x00;
    return MI_OK;						 	//��ʱ��ʱ��6.78M/TPrescaler(ms)
}

//*************************************************************************
// ������	��CalulateCRC
// ����		����MF522����CRC16����
// ���		��pIndata[IN]:��Ҫ���������
// 			  len[IN]:���ݳ���
// ����		��pDataOut[OUT]:�������������ֽ�����
// ����		����
//*************************************************************************
void CalulateCRC(u8 *pIndata,u8 len,u8 *pDataOut)
{
    u8 i,n;
    ClearBitMask(DivIrqReg,0x04);
    WriteRawRC(CommandReg,PCD_IDLE);   //ȡ����ǰ����
    SetBitMask(FIFOLevelReg,0x80);	   //FlushBuffer ���ErrReg �ı�־λ
    for (i=0; i<len; i++)
    	WriteRawRC(FIFODataReg, *(pIndata+i));
    WriteRawRC(CommandReg, PCD_CALCCRC);
    i = 0xFF;
    do 
    {
        n = ReadRawRC(DivIrqReg);
        i--;
    }
    while ((i!=0) && !(n&0x04));		//��CRCIRq �������ݱ�������ϸ�λ��λ
    pDataOut[0] = ReadRawRC(CRCResultRegL);
    pDataOut[1] = ReadRawRC(CRCResultRegM);
}


//*************************************************************************
// ������	��PcdSelect
// ����		����֤��Ƭ����
// ���		��pSnr[IN]:��Ƭ���кţ�4�ֽ�
// ����		����
// ����		���ɹ�����MI_OK
//*************************************************************************
s8 PcdSelect(u8 *pSnr)
{
    s8 status;
    u8 i;
    u16 unLen;
    u8 ucComMF522Buf[MAXRLEN]; 		
    
    ClearBitMask(Status2Reg,0x08);				// ���У��ɹ���־	

	memset(ucComMF522Buf, 0x00, MAXRLEN);

    ucComMF522Buf[0] = PICC_ANTICOLL1;				// ����ͻ
    ucComMF522Buf[1] = 0x70;						// ����7�ֽ� 
    ucComMF522Buf[6] = 0;							//IDУ����0
    for (i=0; i<4; i++)
    {
    	ucComMF522Buf[i+2] = *(pSnr+i);				// ���濨ID
    	ucComMF522Buf[6]  ^= *(pSnr+i);				// ����У��ֵ
    }
    CalulateCRC(ucComMF522Buf,7,&ucComMF522Buf[7]);	// ���ɷ������ݵ�CRCУ��,���浽��������ֽ�
    status = PcdComCmd(PCD_TRANSCEIVE,ucComMF522Buf,9,ucComMF522Buf,&unLen);	// ����ѡ��,��CRCУ��
    
    if ((status != MI_OK) || (unLen != 0x18))		// ���ؽ����ȷ, ���ҳ���Ϊ24λ, 3�ֽ�,(1)���ڴ洢����С+(2)CRC
	{
    	status = MI_ERR;							// ����
	}
    return status;									// ���ؽ��
}
//*************************************************************************
// ������	��PcdAnticoll
// ����		������ײ
// ���		��pSnr[OUT]:��Ƭ���кţ�4�ֽ�
// ����		����
// ����		���ɹ�����MI_OK
//*************************************************************************
s8 PcdAnticoll(u8 *pSnr)
{
    s8 status;
    u8 i,snr_check=0;
    u16  unLen;
    u8 ucComMF522Buf[MAXRLEN]; 
   
    ClearBitMask(Status2Reg,0x08);			// ���У��ɹ���־	
    WriteRawRC(BitFramingReg,0x00);			// ���һ���ֽڷ�����������
    ClearBitMask(CollReg,0x80);				// CollRegCollReg	0��ͻ�������ͻλ������
 
	memset(ucComMF522Buf, 0x00, MAXRLEN);
	
    ucComMF522Buf[0] = PICC_ANTICOLL1;			// ����ͻָ�����λ�ڽ��յ���ͻ�����
    ucComMF522Buf[1] = 0x20;					// ����2���ֽ�

    status = PcdComCmd(PCD_TRANSCEIVE,ucComMF522Buf,2,ucComMF522Buf,&unLen);

    if (status == MI_OK)						
    {
    	 for (i=0; i<4; i++)					// ���ܿ�ID
         {   
             *(pSnr+i)  = ucComMF522Buf[i];		// 0-3:ID
             snr_check ^= ucComMF522Buf[i];		// У��ID
         }
         if (snr_check != ucComMF522Buf[i])		// 4:У��ֵ
		 { 	 
         	status = MI_ERR;					// У�����
		 }
    }
	else
	{
		status = MI_ERR;
	}
    SetBitMask(CollReg,0x80);					// CollRegCollReg	��106kbps���õķ���ͻ����¸�λ��1
	return status;								// ���ؽ��
}
//*************************************************************************
// ������	��PcdRequest
// ����		��Ѱ��
// ���		��req_code[IN]:Ѱ����ʽ, 0x52 = Ѱ��Ӧ�������з���14443A��׼�Ŀ�,0x26 = Ѱδ��������״̬�Ŀ�
// 			 
// ����		�� pTagType[OUT]����Ƭ���ʹ���
//			      0x4400 = Mifare_UltraLight 
//			      0x0400 = Mifare_One(S50)
//                0x0200 = Mifare_One(S70)
//                0x0800 = Mifare_Pro(X)CPU��
//                0x4403 = Mifare_DESFire
// ����		���ɹ�����MI_OK
//*************************************************************************
s8 PcdRequest(u8 req_code,u8 *pTagType)
{
	s8 status =MI_ERR;  
	u16 unLen;
	u8 ucComMF522Buf[MAXRLEN]; 

	PcdReset();
	
	ClearBitMask(Status2Reg,0x08);			// ���У��ɹ���־,���MFCrypto1Onλ
	WriteRawRC(BitFramingReg,0x07);			// StartSend =0;RxAlign=0�������һ���ֽڷ��͵�λ��,����7��λ
	SetBitMask(TxControlReg,0x03);			// �����߷����ź�,Tx1RFEn,Tx2RFEn��1
 
	memset(ucComMF522Buf, 0x00, MAXRLEN);
	
	ucComMF522Buf[0] = req_code;				//Ѱ����ʽ,���п���������ʲô��

	status = PcdComCmd(PCD_TRANSCEIVE,ucComMF522Buf,1,ucComMF522Buf,&unLen);// ���յ��Ŀ�Ƭ���ͺű���

	if(status == MI_OK)
	{
		*pTagType     = ucComMF522Buf[0];
		*(pTagType+1) = ucComMF522Buf[1];
	}	 
	else
	{
		status = MI_ERR;
	}
	return status;								//���ؽ��
}

//*************************************************************************
// ������	��PcdRats
// ����		��ת��APDU�����ʽ
// ���		�� ��
// ����		�� DataOut ��������ݣ�	Len ������ݵĳ���
// ����		���ɹ�����MI_OK
//*************************************************************************

s8 PcdRats(u8 * DataOut,u8 * Len)
{
	s8 status =MI_ERR;  
	u16 unLen;
	u8 ucComMF522Buf[MAXRLEN]; 

	ClearBitMask(Status2Reg,0x08);	// ���У��ɹ���־,���MFCrypto1Onλ

	memset(ucComMF522Buf, 0x00, MAXRLEN);

	ucComMF522Buf[0] = 0xE0;		
	ucComMF522Buf[1] = 0x51;				

    CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);	// ���ɷ������ݵ�CRCУ��,���浽��������ֽ�
	status = PcdComCmd(PCD_TRANSCEIVE,ucComMF522Buf,4,DataOut,&unLen);// ���յ��Ŀ�Ƭ���ͺű���

	if (status == MI_OK)
	{
		Len[0]= unLen/8-2;
		status = MI_OK;
	}	 
	else
		status = MI_ERR;	 
	return status;								//���ؽ��
}

//*************************************************************************
// ������	��GetCard()
// ����		��IC�����,����������
// ���		��req_code[IN]:Ѱ����ʽ, 0x52 = Ѱ��Ӧ�������з���14443A��׼�Ŀ�,0x26 = Ѱδ��������״̬�Ŀ�
// ����		��pTagType[OUT]����Ƭ���ʹ���
//			      	0x4400 = Mifare_UltraLight 
//			      	0x0400 = Mifare_One(S50)
//                	0x0200 = Mifare_One(S70)
//               	0x0800 = Mifare_Pro(X)
//                	0x4403 = Mifare_DESFire
//			  		snr[OUT]:IC��ID
// ����		���ɹ����� ST_OK
//*************************************************************************
u16 GetCard(u8 Reqcode, u8* pTagType, u8* pSnr)
{
	s8 status ;
	status = PcdRequest(Reqcode, pTagType);  				//Ѱ��
	if(status)
	{
		status = PcdRequest(Reqcode, pTagType);

		if(status)
		{
			return ST_ERR;
		}
	}

	status = PcdAnticoll(pSnr);								//����ײ, ��ȡID
	if(status)
	{
		return ST_ERR;
	}

	status = PcdSelect(pSnr);								//ѡ��Ƭ
	if(status)
	{
		return ST_ERR;
	}
	return ST_OK;

}
//*************************************************************************
// ������	��CpuReset()
// ����		��CPU��ר�ø�λ
// ���		����
// ����		��Data_Out ����ĸ�λ������Ϣ������
// ����		���ɹ�����9000
//*************************************************************************

u16 CardReset(u8 * Data_Out,u8 *  Len)
{
	s8 status = MI_OK;	
	status = PcdRats(Data_Out,Len);										//��Ƭ��λ
	if(status)
	{
		return ST_ERR;
	}
	else

		return ST_OK;
	
}




///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 
//*************************************************************************
// ������	��Pcd_Cmd
// ����		��ִ����λ��������ָ��
// ���		��pDataIn : Ҫִ�е�ָ�� In_Len  ָ�����ݳ���					
// ����		��pDataOut�����ִ�к�ķ�������   Out_Len��������ݳ���
// ����		��MI_OK
//*************************************************************************
 
u8 Pcd_Cmd(u8* pDataIn, u8  In_Len, u8* pDataOut,u8 * Out_Len)
{
	s8 status =MI_ERR;  
	u16 unLen;
	u8 ucComMF522Buf[MAXRLEN]; 
	u8 i;

	ClearBitMask(Status2Reg,0x08);					// ���У��ɹ���־,���MFCrypto1Onλ
	memset(ucComMF522Buf, 0x00, MAXRLEN);

 	PcdSwitchPCB();
	
	ucComMF522Buf[0] = PCB;
	ucComMF522Buf[1] = 0x01;


	ucComMF522Buf[2] = pDataIn[0];				// CLA
	ucComMF522Buf[3] = pDataIn[1];				// INS 			
	ucComMF522Buf[4] = pDataIn[2];				// P1						 
	ucComMF522Buf[5] = pDataIn[3];				// P2					
	ucComMF522Buf[6] = pDataIn[4];				// LEN	
	Hex_Print(In_Len);
	Hex_Print(ucComMF522Buf[0]);
	Hex_Print(ucComMF522Buf[1]);
	Hex_Print(ucComMF522Buf[2]);
	Hex_Print(ucComMF522Buf[3]);
	Hex_Print(ucComMF522Buf[4]);
	Hex_Print(ucComMF522Buf[5]);
	Hex_Print(ucComMF522Buf[6]);
	
	Send_char1('\r');
	Send_char1('\n');

	for(i=0;i<ucComMF522Buf[6];i++)				//DATA
	{
		ucComMF522Buf[7+i] = pDataIn[5+i];	  

	}
										
    CalulateCRC(ucComMF522Buf,In_Len+2,&ucComMF522Buf[In_Len+2]);	// ���ɷ������ݵ�CRCУ��,���浽��������ֽ�
	status = PcdComCmd(PCD_TRANSCEIVE,ucComMF522Buf,In_Len+4,pDataOut,&unLen); 							
	if (status == MI_OK)
	{
		Out_Len[0] = unLen/8-4;	 //���յ����ݵĳ���,��Ҫǰ��ͺ���ĸ������ֽڣ����Ƿ��ص���������						
		return MI_OK;
	}
	else
	{
	    Out_Len[0] = unLen/8-4;
		if((pDataOut[2]==0x90)&&(pDataOut[3]==0x00))
		return MI_OK;
		else
		return MI_ERR;

	}

}

//*************************************************************************
// ������	��PcdGetChallenge
// ����		����ȡ�����
// ���		����			  		
// ����		�� pRan: 4���ֽڵ������
// ����		���ɹ�����9000
//*************************************************************************
 
u16 PcdGetChallenge(u8* pRan)
{
	s8 status =MI_ERR;  
	u16 unLen;
	u8 i,ucComMF522Buf[MAXRLEN]; 
	u8 st[2];
	u16 sst=0;

	ClearBitMask(Status2Reg,0x08);		//���У��ɹ���־,���MFCrypto1Onλ
 
	memset(ucComMF522Buf, 0x00, MAXRLEN);
	
 	PcdSwitchPCB();
	
	ucComMF522Buf[0] = PCB;
	ucComMF522Buf[1] = 0x01;
	ucComMF522Buf[2] = 0x00;			// CLA
	ucComMF522Buf[3] = 0x84;			// INS - get random	
	ucComMF522Buf[4] = 0x00;			// P1
	ucComMF522Buf[5] = 0x00;			// P2
	ucComMF522Buf[6] = 0x04;			// Le - ������ĳ���=4���ֽ�	

    CalulateCRC(ucComMF522Buf,7,&ucComMF522Buf[7]);	// ���ɷ������ݵ�CRCУ��,���浽��������ֽ�
	
	status = PcdComCmd(PCD_TRANSCEIVE,ucComMF522Buf,9,ucComMF522Buf,&unLen);// ���յ��Ŀ�Ƭ���ͺű��� 	
	
	if (status == MI_OK)
	{
		st[0] = ucComMF522Buf[6];
		st[1] = ucComMF522Buf[7]; 

		sst=st[0];
		sst=(sst<<8)|st[1];

		if(sst==0x9000)
		{
			for(i = 0;i<4;i++)
				*(pRan+i) = ucComMF522Buf[2+i];
	
		}
		return sst;
	}
	else
		return ST_ERR;
}

//*************************************************************************
// ������	��PcdSelectFile
// ����		��ѡ���ļ�
// ���		��ChooseType : ѡ��ʽ
//					Lc : ����ѡ��ʽ�������ݳ���
//					pDataIn : �ļ���ʶ������DF ����
// ����		��	    pDataOut �������   Len��������ݳ���
// ����		���ɹ�����9000
//*************************************************************************
// 
u16 PcdSelectFile(u8* pDataIn, u8* pDataOut,u8 * Len)
{
	s8 status =MI_ERR;  
	u16 unLen;
	u8 ucComMF522Buf[MAXRLEN]; 
	u8 st[2];
	u16 sst=0;

	ClearBitMask(Status2Reg,0x08);					// ���У��ɹ���־,���MFCrypto1Onλ
	memset(ucComMF522Buf, 0x00, MAXRLEN);

 	PcdSwitchPCB();
	
	ucComMF522Buf[0] = PCB;
	ucComMF522Buf[1] = 0x01;
	ucComMF522Buf[2] = 0x00;				// CLA
	ucComMF522Buf[3] = 0xA4;				// INS-Select File			
	ucComMF522Buf[4] = 0x00;				// P1						 
	ucComMF522Buf[5] = 0x00;				// P2					
	ucComMF522Buf[6] = 0x02;				// Lc								
	ucComMF522Buf[7] = pDataIn[0];			//д���ļ���
	ucComMF522Buf[8] = pDataIn[1];									
    CalulateCRC(ucComMF522Buf,9,&ucComMF522Buf[9]);	// ���ɷ������ݵ�CRCУ��,���浽��������ֽ�
	status = PcdComCmd(PCD_TRANSCEIVE,ucComMF522Buf,11,pDataOut,&unLen);// ���յ��Ŀ�Ƭ���ͺű���								
	if (status == MI_OK)
	{
		st[0] = pDataOut[unLen/8-4];   //ִ�еķ���״̬
		st[1] = pDataOut[unLen/8-3];

		sst=st[0];
		sst=(sst<<8)|st[1];

		Len[0] = unLen/8-4;

		return sst;

	}
	else
		return ST_ERR;
}

//*************************************************************************
// ������	��PcdExAuth
// ����		���ⲿ��֤��Կ
// ���		��keyflag: �ⲿ��֤��Կ��ʶ��
// 			  pRan: 8���ֽڵ���֤��Կ.
// ����		����
// ����		���ɹ�����9000
//*************************************************************************

u16 PcdExAuth(u8 keysign, u8 *pRan)
{
	s8 status =MI_ERR;  
	u16 unLen;
	u8 ucComMF522Buf[MAXRLEN]; 
	u8 st[2];
	u16 sst=0;

	ClearBitMask(Status2Reg,0x08);			// ���У��ɹ���־,���MFCrypto1Onλ
	
	memset(ucComMF522Buf, 0x00, MAXRLEN);
	
 	PcdSwitchPCB();
	
	ucComMF522Buf[0] = PCB;	
	ucComMF522Buf[1] = 0x01;
	ucComMF522Buf[2] = 0x00;				// CLA
	ucComMF522Buf[3] = 0x82;				// INS - external authenticate
	ucComMF522Buf[4] = 0x00;				// P1
	ucComMF522Buf[5] = keysign;				// P2 - ��֤�������ʶ��  ���ͨΪ0x54
	ucComMF522Buf[6] = 0x08;				// Lc - 8���ֽڵ���Կ
	ucComMF522Buf[7] = pRan[0];					 
	ucComMF522Buf[8] = pRan[1];
	ucComMF522Buf[9] = pRan[2];
	ucComMF522Buf[10] = pRan[3];
	ucComMF522Buf[11] = pRan[4];
	ucComMF522Buf[12] = pRan[5];
	ucComMF522Buf[13] = pRan[6];
	ucComMF522Buf[14] = pRan[7];			// ����Ϊ8���ֽڵ���֤��Կ

    CalulateCRC(ucComMF522Buf,15,&ucComMF522Buf[15]);	// ���ɷ������ݵ�CRCУ��,���浽��������ֽ�
										
	status = PcdComCmd(PCD_TRANSCEIVE,ucComMF522Buf,17,ucComMF522Buf,&unLen);// ���յ��Ŀ�Ƭ���ͺű���
	
	if (status == MI_OK)
	{
		st[0] = ucComMF522Buf[2];	 //0X90
		st[1] = ucComMF522Buf[3]; 	 //0X00

		sst=st[0];
		sst=(sst<<8)|st[1];

		return sst;
	}
	else
	{
		if((ucComMF522Buf[2]==0x90)&&(ucComMF522Buf[3]==0x00))
		return 0x9000;
		else
		return MI_ERR;

	 }

}

//*************************************************************************
// ������	��PcdEraseDF
// ����		������Ŀ¼
// ���		����
// ����		����
// ����		���ɹ�����MI_OK
//*************************************************************************

u16 PcdEraseDF(void)
{  
	s8 status =MI_ERR;
	u16 unLen;
	u8 ucComMF522Buf[MAXRLEN];
	u8 st[2];
	u16 sst=0;

	ClearBitMask(Status2Reg, 0x08);												// ���У��ɹ���־,���MFCrypto1Onλ

	memset(ucComMF522Buf, 0x00, MAXRLEN);

 	PcdSwitchPCB();
	
	ucComMF522Buf[0] = PCB;	 
	ucComMF522Buf[1] = 0x01;
	ucComMF522Buf[2] = 0x80;			//CLA 
	ucComMF522Buf[3] = 0x0E;			//INS -EraseDF	
	ucComMF522Buf[4] = 0x00;			//P1
	ucComMF522Buf[5] = 0x00;			//P2
	ucComMF522Buf[6] = 0x00;			//Lc
	
    CalulateCRC(ucComMF522Buf, 7, &ucComMF522Buf[7]);								// ���ɷ������ݵ�CRCУ��,���浽��������ֽ�

	status = PcdComCmd(PCD_TRANSCEIVE, ucComMF522Buf, 9, ucComMF522Buf, &unLen);			// ���յ��Ŀ�Ƭ���ͺű���			 
	if (status == MI_OK)
	{
		st[0] = ucComMF522Buf[2];
		st[1] = ucComMF522Buf[3]; 

		sst=st[0];
		sst=(sst<<8)|st[1];

		return sst;
	}
	else
		return ST_ERR;
			
}
	
//*************************************************************************
// ������	��PcdEraseCheck
// ����		���������
// ���		����
// ����		��pDataOut ���������
// ����		��MI_OK ִ�гɹ�
//*************************************************************************

s8 PcdEraseCheck(u8* pDataOut,u8 *Len)
{
	s8 status =MI_ERR;
	u16 unLen;
	u8 ucComMF522Buf[MAXRLEN]; 
	
	ClearBitMask(Status2Reg, 0x08); 											// ���У��ɹ���־,���MFCrypto1Onλ
	
	memset(ucComMF522Buf, 0x00, MAXRLEN);
					   
 	PcdSwitchPCB();
	
	ucComMF522Buf[0] = 0xFA;
	ucComMF522Buf[1] = 0x01;
	ucComMF522Buf[2] = 0x01;

    CalulateCRC(ucComMF522Buf, 3, &ucComMF522Buf[3]);						// ���ɷ������ݵ�CRCУ��,���浽��������ֽ�

	status = PcdComCmd(PCD_TRANSCEIVE, ucComMF522Buf, 5, pDataOut, &unLen);	// ���յ��Ŀ�Ƭ���ͺű���		
	if (status == MI_OK)
	{
		Len[0]= unLen/8-2;
		return MI_OK;
	}
	return MI_ERR;


}


//*************************************************************************
// ������	��PcdCreateFile
// ����		�������ļ�
// ���		��fileid�ļ���ʶ  
// ����		��
// ����		��
//*************************************************************************

u16 PcdCreateFile(u8 *fileid, u8 Len, u8 *pDataIn)
{
	s8 status =MI_ERR;	
	u16 unLen;
	u8 i, ucComMF522Buf[MAXRLEN]; 
	u8 st[2];
	u16 sst=0;
	
		
	ClearBitMask(Status2Reg,0x08);	// ���У��ɹ���־,���MFCrypto1Onλ
	
	memset(ucComMF522Buf, 0x00, MAXRLEN);
	
	PcdSwitchPCB();
		
	ucComMF522Buf[0] = PCB;
	ucComMF522Buf[1] = 0x01;
	ucComMF522Buf[2] = 0x80;			//CLA
	ucComMF522Buf[3] = 0xE0;			//INS-Create File	
	ucComMF522Buf[4] = fileid[0];		//P1
	ucComMF522Buf[5] = fileid[1];		//P2
	ucComMF522Buf[6] = Len;				//Lc 
	for (i=0; i<Len; i++)
		ucComMF522Buf[7+i] = *(pDataIn+i);						//д������
	
	CalulateCRC(ucComMF522Buf,7+Len,&ucComMF522Buf[7+Len]); 	// ���ɷ������ݵ�CRCУ��,���浽��������ֽ�
										
	status = PcdComCmd(PCD_TRANSCEIVE,ucComMF522Buf,9+Len,ucComMF522Buf,&unLen);// ���յ��Ŀ�Ƭ���ͺű��� 	
					   				
	if (status == MI_OK)
	{
		st[0] = ucComMF522Buf[2];
		st[1] = ucComMF522Buf[3]; 

		sst=st[0];
		sst=(sst<<8)|st[1];

		return sst;
	}
	else
	{
		if((ucComMF522Buf[2]==0x90)&&(ucComMF522Buf[3]==0x00))
		{
			st[0] = ucComMF522Buf[2];
			st[1] = ucComMF522Buf[3]; 
	
			sst=st[0];
			sst=(sst<<8)|st[1];
	
			return sst;
		}
	
		else
		return MI_ERR;
	}
}

//*************************************************************************
// ������	��PcdSetKey
// ����		���޸���Կ
// ���		��
//					Keysign:��Կ��ʶ
//					Lc : ����
//					pDataIn : ��������
//					Le : ���س���
// ����		��
// ����		���ɹ�����9000
//*************************************************************************

u16 PcdSetKey(u8 Keysign, u8 Len, u8* pDataIn)
{
	s8 status = MI_ERR;  
	u16 unLen;
	u8 i,ucComMF522Buf[MAXRLEN]; 
	u8 st[2];
	u16 sst=0;

	ClearBitMask(Status2Reg,0x08);							//���У��ɹ���־,���MFCrypto1Onλ
	
	memset(ucComMF522Buf, 0x00, MAXRLEN);
	
 	PcdSwitchPCB();

	ucComMF522Buf[0] = PCB;
	ucComMF522Buf[1] = 0x01;
	ucComMF522Buf[2] = 0x80;
	ucComMF522Buf[3] = 0xD4;				
	ucComMF522Buf[4] = 0x01;								 
	ucComMF522Buf[5] = Keysign;								//��Կ��ʶ
	ucComMF522Buf[6] = Len;									//���ֽ��Ժ���������ݵĳ���
	for (i=0; i < Len; i++)										
		ucComMF522Buf[7+i] = *(pDataIn+i);					//��������
		
	CalulateCRC(ucComMF522Buf, 7+Len, &ucComMF522Buf[7+Len]);	// ���ɷ������ݵ�CRCУ��,���浽��������ֽ�	  
	
	status = PcdComCmd(PCD_TRANSCEIVE, ucComMF522Buf, 9+Len, ucComMF522Buf, &unLen);// ���յ��Ŀ�Ƭ���ͺű���
	if (status == MI_OK)
	{
		st[0] = ucComMF522Buf[2];	 //0X90
		st[1] = ucComMF522Buf[3]; 	 //0X00

		sst=st[0];
		sst=(sst<<8)|st[1];

		return sst;
	}
	else
		return ST_ERR;
}

//*************************************************************************
// ������	��PcdReadBinary
// ����		����ȡ�������ļ�
// ���		��sofset:���λΪ1,P1 P2Ϊ�����ļ���ƫ����,P1Ϊƫ�������ֽ�,P2Ϊ���ֽ�,�������ļ�Ϊ��ǰ�ļ�
//			: Len:Ҫ�������ݳ���		
// ����		��pDataOut������������ ,LenOut,���ص����ݳ���
// ����		���ɹ�����9000
//*************************************************************************

u16 PcdReadBinary(u8 P1,u8 P2 , u8 Len, u8* pDataOut,u8* LenOut)
{
	s8 status =MI_ERR;  
	u16 unLen;
	u8 ucComMF522Buf[MAXRLEN]; 
	u8 st[2];
	u16 sst=0;


	ClearBitMask(Status2Reg,0x08);							//���У��ɹ���־,���MFCrypto1Onλ
		
	memset(ucComMF522Buf, 0x00, MAXRLEN);
	
 	PcdSwitchPCB();

	ucComMF522Buf[0] = PCB;
	ucComMF522Buf[1] = 0x01;
	ucComMF522Buf[2] = 0x00;
	ucComMF522Buf[3] = 0xB0;			
	ucComMF522Buf[4] = P1;								//��ʼλ�ø��ֽ�	  ,ע�⣬��8λ����Ϊ1��P1����Ч��ַΪ0X7F
	ucComMF522Buf[5] = P2;								//��ʼλ�õ��ֽ�	                       P2����Ч��ַΪ0XFF
	ucComMF522Buf[6] = Len;									//�������ݵĳ���
	CalulateCRC(ucComMF522Buf, 7, &ucComMF522Buf[7]);	// ���ɷ������ݵ�CRCУ��,���浽��������ֽ�	
		
	status = PcdComCmd(PCD_TRANSCEIVE, ucComMF522Buf, 9, pDataOut, &unLen);// ���յ��Ŀ�Ƭ���ͺű���


	if (status == MI_OK)
	{
		st[0] = pDataOut[Len+2];	 //0X90
		st[1] = pDataOut[Len+3]; 	 //0X00
					   
		sst=st[0];
		sst=(sst<<8)|st[1];
		LenOut[0] = unLen/8;
		return sst;
	}
	else
		return ST_ERR;

}


//*************************************************************************
// ������	��PcdUpdateBinary
// ����		��д��������ļ�
// ���		��AddressH,AddressL : ��ʼ��ַ�ĸ�λ����λ
//			  Len : д����ֽ���		
//			  pDataIn : д������		
//					
// ����		����
// ����		���ɹ�����9000
//*************************************************************************

u16 PcdUpdateBinary(u8 P1,u8 P2 , u8 Len, u8* pDataIn)
{
	s8 status =MI_ERR;  
	u16 unLen;
	u8 i,ucComMF522Buf[MAXRLEN];  
	u8 st[2];
	u16 sst=0;

	ClearBitMask(Status2Reg,0x08);						// ���У��ɹ���־,���MFCrypto1Onλ

 
	memset(ucComMF522Buf, 0x00, MAXRLEN);

 	PcdSwitchPCB();
	
	ucComMF522Buf[0] = PCB;
	ucComMF522Buf[1] = 0x01;
	ucComMF522Buf[2] = 0x00;
	ucComMF522Buf[3] = 0xD6;				
	ucComMF522Buf[4] = P1;										//��ʼλ�ø��ֽ�	  ,ע�⣬��8λ����Ϊ1��P1����Ч��ַΪ0X7F
	ucComMF522Buf[5] = P2;										//��ʼλ�õ��ֽ�	                       P2����Ч��ַΪ0XFF
	ucComMF522Buf[6] = Len;											//�������ݳ���
	for (i=0; i<Len; i++)										
		ucComMF522Buf[7+i] = *(pDataIn+i);							//��������
		
	CalulateCRC(ucComMF522Buf, 7+Len, &ucComMF522Buf[7+Len]);
	
	status = PcdComCmd(PCD_TRANSCEIVE, ucComMF522Buf, 9+Len, ucComMF522Buf, &unLen);

	if (status == MI_OK)
	{
		st[0] = ucComMF522Buf[2];	 //0X90
		st[1] = ucComMF522Buf[3]; 	 //0X00

		sst=st[0];
		sst=(sst<<8)|st[1];

		return sst;
	}
	else
		return ST_ERR;
}

//*************************************************************************
// ������	��WTX_Cmd
// ����		���˺�����Ҫ�����ǣ�����CPU�����ļ���ɾ���ļ�ʱ����Ҫ�ṩ�ȴ�ʱ�䣬
//            ��ִ��CPU��ָ���01ʱ���������01���ͻظ�CPU����ֱ���ȵ����ٷ���01��ִ�гɹ�
// ���		��pDataIn : Ҫִ�е�ָ��					
// ����		��pDataOut�����ִ�к�ķ�������   Out_Len��������ݳ���
// ����		��MI_OK
//*************************************************************************
char  WTX_Cmd(unsigned char* pDataOut,unsigned char * Out_Len)
{
  
    char status;

	unsigned int  idata sst=0;
	unsigned int  unLen;
    unsigned char ucComMF522Buf[MAXRLEN]; 
  
  
	ClearBitMask(Status2Reg,0x08);					// ���У��ɹ���־,���MFCrypto1Onλ
	memset(ucComMF522Buf, 0x00, MAXRLEN);


    ucComMF522Buf[0] = 0xFA;
    ucComMF522Buf[1] = 0x01;
	ucComMF522Buf[2] = 0X01;				
		

    CalulateCRC(ucComMF522Buf,3,&ucComMF522Buf[3]);	// ���ɷ������ݵ�CRCУ��,���浽��������ֽ�

	status = PcdComCmd(PCD_TRANSCEIVE,ucComMF522Buf,5,pDataOut,&unLen);
								
    if (status == MI_OK)
    {
 	   
	    *Out_Len=unLen/8;
		Out_Len[0]-=4;
		memcpy(pDataOut, &pDataOut[2], Out_Len[0]);	//DATA	

        return   MI_OK;
    }
	else
	{
		return ST_ERR;
	}
	
 }

 unsigned char  PcdStatus(unsigned int sta)
{
	unsigned char t[2];
	unsigned char  stdata[4];
	unsigned char TimeOut=200;
	if(sta!=0x9000) 
	{
		 if(sta!=0xFFFF) 
		 {
			 t[0]=sta>>8;
			 t[1]=sta&0x00ff;
		     if(t[0]==0x01)	  //����01ʱ����Ҫ�ȴ�ʱ������
			 {
				while(1)
				{
					WTX_Cmd(stdata,t);
					if((stdata[2]==0x90)&&(stdata[3]==0x00))//ֱ������9000���˳�
					{
						return 0;	//ִ��ָ����ȷ
					}
					else
					pcddelay(100);
					TimeOut--;
					if(TimeOut==0)
					{
//						UART_Send_Str("��ʱ�˳�");
						return 1;
					}
				}
			 }
			 else
			 {

//				 UART_Send_Str("������:");
//				 UART_Put_Num(t,2);
			 }
		 }
		 else {
			;
		 }
//		 UART_Send_Str("����ʧ��!");
//		 UART_Send_Enter();	
		 return 1;	 //����
	}
	else
		return 0;	//ִ��ָ����ȷ
}

 
void Request_loop(void)
{
 	s8 status,atq[2] ;
    while(1)
    {     
        status = PcdRequest(PICC_REQALL, atq);
        if(status != MI_OK)
        {
            status = PcdRequest(PICC_REQALL, atq);	
	        if(status != MI_OK)
	        break;
        }
    }
 }


