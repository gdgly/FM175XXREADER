
#ifndef __RC522_H
#define __RC522_H


#define s8  char
#define u8 unsigned char
#define u16 unsigned int


sbit     NSS  = P1^2;			
sbit     SCK  = P1^5;
sbit     MOSI = P1^3;
sbit     MISO = P1^4;
sbit     MFRST= P3^3;

 
//ͨѶ������󳤶�
#define MAXRLEN 					64

//MF522������
#define PCD_IDLE              		0x00               //ȡ����ǰ����
#define PCD_AUTHENT           		0x0E               //��֤��Կ
#define PCD_RECEIVE           		0x08               //��������
#define PCD_TRANSMIT          		0x04               //��������
#define PCD_TRANSCEIVE        		0x0C               //���Ͳ���������
#define PCD_RESETPHASE        		0x0F               //��λ
#define PCD_CALCCRC           		0x03               //CRC����

//Mifare_One��Ƭ������
#define PICC_REQIDL           		0x26               //Ѱ��������δ��������״̬
#define PICC_REQALL           		0x52               //Ѱ��������ȫ����
#define PICC_ANTICOLL1        		0x93               //����ײ
#define PICC_ANTICOLL2        		0x95               //����ײ
#define PICC_AUTHENT1A        		0x60               //��֤A��Կ
#define PICC_AUTHENT1B        		0x61               //��֤B��Կ
#define PICC_READ             		0x30               //����
#define PICC_WRITE            		0xA0               //д��
#define PICC_DECREMENT        		0xC0               //�ۿ�
#define PICC_INCREMENT        		0xC1               //��ֵ
#define PICC_RESTORE          		0xC2               //�������ݵ�������
#define PICC_TRANSFER         		0xB0               //���滺����������
#define PICC_HALT             		0x50               //����

//MF522 FIFO���ȶ���
#define DEF_FIFO_LENGTH       		64                 //FIFO size=64byte

//MF522�Ĵ�������
// PAGE 0
#define     RFU00                 	0x00    //����
#define     CommandReg            	0x01    //������ֹͣ�����ִ��
#define     ComIEnReg             	0x02    //�ж����󴫵ݵ�ʹ�ܺͽ��ܿ���λ
#define     DivlEnReg             	0x03    //�ж����󴫵ݵ�ʹ�ܺͽ��ܿ���λ
#define     ComIrqReg             	0x04    //�����ж������־
#define     DivIrqReg             	0x05	//�����ж������־
#define     ErrorReg              	0x06    //�����־,ָʾִ�е��ϸ�����Ĵ���״̬
#define     Status1Reg            	0x07    //����ͨ�ŵ�״̬��־
#define     Status2Reg            	0x08    //�����������ͷ�������״̬��־
#define     FIFODataReg           	0x09    //64�ֽ�FIFO����������������
#define     FIFOLevelReg          	0x0A	//ָʾFIFO�д洢���ֽ���
#define     WaterLevelReg         	0x0B    //����FIFO��������籨����FIFO���
#define     ControlReg            	0x0C    //��ͬ�Ŀ��ƼĴ���
#define     BitFramingReg         	0x0D    //����λ��֡�ĵ���
#define     CollReg               	0x0E    //RF�ӿ��ϼ�⵽�ĵ�һ��λ��ͻ��λ��λ��
#define     RFU0F                 	0x0F    //����
// PAGE 1     
#define     RFU10                 	0x10    //����
#define     ModeReg               	0x11    //���巢�ͺͽ��յĳ���ģʽ
#define     TxModeReg             	0x12    //���巢�͹��̵����ݴ�������
#define     RxModeReg             	0x13    //������չ����е����ݴ�������
#define     TxControlReg          	0x14    //���������������ܽ�TX1��TX2���߼�����
#define     TxAutoReg             	0x15    //��������������������
#define     TxSelReg              	0x16    //ѡ������������������
#define     RxSelReg              	0x17    //ѡ���ڲ��Ľ���������
#define     RxThresholdReg        	0x18    //ѡ��λ����������ֵ
#define     DemodReg              	0x19    //��������������
#define     RFU1A                 	0x1A    //����
#define     RFU1B                 	0x1B    //����
#define     MifareReg             	0x1C    //����ISO 14443/MIFAREģʽ��106kbit/s��ͨ��
#define     RFU1D                 	0x1D    //����
#define     RFU1E                 	0x1E    //����
#define     SerialSpeedReg        	0x1F    //ѡ����UART�ӿڵ�����
// PAGE 2    
#define     RFU20                 	0x20    //����
#define     CRCResultRegM         	0x21    //��ʾCRC�����ʵ��MSBֵ
#define     CRCResultRegL         	0x22    //��ʾCRC�����ʵ��LSBֵ
#define     RFU23                 	0x23    //����
#define     ModWidthReg           	0x24    //����MODWIDTH������
#define     RFU25                 	0x25    //����
#define     RFCfgReg              	0x26    //���ý���������
#define     GsNReg                	0x27    //ѡ�������������ܽ�TX1��TX2�ĵ��Ƶ絼
#define     CWGsCfgReg            	0x28    //ѡ�������������ܽ�TX1��TX2�ĵ��Ƶ絼
#define     ModGsCfgReg           	0x29	  //ѡ�������������ܽ�TX1��TX2�ĵ��Ƶ絼
#define     TModeReg              	0x2A    //�����ڲ���ʱ��������
#define     TPrescalerReg         	0x2B    //�����ڲ���ʱ��������
#define     TReloadRegH           	0x2C    //����16λ���Ķ�ʱ����װֵ
#define     TReloadRegL           	0x2D    //����16λ���Ķ�ʱ����װֵ
#define     TCounterValueRegH     	0x2E    //��ʾ16λ����ʵ�ʶ�ʱ��ֵ
#define     TCounterValueRegL     	0x2F    //��ʾ16λ����ʵ�ʶ�ʱ��ֵ
// PAGE 3      
#define     RFU30                 	0x30    //����
#define     TestSel1Reg           	0x31    //���ò����źŵ�����
#define     TestSel2Reg           	0x32    //���ò����źŵ����ú�PRBS����
#define     TestPinEnReg          	0x33    //D1-D7�����������ʹ�ܹܽ�(ע:�����ڴ��нӿ�)
#define     TestPinValueReg       	0x34    //����D1-D7����I/O����ʱ��ֵ
#define     TestBusReg            	0x35    //��ʾ�ڲ��������ߵ�״̬
#define     AutoTestReg           	0x36    //���������Բ���
#define     VersionReg            	0x37    //��ʾ�汾
#define     AnalogTestReg         	0x38    //���ƹܽ�AUX1��AUX2
#define     TestDAC1Reg           	0x39    //����TestDAC1�Ĳ���ֵ
#define     TestDAC2Reg           	0x3A    //����TestDAC2�Ĳ���ֵ
#define     TestADCReg            	0x3B    //��ʾADCI ��Qͨ����ʵ��ֵ
#define     RFU3C                 	0x3C    //����
#define     RFU3D                 	0x3D    //����
#define     RFU3E                 	0x3E    //����
#define     RFU3F		          	0x3F    //����

//��MF522ͨѶʱ���صĴ������
#define 	MI_OK					(0)		  //���سɹ�
#define 	MI_NOTAGERR				(-1)	  //����Ŀ�����
#define 	MI_ERR					(-2)	  //���ش���

//�ļ���·������ʽ
#define		MOD_NOPROTECT			0x00	//��ʹ����·����
#define		MOD_CRYPT				0xC0	//��·�����Ҽ���

//CPU��ͨѶ����״̬��
#define		ST_OK					0x9000	//��ȷִ��
#define		ST_ERR					0xFFFF	//���ش���
#define		ST_RETDATWRG			0x6281	//�������ݴ���
#define		ST_VERIFY				0x6283	//ѡ���ļ���Ч,�ļ�����ԿУ�����
#define		ST_STNCHG				0x6400	//״̬��־δ�ı�
#define		ST_EEPWRG				0x6581	//дEEPROM���ɹ�
#define		ST_LENWRG				0x6700	//���󳤶�
#define		ST_CLAPTWRG				0x6900	//CLA����·����Ҫ��ƥ��
#define		ST_INVALID				0x6901	//��Ч��״̬
#define		ST_COMMAND				0x6981	//�������ļ��ṹ������
#define		ST_SECURE				0x6982	//�����㰲ȫ״̬
#define		ST_KEYLOCK				0x6983	//��Կ����
#define		ST_USECON				0x6985	//ʹ������������
#define		ST_NSPACK				0x6987	//�ް�ȫ����
#define		ST_SPDATWRG				0x6988	//��ȫ�����������ȷ
#define		ST_DATPRWRG				0x6A80	//�������������
#define		ST_FUNWRG				0x6A81	//���ܲ�֧�ֻ�����MF��Ƭ����
#define		ST_NOFILE				0x6A82	//�ļ�δ�ҵ�
#define		ST_NORECORD				0x6A83	//��¼δ�ҵ�
#define		ST_NOSPACE				0x6A84	//�ļ����㹻�ռ�
#define		ST_P1P2WRG				0x6A86	//����P1,P2����,�ļ��Ѵ���
#define		ST_NOKEY				0x6A88	//��Կδ�ҵ�
#define		ST_OFFSETWRG			0x6B00	//�ڴﵽLe/Lc�ֽ�֮ǰ�ļ�����,ƫ��������
#define		ST_CLAWRG				0x6E00	//��Ч��CLA
#define		ST_DATWRG				0x6F00	//������Ч
#define		ST_MACWRG				0x9302	//MAC����
#define		ST_APPLOCK				0x9303	//Ӧ���ѱ�����
#define		ST_SUMLACK				0x9401	//����
#define		ST_NOKEY2				0x9403	//��Կδ�ҵ�
#define		ST_MACINVAL			    0x9406	//�����MAC������

//��Կ����
#define		INTERNALKEY			0x34	//�ڲ���֤��Կ
#define		PROTECTKKEY			0x36	//�ļ���·������Կ
#define		EXTERNALKEY			0x39	//�ⲿ��֤��Կ
/*
extern u8 ReadACard;			//��������־
extern u8 fcnt;					//��������LED��Ӧʱ��
extern u8 Key[6];				//��Կ
   */
void PcdInit(void); 
void PcdSwitchPCB(void);
s8 PcdConfigISOType(u8 type);//
void WriteRawRC(u8 Address, u8 value);//
u8 ReadRawRC(u8 Address);
void SetBitMask(u8 reg,u8 mask);
void ClearBitMask(u8 reg,u8 mask);
void PcdAntennaOff(void);
void PcdAntennaOn(void);
s8 PcdComCmd(u8 Command, u8 *pDataIn, u8 InLenByte, u8 *pDataOut, u16  *pOutLenBit);
s8 PcdReset(void);
void CalulateCRC(u8 *pIndata,u8 len,u8 *pDataOut);
s8 PcdHalt(void);
s8 PcdAnticoll(u8 *pSnr);
s8 PcdRequest(u8 req_code,u8 *pTagType);
s8 PcdRats(u8 * DataOut,u8 * Len);
u16 PcdEraseDF(void);
s8 PcdEraseCheck(u8* pDataOut,u8 *Len);
u16 PcdCreateFile(u8 *fileid, u8 Lc, u8 *pDataIn);
u16 PcdGetChallenge(u8* pRan);
u16 PcdExAuth(u8 keyflag,u8 *pRan);
u16 PcdSelectFile(u8* pDataIn, u8* pDataOut,u8 * Len);
u16 PcdReadBinary(u8 offset, u8 Len, u8* pDataOut);
u16 PcdUpdateBinary(u8 offset, u8 Len, u8* pDataIn);
u16 PcdSetKey(u8 Keysign, u8 Len, u8* pDataIn);
u16 GetCard(u8 Reqcode, u8* pTagType, u8* pSnr);
u16 CardReset(u8 * Data_Out,u8 *  Len);
void Request_loop();
u8 Pcd_Cmd(u8* pDataIn, u8  In_Len, u8* pDataOut,u8 * Out_Len);
s8 PcdEraseCheck(u8* pDataOut,u8 *Len);
#endif


