

#include "STC15F2K60S2.h"
#include "intrins.h"
#include "EEPROM.h"

void E2prom(uchar isp_iap_C,uint Address)	//���ڲ�E2prom����stc89c51��ַ��0X1000~0x1FFF 4K,stc89c52��ַ��0X2000~0x33FF 5K,stc89c54��ַ��0X4000~0xF3FF 45K
                                          //11F16XE 0x0000~b3ff 45K
											//ÿ����512�ֽ�,����ֻ����128�ֽ�	
{

	ISP_ADDRH=(uchar)(Address>>8);
	ISP_ADDRL=(uchar)(Address);
	ISP_CONTR=wait_time|0x80;
	ISP_CMD=isp_iap_C;	 //
	ISP_TRIG=isp_trig1;
	ISP_TRIG=isp_trig2;
	_nop_();
	ISP_CONTR=ISP_TRIG=ISP_CMD=0x00; //���isp_iap����,�Է������
	ISP_ADDRH=ISP_ADDRL=0xFF;
//	EA=1;				 //���ж�
}
