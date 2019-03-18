
#include "STC15F2K60S2.h"
#include "BT.h"
#include "uart.h"
#include "main.h"

#define upper_lower_EN 0  //0 ������ĸ��Сд   1 ��������ĸ��Сд
uchar strsearch(uchar *ptr2 ,uchar *ptr1_at ,uint Len)//���ַ���*ptr2��*ptr1�е�λ��
{
//*ptr2ΪĿ���ַ���,������'\0'����
//*ptr1ΪԴ�ַ�,����ΪLen
//����:  0  û���ҵ�
//1-255 �ӵ�N���ַ���ʼ��ͬ
	uint i,j,k;
	uchar Data;
	if(ptr2[0]==0) return(0);
	for(i=0,j=0;i<Len;i++)
	{
		Data=ptr2[j];
	#if 1 == upper_lower_EN
		if(ptr1_at[i]!=Data)
		{
			if(Data>='a'&&Data<='z')Data=Data-'a'+'A';
			else if(Data>='A'&&Data<='Z')Data=Data-'A'+'a';
		}
	#endif
		if(ptr1_at[i] == Data)
		{//��һ���ַ���ͬ
			for(k=i;k<Len;k++,j++)
			{
				Data=ptr2[j];
				if(Data == '\0')//�Ƚ���ȷ
					return(i+1);//����ֵ������������0
			#if 1 == upper_lower_EN
				if(ptr1_at[k]!=Data)
				{
					if(Data>='a'&&Data<='z')Data=Data-'a'+'A';
					else if(Data>='A'&&Data<='Z')Data=Data-'A'+'a';
				}
			#endif
				if(ptr1_at[k]!=Data) break;
			}
			j=0;
		}
	}
	return(0);
}

void BT_Init(void)
{
	uchar xdata BT_Name[]="AT+NAMERF-BLE4.0\n";
	
	REST_BT=0;
	Time0_=0;
	while(Time0_<5);
	REST_BT=1;
	Time0_=0;
	while(Time0_<5);
	
	AT_BT=0;
	Time0_=0;
	while(Time0_<5);
	UART2_rs[RH_ID]=0;
	UART2_rs[Time0_COM]=0;
	receive_number2=0;
	receive_data2[0]='\0';
	Send_string1("\nAT+VER\n",0);
	Send_string2("AT+VER\n",0);
	Time0_=0;
	while(Time0_<5);//�˴����ɴ��ڴ��ڳ�ʱʱ��
	UART2_rs[RH_ID]=0;
	Send_string1(&receive_data2[0],receive_number2);
	receive_number2=0;
	Send_char1('\r');Send_char1('\n');
	
	UART2_rs[RH_ID]=0;
	UART2_rs[Time0_COM]=0;
	receive_number2=0;
	receive_data2[0]='\0';
	Send_string1("\nAT+NAME\n",0);
	Send_string2("AT+NAME\n",0);
	Time0_=0;
	while(strsearch("RF-BLE4.0",receive_data2,receive_number2) == 0)if(Time0_>15)break;
	//+NAME:RF-BLE4.0
	UART2_rs[RH_ID]=0;
	Send_string1(&receive_data2[0],receive_number2);
	receive_number2=0;
	if(Time0_>15)
	{		
		UART2_rs[RH_ID]=0;
		UART2_rs[Time0_COM]=0;
		receive_number2=0;
		receive_data2[0]='\0';
		Send_char1('\r');Send_char1('\n');
		Send_string1(BT_Name,0);
		Send_string2(BT_Name,0);
		Time0_=0;
		while(strsearch("OK",receive_data2,receive_number2) == 0)if(Time0_>15)break;
		UART2_rs[RH_ID]=0;
		Send_string1(&receive_data2[0],receive_number2);
		receive_number2=0;
	}
	
	UART2_rs[RH_ID]=0;
	UART2_rs[Time0_COM]=0;
	receive_number2=0;
	receive_data2[0]='\0';
	Send_string1("\nAT+MAC\n",0);
	Send_string2("AT+MAC\n",0);
	Time0_=0;
	while(Time0_<5);//�˴����ɴ��ڴ��ڳ�ʱʱ��
	UART2_rs[RH_ID]=0;
	Send_string1(&receive_data2[0],receive_number2);
	receive_number2=0;
	
	UART2_rs[Time0_COM]=0;
	AT_BT=1;
}
