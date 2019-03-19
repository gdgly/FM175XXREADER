/*************************************************************/
//2014.06.03�޸İ�
/*===========================================================*/
#include "STC15F2K60S2.h"
#include "intrins.h"
#include "pin.h"
//#include "fm175xx.h"
//#include "spi.h"
//#include "type_a.h"
//#include "type_b.h"
//#include "ultralight.h"
#include "mifare_card.h"
#include "uart.h"
//#include "cpu_card.h"
#include "main.h"
#include "pcd.h"
#include "uart_app.h"
//#include "24c64.h"
#include "I2C.h"
#include "BT.h"
#include "EEPROM.h"
#include "24Cxx.h"
#include "2405.h"

uchar xdata receive_data1[rs_Max_Len1]={0};
uint receive_number1=0;//
uint R_ACC1;//=0;
uchar UART1_rs[9];

uchar xdata receive_data2[rs_Max_Len2]={0};
uint receive_number2=0;//
uint R_ACC2;//=0;
uchar UART2_rs[9];

uchar S_Yadd=0;
uchar S_Madd=0;
uchar S_LSH=0;

uchar C_add=0x0a;
uchar idata MCU_ID[7];
	
unsigned  char ovt=0;

// unsigned char bjid[12]={0,0,0,0};

unsigned char mima[6]={0xff,0xff,0xff,0xff,0xff,0xff} ;
unsigned char mdata[16*16+1]="����Ա: ";
unsigned char mima2[6]={0xc8,0xf0,0xb7,0xe5,0x18,0x13} ;
unsigned char rxbuf[33] ;
//uchar ly[33] = "������";//{0xc0,0xb6,0xd1,0xc0,0xd6,0xdc,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,};
#define open_box_cmd_add 0x0000 //
uchar open_box_cmd[33];// = "ֱ�Ӻ�";//{0xd6,0xb1,0xbd,0xd3,0xba,0xd0};//,0x05,0x06,0x07,0x08,0x09,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x01,0x02,0x1f};
uchar ty[100];
uchar SN=0;
uchar _1a_flag=0;//���;���
uchar _1a_flag_17xx_EN=0;
uchar _1a_flag_17xx=0;
uchar _1a_flag_11xx=0;
uchar Time0_=0;//=0x00;	  //��ʱ���ж��ۼ���
	
bit GPRS_flag=0;

uchar OPEN_EN_17xx=0;//0x00:�ر�,���ɿ���  0x01:����,���Կ���
uchar OPEN_EN_11xx=0;

uchar ICC_type=0;//������

uchar User_type=0;//�û����� 0:�㳮Ա   1:����Ա

uchar Hex[]="0123456789ABCDEF";
	
unsigned char go_flag=0;			//��������״̬��־λ

void Io_Init();
void Delay_100us(unsigned int delay_time);
void Variable_Init(void);

void WDT_rest(void)
{
	WDT_CONTR=0x35;
}
uchar X_or(uchar *pz,uint Len)//pz����׵�ַ,Len���ݳ���(������65535),�������ֵ
{
  uchar uXor=0;//
  while(Len--)
  {
    uXor^=*pz++;
  }
  return uXor;
}
unsigned int crc16_char(unsigned int gx, unsigned char Data)//OK 
{
//x^16+x^15+x^2+x^0=1 1000 0000 0000 0101=0x18005 
  unsigned int  crc=0;
  unsigned char i;
	for(i=0;i<8;i++)
	{
		if((crc^((unsigned int)(Data)<<8))&0x8000) //��תʱ crc&0x0001
			crc=(crc<<=1)^gx; //��תʱ crc=(crc>>=1)^gx
		else
			crc<<=1; //��תʱ crc>>=1
		Data<<=1;
	}
  return (crc);
}
unsigned int crc16_string(unsigned int gx, unsigned char *pt, unsigned int Len)//OK
{
  unsigned int  crc=0;
  while(Len--)
  {	
    crc=(crc<<8)^crc16_char(gx,(crc>>8)^*pt++);//����
//    crc=(crc>>8)^crc16_char(gx,(crc&0xff)^*pt++);//����
//    crc=GetCRC16(crc,*pt++);//���
  }	
  return crc;
}
void Delay(unsigned int t)
{
	unsigned int x,y;
	for(x=t;x>0;x--)
		for(y=110;y>0;y--);
}
#if 0
unsigned char wrm1(char key,char *miyao,char bank,char sector,char *sj) //дM1�����������������key:A 0��Կ��֤����B 1��Կ��֤��
//miyao:��֤�����롣6�ֽ�
//bank�����
//sector:������
//sj:��Ҫд������� 16�ֽ�
{		//send(0x33);
	if(bank>3 || sector > 15)
		return 0;
	bank += 4*sector;
	Set_Rf(3);   //turn on radio ����Ƶ���
	Pcd_ConfigISOType(0);
		// send(0x44);
	send_buff.buff[0]=TypeA_CardActivate(&PICC_A);	 //����ѡ��
	   	// send(0x55);
	if(send_buff.buff[0]==OK)
	{ 
		if(Mifare_Auth(key,sector,miyao,PICC_A.UID)==OK)//��֤ �ڶ�������������
		{ 
			if( Mifare_Blockwrite(bank,sj)==OK)
			{
				Set_Rf(0);
				return 1;
			}
			else
			{
				Set_Rf(0);
				return 0;
			}
		}
		else
		{
			Set_Rf(0);  //�ر���Ƶ���
			return 0;
		}
	}
	else
	{
		Set_Rf(0);  //�ر���Ƶ���
	 // send(0x66);
		return 0;
	}
}           																

char rem1(char key,unsigned char *miyao,char bank,char sector,char *js)//���ܣ���M1��;������key:��֤ģʽ��miyao 6�ֽ����룬bank ��ţ�sector:������
{
	if(bank>3 || sector > 15)
		return 0;
	bank += 4*sector;
	Set_Rf(3);   //turn on radio
	Pcd_ConfigISOType(0);
	send_buff.buff[0]=TypeA_CardActivate(&PICC_A);
	if(send_buff.buff[0]==OK)
	{
		if(Mifare_Auth(key,sector,miyao,PICC_A.UID)==OK)//��֤ �ڶ�������������
		{
			if(Mifare_Blockread(bank,js)==OK)
			{
				Set_Rf(0);
				return 1;
			}
			else
			{
				Set_Rf(0);
				return 0;
			}
		}
		else
		{
			Set_Rf(0);
			return 0;
		}
	}
	else
	{
		Set_Rf(0);
		return 0;
	}
}
unsigned char rem1_sector(char key,unsigned char *miyao,char bank,char sector,char *js,unsigned int Len)//���Կ��,���Կ�����,��Ҫ������������ͬ
{
	unsigned int i;
	for(i=0;i<Len/16;i++)
	{
		if(0 == rem1(key,miyao,(bank+i)%4,sector+(bank+i)/4,js+16*i))
		{
			break;
		}
	}
	if((Len/16) == i)
	{
		if(Len%16)
		{
			return(rem1(key,miyao,(bank+i)%4,sector+(bank+i)/4,js+16*i));
		}
		return 1;
	}
	else
	{
		return 0;
	}
}
#endif

void UART_(unsigned char *receive_data,unsigned char *buf,unsigned char uartx)
{
  uchar Data;
  uint receive_number,R_ACC;
	uint rece_len;
  switch(uartx)
  {
   #if UART1==1
    case 1:
    Data=SBUF;
    receive_number=receive_number1;
	rece_len = rece_buff.length;
    R_ACC=R_ACC1;
		//Send_char1(Data);
    break;
   #endif
   
   #if UART2==1
    case 2:
    Data=S2BUF;
    receive_number=receive_number2;
    R_ACC=R_ACC2;
		//Send_char1(Data);return;
		/**/
		if(0 == AT_BT)
		{
			receive_data[receive_number++]=Data;
			receive_number2=receive_number;
			buf[RH_ID]=1;
			buf[RB_ID]=0;
			buf[Time0_COM]=0x00;
			return;
		}
		/**/
    break;
   #endif
    
   #if UART3==1
    case 3:
    Data=S3BUF;
    receive_number=receive_number3;
    R_ACC=R_ACC3;
    break;
   #endif
    
   #if UART4==1
    case 4:
    Data=S4BUF;
    receive_number=receive_number4;
    R_ACC=R_ACC4;
    break;
   #endif
   
   default:return;
  }

start_uart:
  rece_buff.buff[rece_len] = Data;
  receive_data[receive_number] = rece_buff.buff[rece_len]; 
  buf[R_XOR]^=receive_data[receive_number];
  if(buf[R_ACCfinish]==0)
    R_ACC+=receive_data[receive_number];
  receive_number++;
	rece_len ++;
  if(1 == receive_number)
  {
    buf[R_XOR]=receive_data[0]^0x00;
    R_ACC=receive_data[0]+0x00;
    buf[R_ACCfinish]=0;
    buf[RH_ID]=1;
    buf[RB_ID]=0;
    buf[Time0_COM]=0x00;
    switch(receive_data[0])
    {    
      case 0x1e:buf[_XY_]=0x1e;break;
      default:buf[RH_ID]=0;receive_number=0;break;
    }
  }
  else if(2 == receive_number)
  {
    if(0x1e == receive_data[0] && 0x60 == receive_data[1])
    { 
      buf[_XY_]=0x1e;
    }
    else
    {
      receive_number=0;
      goto start_uart;
    }
  }
  else if(0x1e == buf[_XY_])//1e 60
  {
	#if 0
    if(3 == receive_number)
    {
      /***
      if(receive_data[2]!=0xff&&receive_data[2]!=C_add)//�ж���Ϣ��Ŀ���ַ
      {
        receive_number=0x00;
        goto start_uart;
      }
      ***/
    }
    else if(4 == receive_number)
    {
      /**
      if((receive_data[2]==0xff||receive_data[2]==C_Address3||receive_data[2]==C_Address4)&&receive_data[3]==0x03)//�����п��������Ĳ�������
      {
        receive_number=0x00;
        goto start_uart;
      }
      **/
    }
    else if(5 == receive_number)
    {
      /**if(
        receive_data[4]!=0x01&&//�����豸
        receive_data[4]!=0x02&&//Уʱ
        receive_data[4]!=0x05&&//Ӧ��
        receive_data[4]!=0x06&&//����
        receive_data[4]!=0x07  //ʵʱ
        )//�ж��Ƿ�������Ҫ����Ϣ
      {
        receive_number=0x00;
        goto start_uart;
      }**/
    }
  #endif
		if(receive_number>9)//�Ƿ���յ���С����
    {
      if((((uint)(receive_data[6])<<8)+receive_data[7]==receive_number-10))//�жϳ����Ƿ���ȷ
      {
        if(buf[R_XOR]==0x1f)//��β��XOR�Ƿ���ȷ
        {
					buf[RB_ID]=1;
        }
        else
        {
          Send_char1(buf[R_XOR]^0x1f^receive_data[receive_number-2]);
          buf[RB_ID]=0;
        }
        buf[RH_ID]=0;
        receive_number=0;
        buf[Time0_COM]=0x00;
      }
      else
      {
        buf[RB_ID]=0;
      }
    }
  }
  else
  {
    receive_number=0;
  }

  switch(uartx)
  {
   #if UART1==1
    case 1:
    if(buf[RB_ID]==1)
      ES=0;//������
    receive_number1=receive_number;
	rece_buff.length=rece_len;
    R_ACC1=R_ACC;
    break;
   #endif
   
   #if UART2==1
    case 2:
    if(buf[RB_ID]==1)
      IE2 &= ~0x01;//������
    receive_number2=receive_number;
    R_ACC2=R_ACC;
    break;
   #endif
   
   #if UART3==1
    case 3:
    if(buf[RB_ID]==1)
      IE2 &= ~0x08;//������
    receive_number3=receive_number;
    R_ACC3=R_ACC;
    break;
   #endif
   
   #if UART4==1
    case 4:
    if(buf[RB_ID]==1)
      IE2 &= ~0x10;//������
    receive_number4=receive_number;
    R_ACC4=R_ACC;
    break;
   #endif

    default:return;
  }
}

void uart1_inte() interrupt 4	//point,data_length
{
  if(TI)
    TI=0;
  if(RI)
  {  //bz=1;
    RI=0;
		if(receive_number1>=rs_Max_Len1)
			receive_number1=0;
		UART_(receive_data1,UART1_rs,1);
		uart_status.rece_finish=True;
		uart_status.rece_error=False;
	}
}

void uart2_inte() interrupt 8
{
	if (S2CON & S2RI)
  {
    S2CON &= ~S2RI;         //���S2RIλ
    //P0 = S2BUF;             //P0��ʾ��������
    //P2 = (S2CON & S2RB8);   //P2.2��ʾУ��λ
		if(receive_number2>=rs_Max_Len2)
			receive_number2=0;
    UART_(receive_data2,UART2_rs,2);
  }
  if (S2CON & S2TI)
  {
    S2CON &= ~S2TI;         //���S2TIλ
  }
}

void fmq()
{
	P34=0;
	Delay(2000);
	P34=1;
}

void Timer0Init(void)		//50����@11.0592MHz
{
	AUXR &= 0x7F;		//��ʱ��ʱ��12Tģʽ
	TMOD &= 0xF0;		//���ö�ʱ��ģʽ
	TL0 = 0x00;		//���ö�ʱ��ֵ
	TH0 = 0x4C;		//���ö�ʱ��ֵ
	TF0 = 0;		//���TF0��־
	TR0 = 1;		//��ʱ��0��ʼ��ʱ
  ET0=1;
  EA=1;//�����ж�
}
void time0(void) interrupt 1  //50ms�ж�
{
	Time0_++;

	if(UART1_rs[RH_ID])
	{
		UART1_rs[Time0_COM]++;
		if(UART1_rs[Time0_COM]>=20)//50ms*20
		{
			Send_char1((receive_number1>>8)&0xff);
			Send_char1((receive_number1>>0)&0xff);
			//Send_string1(receive_data1,receive_number1);
			UART1_rs[RB_ID]=0;
			UART1_rs[RH_ID]=0;
			receive_number1=0;
			UART1_rs[Time0_COM]=0x00;
			Send_char1(0xE1);
		}
	}
	if(UART2_rs[RH_ID])
	{
		UART2_rs[Time0_COM]++;
		if(UART2_rs[Time0_COM]>=20)//50ms*20
		{
			Send_char1((receive_number2>>8)&0xff);
			Send_char1((receive_number2>>0)&0xff);
			//Send_string1(receive_data2,receive_number2);
			UART2_rs[RB_ID]=0;
			UART2_rs[RH_ID]=0;
			receive_number2=0;
			UART2_rs[Time0_COM]=0x00;
			Send_char1(0xE2);
		}
	}
		
	if(1 == STAT_BT)
	{
		GPRS_flag=1;
	}
	else
	{
		GPRS_flag=0;
		if(1 == OPEN_EN_17xx)
			OPEN_EN_17xx=0;
		if(1 == OPEN_EN_11xx)
			OPEN_EN_11xx=0;
	}
	
//	OPEN_EN_17xx=1;
//	OPEN_EN_11xx=1;
	
  if(Time0_%10 == 0)
  {
    not_LED;
		if(1 == STAT_BT)
		{
			if(0 == User_type)//�㳮Ա
			{
				Close_LED3;
				if((0 == OPEN_EN_17xx) && (0 == OPEN_EN_11xx))
					Open_LED2;
				else
					not_LED2;
			}
			else if(1 == User_type)//����Ա
			{
				Close_LED2;
				if(0 == OPEN_EN_17xx)
					Open_LED3;
				else
					not_LED3;
			}
		}
		else
		{
			Open_LED2;
			Open_LED3;
		}
  }
	
	if(Time0_>=200)	 
    Time0_=0x00;
	
	ovt++;
	if(ovt>80)
	 ovt=0;
}

unsigned char UARTn(unsigned char *r_data ,unsigned char BJXY , unsigned char uartx)
{
  uint i,j;
  uchar m,n;
  uchar r_Hadd;
  r_Hadd=r_data[2];
  F0=0;
  if(0x1e == BJXY)
  {
    i=8;
		if(2 == uartx)
		{
			Send_string1(r_data,((uint)(r_data[6])<<8)+r_data[7]+10);
		}
    switch (r_data[4])
    {           //1e 60 90 60 01 90 00 00 1f 1f
      case 0x01://1E 60 05 01 01 05 00 00 7e 1fӲ���汾,����汾
      {
        r_data[i++]=0x09,r_data[i++]=0x00,r_data[i++]=0x04;
        r_data[i++]='V',r_data[i++]='e',r_data[i++]='r';
        
        r_data[i++]=YJBan3_Ben3;//Ӳ���汾
				if(r_Hadd==C_add&&r_data[3]==0xff)
				{
          r_data[i++]='M',r_data[i++]='C',r_data[i++]='U',r_data[i++]='_',r_data[i++]='I',r_data[i++]='D';
					for(m=0;m<7;m++)
				  {
            r_data[i++]=MCU_ID[m];
				  }
				}
				r_data[10]=i-11;
        r_data[i++]=0x0A,r_data[i++]=0x00,r_data[i++]=0x05;
        r_data[i++]=RJBan3_Ben31;r_data[i++]=RJBan3_Ben32;r_data[i++]=RJBan3_Ben33;r_data[i++]=RJBan3_Ben34;r_data[i++]=RJBan3_Ben35;//����汾

	      switch(uartx)
        {
         #if UART1==1
          case 1:
            UART1_rs[finish01]=1;
          break;
         #endif
          
         #if UART2==1
          case 2:
            UART2_rs[finish01]=1;
          break;
         #endif
         
         #if UART3==1
          case 3:
            UART3_rs[finish01]=1;
          break;
         #endif
         
         #if UART4==1
          case 4:
            UART4_rs[finish01]=1;
          break;
         #endif
         
         default:return 2;
        }
				if(r_Hadd==0xff&&r_data[3]==0x01)
				{
          r_data[2]=r_data[3];
					r_data[3]=0x10;
					r_data[4]=0x05;//Ŀ�ĵ�ַ��Դ��ַ�Ե�,��Ϣ֡Ϊ0x05
		      r_data[6]=0x00/*(i-8)>>8*/,r_data[7]=i-8;//д�����ݳ���
		      r_data[i++]=X_or(r_data,i/*((uint)(r_data[6])<<8)+r_data[7]+8*/);//�������ֵ,
					r_data[i++]=0x1f;//д���β
		      switch(uartx)
          {
           #if UART1==1
            case 1:
              Send_string1(r_data,i);//����Ӧ��
            break;
           #endif
            
           #if UART2==1
            case 2:
              Send_string2(r_data,i);//����Ӧ��
            break;
           #endif
           
           #if UART3==1
            case 3:
              Send_string3(r_data,i);//����Ӧ��
            break;
           #endif
           
           #if UART4==1
            case 4:
              Send_string4(r_data,i);//����Ӧ��
            break;
           #endif
           
           default:return 2;
          }
					return 1;
				}
      }
        break;
			
      case 0x05://��Ϣ�ظ�
			{
				//00 01 02 03 04 05 06 07 08 09 10 11 12 13 14
				//1e 60 05 03 05 05 00 04 0c 00 01 01 xor 1f
				//1E 60 05 03 05 03 00 04 0C 00 01 01 76 1F
				//1e 60 05 03 05 05 00 00 xor 1f
				//1E 60 05 01 05 05 00 00 7a 1f
				//
				if(/**/r_data[2]==S_Yadd&&//�ϴη��͵�Դ��ַ,�˴��յ���Ŀ���ַ
					 r_data[3]==S_Madd&&//�ϴη��͵�Ŀ���ַ,�˴��յ���Դ��ַ
					 r_data[5]==S_LSH&&//��ˮ��
					 /**/
					 (
					 (r_data[6]==0x00&&r_data[7]==0x00)||//�򻯻ظ��ɹ�
					 (r_data[6]==0x00&&r_data[7]==0x04&&r_data[8]==0x0c&&r_data[9]==0x00&&r_data[10]==0x01&&r_data[11]==0x01)||//��׼�ظ��ɹ�
					 (r_data[8]==0x09&&r_data[8+r_data[10]+3]==0x0a)||//Ӳ���汾,����汾
					 0
					 )&&
					 1
					)
				{
					switch(uartx)
					{
					 #if UART1==1
						case 1:
							UART1_rs[finish05]=1;
						break;
					 #endif
						
					 #if UART2==1
						case 2:
							UART2_rs[finish05]=1;
							_1a_flag=0;
						break;
					 #endif
					 
					 #if UART3==1
						case 3:
							UART3_rs[finish05]=1;
						break;
					 #endif
					 
					 #if UART4==1
						case 4:
							UART4_rs[finish05]=1;
						break;
					 #endif
					 
					 default:return 2;
					}
				}
				return 1;
				//r_data[2]=0xFF;
			}
        break;
			
      case 0x06://��Ϣ֡,��������
			{
				//1e 60 30 01 06 30 00 03 00 00 00 7A 1f
				//1E 60 10 01 06 01 00 3C 11 00 39 00 00 01 02 05 41 32 31 C2 B7 B9 AB BD BB B3 B5 B3 B5 CC E5 B9 E3 B8 E6 20 20 20 20 20 D5 D0 D7 E2 B5 E7 BB B0 A3 BA 20 20 20 20 20 20 20 31 33 35 32 35 37 32 34 31 31 38 3D 1F
				if(0x00 == r_data[8]) //��������,��Դ��ַ�������������Ǻ�
				{
					if(r_data[5]==C_add&&r_data[6]==0x00&&r_data[7]==0x03&&r_data[9]==0x00&&r_data[10]==0x00)
						F0=1;
					else
						F0=0;
				}
//00 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15 16 17 18 19 20
//1e 60 0a 01 06 01 00 09 6f 00 06 d6 b1 bd d3 ba d0 71 1f //ֱ�Ӻ�
//1e 60 0a 01 06 01 00 09 6f 00 06 c0 b6 d1 c0 d6 dc 00 1f //������
				else if('o' == r_data[8])//open_box_cmd �����в�����0x00��0xff
				{
					E2prom(Sector_Erase,open_box_cmd_add);
					for(j=0;j<(((uint)(r_data[9])<<8)+r_data[10])%33;j++)
					{
						open_box_cmd[j]=ISP_DATA=r_data[11+j];
						E2prom(Byte_Write,open_box_cmd_add+j);
					}
					ISP_DATA='\0';
					E2prom(Byte_Write,open_box_cmd_add+j);
				}
			}
        break;

			case 0x08:
			{
				if(0x3d == r_data[8])
				{
/**
00 01 02 03 04 05 06 07 08 09 10 11 12 13
1E 60 0A 01 08 06 00 04 3d 00 01 00 43 1F//�ر�����������
1E 60 0A 01 08 06 00 04 3d 00 01 01 42 1F//:��������������
1.	��������������Ĭ��Ϊ�ر�״̬
2.	������ÿһ�����Ӻ�,ƽ�巢�Ϳ�������
3.	����������������Զ��ر�
4.	ƽ���ϵ��ύ���ݳɹ����ٴη��Ϳ�������
5.	�����Ͽ����Ӻ�,�������Զ���Ϊ�ر�״̬
**/
					Send_char1(r_data[8]);
					Send_char1(r_data[11]);
					Send_char1(_1a_flag_17xx);
					Send_char1(_1a_flag_11xx);
					if(0 == r_data[11])
					{
						if(1 == _1a_flag_17xx)
						{
							_1a_flag_17xx=0;
							OPEN_EN_17xx=r_data[11];
						}
						if(1 == _1a_flag_11xx)
						{
							_1a_flag_11xx=0;
							OPEN_EN_11xx=r_data[11];
						}
					}
					else
					{
						OPEN_EN_17xx=r_data[11];
						OPEN_EN_11xx=r_data[11];
					}
				}
				else if(0x3f == r_data[8])
				{
/**
1E 60 0A 01 08 06 00 04 3f 00 01 00 40 1F//�㳮Ա
1E 60 0A 01 08 06 00 04 3f 00 01 01 40 1F//����Ա
**/
					User_type=r_data[11];
					if(0 == User_type)
						Send_string1("\n�㳮Ա\n",0);
					else if(1 == User_type)
						Send_string1("\n����Ա\n",0);
				}
			}
				break;

			default:
			{
				r_data[i++]=0x0c;//Ӧ������֡
				r_data[i++]=0x00,r_data[i++]=0x01;//����
				r_data[i++]=0x03;//�����ط�
			}
				break;
    }
    if(r_data[2]==C_add||(r_Hadd!=0xff&&r_data[3]==0x01))//C_add)//�ж��Ƿ�Ӧ��
    {
      r_data[2]=r_data[3];
			r_data[3]=r_Hadd;
			r_data[4]=0x05;//Ŀ�ĵ�ַ��Դ��ַ�Ե�,��Ϣ֡Ϊ0x05
      r_data[6]=0x00/*(i-8)>>8*/,r_data[7]=i-8;//д�����ݳ���
      r_data[i++]=X_or(r_data,i/*((uint)(r_data[6])<<8)+r_data[7]+8*/);
			r_data[i++]=0x1f;//�������ֵ,д���β
      switch(uartx)
      {
       #if UART1==1
        case 1:
          Send_string1(r_data,i);//����Ӧ��
        break;
       #endif
        
       #if UART2==1
        case 2:
					Send_string1(r_data,i);
          Send_string2(r_data,i);//����Ӧ��
        break;
       #endif
       
       #if UART3==1
        case 3:
          Send_string3(r_data,i);//����Ӧ��
        break;
       #endif
       
       #if UART4==1
        case 4:
          Send_string4(r_data,i);//����Ӧ��
        break;
       #endif
       
       default:return 2;
      }
#ifndef MCU_AT89Sxx
      if(F0)
      {
        Time0_=0;//
        while(Time0_<10);
        ISP_CONTR=0x60;//��λ��ISP��
      }
#endif
      return r_data[i-2];
    }
    else
    {
      if(i>8)
        return r_data[i-1];
      else 
        return 1;
    }
  }
  else
	{
		return 0;
	}
}
void rest_FM17xx(void)
{
	NPD=0;//1.1/1.2
	P16=0;//1.0
	Delay(100);
	NPD=1;
	P16=1;
	Delay(100);
}

void Main(void)
{
//	unsigned int i,j;
//	uchar m;
//	uchar idata *ipt=0xf1;
//	uchar code *cpt=0xbff9;
//	bit FM17xx=0;
//	bit FM11xx=0;
//	uchar FM17_Err=0;
//	uchar FM17_Err2=0;
//	uchar FM11_Err=0;
//	uchar FM11_War=0;
//	uchar Send_buf[100];
//	uchar open_box_old_cmd[32]= "������";
//	uchar open_box_old_cmd[32]= "ֱ�Ӻ�";
//	uint crc;
	
	Io_Init();
	Variable_Init();
//	SPI_Init();
	Uart1_Init();
	Send_string1("\n����ʱ��2019��3��26��\n",0);

	PcdInit();
	PcdAntennaOff(); 
	PcdAntennaOn(); 
//	Uart2_Init();
//	Timer0Init();
//	FM11_Init();
//	BT_Init();
	
	WDT_rest();
	
  	while (1) {
		WDT_rest();
		Uart_App();
	
	}
	return;
}


/*****************************/
/*�������ܣ�	�ܽų�ʼ������
/*���������	��
/*���������	��
/*����ֵ��		��
/*ռ����Դ��	��
/*****************************/
void Io_Init()
{
	P0 = P1 = P2 = P3 = 0xFF;
	P0M1=0x00	;
	P0M0=0x00	;
	P1M1=0x00	;
	P1M0=0x00	;
	P2M1=0x00	;
	P2M0=0x00	;
	P3M1=0x00	;
	P3M0=0x00	;
	P4M1=0x00	;
	P4M0=0x00	;
	
	P0M0 |= (1<<2);//P0^2;//42Pin
	REST_BT=0;
	
	P0M0 |= (1<<4);//P0^4;//44Pin
	AT_BT=1;
	
	P1M0 |= (1<<2);//P1.2
	SPI_CS = 1;
	
	P1M0 |= (1<<3);
	P1M0 |= (1<<4);
	P1M0 |= (1<<5);
	
	P1M0 |= (1<<6);
	P16=1;
	
	P3M0 |= (1<<3);
	NPD=1;//P3.3
	
	LED = 0;// P2^5;
	P2M0 |= (1<<5);
	
	LED2 = 0;// P2^6;
	P2M0 |= (1<<6);
	
	LED3 = 0;// P2^7;
	P2M0 |= (1<<7);
	
	CON1=0;// P1^7;
	P1M0 |= (1<<7);
	
}
/*************************/
/*�������ܣ�	��ʱ����
/*���������	delay_time
/*���������	��
/*����ֵ��		��
/*ռ����Դ��	��
/*************************/
void Delay_100us(unsigned int delay_time)		//0.1ms*delay_time
{
	unsigned int i;
	for(i=0;i<delay_time;i++)
	{
		_nop_(); _nop_(); _nop_(); _nop_(); _nop_(); _nop_(); _nop_(); _nop_(); _nop_(); _nop_(); 
		_nop_(); _nop_(); _nop_(); _nop_(); _nop_(); _nop_(); _nop_(); _nop_(); _nop_(); _nop_();
		_nop_(); _nop_(); _nop_(); _nop_(); _nop_(); _nop_(); _nop_(); _nop_(); _nop_(); _nop_();
		_nop_(); _nop_(); _nop_(); _nop_();	_nop_(); _nop_(); _nop_(); _nop_(); _nop_(); _nop_();
		_nop_(); _nop_(); _nop_(); _nop_();	_nop_(); _nop_(); _nop_(); _nop_();	_nop_(); _nop_();
	}
}

void Variable_Init(void)
{
	send_buff.buff=uart_data;
	rece_buff.buff=uart_data+128;
	uart_status.rece_finish=False;
	uart_status.send_finish=False;
	uart_status.rece_error=False;
	uart_status.send_error=False;
}


