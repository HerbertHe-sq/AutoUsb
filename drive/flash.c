#include "includes.h"


// *****************************************************************************
// 函数名称：FLASH_ReadNByte
// 功能描述：读取相应长度字节数据
// 输入参数: /
// 输出参数: /
// 返回参量: /
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
// 函数名称：Flash_WriteCmd
// 功能描述：写命令
// 输入参数: /
// 输出参数: /
// 返回参量: /
// *****************************************************************************
void Flash_WriteCmd(U32 addr,U16 *pBuf,U16 len)
{
	volatile FLASH_Status FLASHStatus;
  U32 *DataBuf;
	
	DataBuf = (U32*)pBuf;
	
	FLASH_Unlock();        //解锁
	FLASHStatus = FLASH_COMPLETE;  
	FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPERR);
  FLASHStatus = FLASH_ErasePage(EEPROM_START_ARRR);
	while(len--)
	{
		FLASH_ProgramWord(addr, *DataBuf);//在指定地址编写半字
		if(FLASH_GetStatus()==FLASH_COMPLETE)  //获取Flash状态
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



