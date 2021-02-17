#ifndef __DS1302_H
#define	__DS1302_H


#define   DS_SEC  0x80        //秒
#define   DS_MIN  0x82        //分
#define   DS_HOUR 0x84        //时
#define   DS_DATE 0x86        //日
#define   DS_MON  0x88        //月
#define   DS_YEAR 0x8c        //年
#define   DS_WEEK 0x8a        //周
#define   DS_WRPT 0x8e        //写保护

 
#define   DS_RST(x)   		GPIOB->ODR=(GPIOB->ODR&~GPIO_Pin_5) | (x ? GPIO_Pin_5 : 0)
#define   DS_SCLK(x)		  GPIOB->ODR=(GPIOB->ODR&~GPIO_Pin_3) | (x ? GPIO_Pin_3 : 0)
#define   DS_WDATA(x)		  GPIOB->ODR=(GPIOB->ODR&~GPIO_Pin_4) | (x ? GPIO_Pin_4 : 0)
#define   DS_READ		      GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_4) 

#define   DS_YEAR_START  2000

typedef enum{
	DS_TIME_SEC = 0,
	DS_TIME_MIN = 1,
	DS_TIME_HOUR = 2,
	DS_TIME_DAY = 3,
	DS_TIME_MON = 4,
	DS_TIME_YEAR = 6
}ds_time_t;

typedef struct
{
	U16 Year;
	U8 ds_time[8];
}ds_set_time_t;

void DS1302_Init(void);
void DS1302_WrByte(U8 dat);
U8 DS1302_RdByte(void);
void DS1302_Write(U8 add,U8 dat);
U8 DS1302_Read(U8 add);
void DS1302_SetTime(U8 year, U8 month,U8 date,U8 hour,U8 min,U8 sec,U8 week);
void DS1302_GetTime(void);
void IO_DS1302In(void);
void IO_DS1302Out(void);

#endif
