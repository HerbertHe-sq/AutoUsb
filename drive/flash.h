#ifndef __FLASH_H
#define __FLASH_H

//宏定义
#define EEPROM_START_ARRR 0x08007C00   //起始地址
#define DEVICE_DATA_LEN   0x04         //有用数据长度



//功能函数
void FLASH_ReadNByte(U8 *pBuffer,U32 readAddr,U16 len);
void Flash_WriteCmd(U32 addr,U16 *pBuf,U16 len);

#endif
