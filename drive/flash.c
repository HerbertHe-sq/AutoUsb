#include "includes.h"


// *****************************************************************************
// �������ƣ�FLASH_ReadNByte
// ������������ȡ��Ӧ�����ֽ�����
// �������: /
// �������: /
// ���ز���: /
// *****************************************************************************
void FLASH_ReadNByte(U8 *pBuffer,U32 readAddr,U16 len)
{
	U32 i,flash_tag_addr;
	FLASH_Unlock();
	
	flash_tag_addr = EEPROM_START_ARRR+readAddr;
	for(i = 0;i<len;i++)
	{
		*pBuffer++=*(VU8*)flash_tag_addr++;
	}
	
	FLASH_Lock();
}

// *****************************************************************************
// �������ƣ�Flash_WriteCmd
// ����������д����
// �������: /
// �������: /
// ���ز���: /
// *****************************************************************************
void Flash_WriteCmd(U32 addr,U16 *pBuf,U16 len)
{
	volatile FLASH_Status FLASHStatus;
  U32 *DataBuf;
	
	DataBuf = (U32*)pBuf;
	
	FLASH_Unlock();        //����
	FLASHStatus = FLASH_COMPLETE;  
	FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPERR);
  FLASHStatus = FLASH_ErasePage(EEPROM_START_ARRR);
	while(len--)
	{
		FLASH_ProgramWord(addr, *DataBuf);//��ָ����ַ��д����
		if(FLASH_GetStatus()==FLASH_COMPLETE)  //��ȡFlash״̬
		{
			addr += 4;
			DataBuf++;
		}
		else
		{
			goto ENDRUN;
		}
	}
   

ENDRUN:
	FLASH_Lock();															
}



