#ifndef __FLASH_H
#define __FLASH_H

//�궨��
#define EEPROM_START_ARRR 0x08007C00   //��ʼ��ַ
#define DEVICE_DATA_LEN   0x04         //�������ݳ���



//���ܺ���
void FLASH_ReadNByte(U8 *pBuffer,U32 readAddr,U16 len);
void Flash_WriteCmd(U32 addr,U16 *pBuf,U16 len);

#endif
