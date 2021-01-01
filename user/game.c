#include "includes.h"
#include "map.h"

//�ڲ�����
SNAKE_BODY* Sanke_Body;   //��������
SNAKE_PARAM Snake_Param={1,4,0,0,0,1};  //�߲��� //������  //�����־λ-Ĭ������  //ˢ�±�־λ   //��Ϸ���   //�Ƿ�    
U8 Sanke_Fool_Buf[SNAKE_FOOL_ITEM_NUM]; //ʳ������

//�ⲿ����
extern SYSTEMSTRUCT SysParam;

// *****************************************************************************
// �������ƣ�Sanke_Init
// ������������ʼ��-̰����
// �������: /
// �������: /
// ���ز���: /
// *****************************************************************************
void Sanke_Init(void)
{	
	//�ͷſռ�
	free(Sanke_Body);	
	
	//����ռ�
	Sanke_Body = (SNAKE_BODY *)malloc(SNAKE_STRUCT_LEN);
	Sanke_Body->snake_next = NULL;
	Sanke_Body->Snake_X = 1; 
	Sanke_Body->Snake_Y = 3;

	Snake_Param.Sanke_Len = 1;     //��ʼ������
	Snake_Param.Direct_Flag = SNAKE_DIR_RIGHT;   //��ʼ������
	
	//��ʼ��ʳ��
	Sanke_Fool_Buf[SNAKE_FOOL_LOCAL_X] = 8;
	Sanke_Fool_Buf[SNAKE_FOOL_LOCAL_Y] = 2;	
	
	//����ת��
	Convert_StructMap(*Sanke_Body,Snake_Param.Sanke_Len);
	
	//��ͼ
	Update_Map();
	
	Snake_Param.Snake_RefleshFg = SNAKE_REFLESH_ON;  //��ʼ��Ϸ
	Snake_Param.Game_Result = SNAKE_GAME_RUNING;     //���
	Snake_Param.Game_Score = 0;                      //����
}

// *****************************************************************************
// �������ƣ�Convert_StructMap
// �����������޸Ķ�Ӧ����
// �������: /
// �������: /
// ���ز���: /
// *****************************************************************************
void Convert_StructMap(SNAKE_BODY body_temp,U8 len)
{
	U8 i = 0;
	U8 *p_map;
	SNAKE_BODY struct_temp = body_temp;
	
	//����ʳ��
	if(Sanke_Fool_Buf[SNAKE_FOOL_LOCAL_X]!=0 && Sanke_Fool_Buf[SNAKE_FOOL_LOCAL_Y]!=0)
	{
		p_map = &Game_Map[(Sanke_Fool_Buf[SNAKE_FOOL_LOCAL_Y]-1)][(Sanke_Fool_Buf[SNAKE_FOOL_LOCAL_X]-1)*8];
		*p_map++=0x3f;
		*p_map++=0x33;
		*p_map++=0x21;
		*p_map++=0x21;
		*p_map++=0x33;
		*p_map=0x3f;
	}
	
	for(i = 0;i<len;i++)
	{
		p_map = &Game_Map[(struct_temp.Snake_Y-1)][(struct_temp.Snake_X-1)*8];
		
		//�������ݴ���
		*p_map++=0x3f;
		if(i==0)
		{
			*p_map++=0x3f;
		  *p_map++=0x33;
		  *p_map++=0x33;
		  *p_map++=0x3f;
		}
		else
		{
		  *p_map++=0x3f;
		  *p_map++=0x3f;
		  *p_map++=0x3f;
		  *p_map++=0x3f;		
		}
		*p_map=0x3f;
		
		//��ȡ�¸�����
	  if(struct_temp.snake_next!=NULL)
		{
			struct_temp = *struct_temp.snake_next;
		}
	}
}

// *****************************************************************************
// �������ƣ�Key_Down_Move
// ��������������ѡ��
// �������: /
// �������: /
// ���ز���: /
// *****************************************************************************
void Key_Down_Move(U8 key_type)
{
	switch(Snake_Param.Direct_Flag)
	{
		case SNAKE_DIR_UP://Up
			if(key_type!=SNAKE_DIR_DOWN)Snake_Param.Direct_Flag = key_type;
			break;
		case SNAKE_DIR_DOWN://Down
			if(key_type!=SNAKE_DIR_UP)Snake_Param.Direct_Flag = key_type;
			break;
		case SNAKE_DIR_LEFT://Left
			if(key_type!=SNAKE_DIR_RIGHT)Snake_Param.Direct_Flag = key_type;
			break;
		case SNAKE_DIR_RIGHT://Right
			if(key_type!=SNAKE_DIR_LEFT)Snake_Param.Direct_Flag = key_type;
			break;
	}
}

// *****************************************************************************
// �������ƣ�Snake_RunStep
// ����������̰����-�ߵ��ƶ�
// �������: /
// �������: /
// ���ز���: /
// *****************************************************************************
void Snake_RunStep(void)
{
	static U8 time_count = 0;
		
	srand(time_count);        //��ʼ���������
	time_count++;
	if(SysParam.SysMode==MODE_GAME && time_count%Snake_Param.Snake_Speed==0 && Snake_Param.Snake_RefleshFg == SNAKE_REFLESH_ON)
	{		
    //������Ļ	  
		Clear_Map();		
		
		//�ж���ͷ�Ƿ�Ե�ʳ��
		if(Sanke_Body->Snake_X==Sanke_Fool_Buf[SNAKE_FOOL_LOCAL_X] && Sanke_Body->Snake_Y==Sanke_Fool_Buf[SNAKE_FOOL_LOCAL_Y])
		{
			//��������
			Snake_EatFool(Sanke_Fool_Buf[SNAKE_FOOL_LOCAL_X],Sanke_Fool_Buf[SNAKE_FOOL_LOCAL_Y]);
						
			//�����������
		  Check_Rand((rand()%SNAKE_RAND_X),(rand()%SNAKE_RAND_Y),Sanke_Fool_Buf);	

			//�Ƿ�
			Snake_Param.Game_Score++;		

			if(Snake_Param.Game_Score>=SNAKE_SCORE_MAX)//ʤ��-����RAM����(4K)��ֻ�����뵽29����������
			{
				Game_Stop((U8 *)"  GAME WINNER!!! ");
			}
		}
		else
		{
			//����ƫ��
			Snake_Offset(Snake_Param.Direct_Flag);
			
			//����Ƿ���ײ
			if(Check_Snake(Sanke_Body->Snake_X,Sanke_Body->Snake_Y))
			{
        Game_Stop((U8 *)"  GAME OVER !!!   ");
			}
		}		
	  if(!Snake_Param.Game_Result)Convert_StructMap(*Sanke_Body,Snake_Param.Sanke_Len);		
	}
}

// *****************************************************************************
// �������ƣ�Game_Print
// ������������Ϸ��ӡ
// �������: /
// �������: /
// ���ز���: /
// *****************************************************************************
void Game_Stop(U8 *tip_str)
{
	Snake_Param.Game_Result = SNAKE_GAME_OVER; //ֹͣ��Ϸ
	Snake_Param.Snake_RefleshFg = SNAKE_REFLESH_OFF;
	OLED_Clear_Row(2,6);
	OLED_ShowString(0,2,tip_str,16);
}

// *****************************************************************************
// ��������:Snake_EatFool
// ��������:��������
// �������: /
// �������: /
// ���ز���: /
// *****************************************************************************
void Snake_EatFool(U8 snake_x,U8 snake_y)
{
	switch(Snake_Param.Direct_Flag)
	{
		case SNAKE_DIR_UP:
			snake_y--;
			if(snake_y<SNAKE_AXIS_MIN)snake_y = SNAKE_AXIS_Y;			
			break;
		case SNAKE_DIR_DOWN:
			snake_y++;
			if(snake_y>SNAKE_AXIS_Y)snake_y = SNAKE_AXIS_MIN;	
			break;
		case SNAKE_DIR_LEFT:
			snake_x--;
			if(snake_x<SNAKE_AXIS_MIN)snake_x = SNAKE_AXIS_X;	
			break;
		case SNAKE_DIR_RIGHT:
			snake_x++;
			if(snake_x>SNAKE_AXIS_X)snake_x = SNAKE_AXIS_MIN;
			break;
	}
	Snake_Add(snake_x,snake_y);
}

// *****************************************************************************
// ��������:Snake_Offset
// ��������:����ƫ��
// �������: /
// �������: /
// ���ز���: /
// *****************************************************************************
void Snake_Offset(U8 direct)
{
	U8 snake_x = 0,snake_y = 0,i = 0;
	SNAKE_BODY *struct_temp = Sanke_Body;
	snake_x = Sanke_Body->Snake_X;
	snake_y = Sanke_Body->Snake_Y;
	switch(direct)
	{
		case SNAKE_DIR_UP:
			snake_y--;
			if(snake_y<SNAKE_AXIS_MIN)snake_y = SNAKE_AXIS_Y;			
			break;
		case SNAKE_DIR_DOWN:
			snake_y++;
			if(snake_y>SNAKE_AXIS_Y)snake_y = SNAKE_AXIS_MIN;	
			break;
		case SNAKE_DIR_LEFT:
			snake_x--;
			if(snake_x<SNAKE_AXIS_MIN)snake_x = SNAKE_AXIS_X;	
			break;
		case SNAKE_DIR_RIGHT:
			snake_x++;
			if(snake_x>SNAKE_AXIS_X)snake_x = SNAKE_AXIS_MIN;
			break;
	}		
	//ɾ��ĩ������
	for(i = 0;i<Snake_Param.Sanke_Len-1;i++)
	{
		 struct_temp = struct_temp->snake_next;
	}
	free(struct_temp->snake_next);
	struct_temp->snake_next = NULL;		
	Snake_Param.Sanke_Len--; 
	
	//����һ������
	Snake_Add(snake_x,snake_y);  
}

// *****************************************************************************
// �������ƣ�Snake_Add
// ��������������������
// �������: /
// �������: /
// ���ز���: /
// *****************************************************************************
void Snake_Add(U8 snake_x,U8 snake_y)
{	
  SNAKE_BODY* snake_temp = (struct snake_body *)malloc(SNAKE_STRUCT_LEN);
	
	snake_temp->Snake_X = snake_x; 
	snake_temp->Snake_Y = snake_y;
	snake_temp->snake_next = Sanke_Body;
	
	Sanke_Body = snake_temp;
	Snake_Param.Sanke_Len++;
}

// *****************************************************************************
// �������ƣ�Check_Rand
// ���������������������Ƿ�������
// �������: /
// �������: /
// ���ز���: /
// *****************************************************************************
U8 Check_Rand(U8 rand_x,U8 rand_y,U8 *rand_buf)
{
	U8 i = 0,result = 0;
	SNAKE_BODY *struct_temp = Sanke_Body;
	
	if(rand_x==0)rand_x+=1;
	if(rand_y==0)rand_y+=1;
	
	for(i = 0;i<Snake_Param.Sanke_Len;i++)
	{	
		//�������ݴ���
		if(struct_temp->Snake_X==rand_x && struct_temp->Snake_Y==rand_y)
		{
			result = Check_Rand((rand()%SNAKE_RAND_X),(rand()%SNAKE_RAND_Y),rand_buf);
		}		
		//��ȡ�¸�����
	  if(struct_temp->snake_next!=NULL)
		{
			struct_temp = struct_temp->snake_next;
		}
	}
	result = 1;
	*rand_buf++=rand_x;
	*rand_buf = rand_y;
	
	return result;	
}

// *****************************************************************************
// �������ƣ�Check_Snake
// ������������������Ƿ������ײ
// �������: /
// �������: /
// ���ز���: /
// *****************************************************************************
U8 Check_Snake(U8 sanke_x,U8 sanke_y)
{
	SNAKE_BODY *struct_temp = Sanke_Body->snake_next;
	U8 i = 0;
	
	for(i = 0;i<Snake_Param.Sanke_Len-1;i++)
	{
		if(sanke_x == struct_temp->Snake_X && sanke_y == struct_temp->Snake_Y)return 1;		
		if(struct_temp->snake_next!=NULL)struct_temp = struct_temp->snake_next;		
	}
	return 0;
}

// *****************************************************************************
// �������ƣ�Update_Map
// �������������µ�ͼ-̰����
// �������: /
// �������: /
// ���ز���: /
// *****************************************************************************
void Update_Map(void)
{
	U8 draw_i = 0;
	if(!Snake_Param.Game_Result)
	{
		for(draw_i = 0;draw_i<SNAKE_AXIS_Y;draw_i++)
		{
			OLED_DrawBMP(0,draw_i+2,128,draw_i+3,Game_Map[draw_i]);
		}
  }
}

// *****************************************************************************
// �������ƣ�Clear_Map
// ��������������������յ�ͼ
// �������: /
// �������: /
// ���ز���: /
// *****************************************************************************
void Clear_Map(void)
{
	U8 i = 0;
	U8 *p_map;
	SNAKE_BODY struct_temp = *Sanke_Body;
	for(i = 0;i<Snake_Param.Sanke_Len;i++)
	{
		p_map = &Game_Map[(struct_temp.Snake_Y-1)][(struct_temp.Snake_X-1)*8];
		
		//�������ݴ���
		*p_map++=0x00;
		*p_map++=0x00;
		*p_map++=0x00;
		*p_map++=0x00;
		*p_map++=0x00;
		*p_map=0x00;
		
		//��ȡ�¸�����
	  if(struct_temp.snake_next!=NULL)
		{
			struct_temp = *struct_temp.snake_next;
		}
	}
	OLED_Clear_Row(2,6);
}

// *****************************************************************************
// �������ƣ�Dispose_Game
// �����������ͷ���Դ
// �������: /
// �������: /
// ���ز���: /
// *****************************************************************************
void Dispose_Game(void)
{
	U16 i_cnt = 0;
	SNAKE_BODY *struct_temp = Sanke_Body,*struct_temp_2=NULL;
		
	Snake_Param.Snake_RefleshFg = SNAKE_REFLESH_OFF;
	Clear_Map();
		
	Sanke_Fool_Buf[SNAKE_FOOL_LOCAL_X] = 0x00;
	Sanke_Fool_Buf[SNAKE_FOOL_LOCAL_Y] = 0x00;
	
	for(i_cnt = 0;i_cnt<256;i_cnt++)
	{
		Game_Map[0][i_cnt] = 0x00;
		Game_Map[1][i_cnt] = 0x00;
		Game_Map[2][i_cnt] = 0x00;
		Game_Map[3][i_cnt] = 0x00;		
	}
	
	while(struct_temp!=NULL)
	{
		struct_temp_2 = struct_temp->snake_next;		
		free(struct_temp);
		struct_temp = struct_temp_2;
	}
  Sanke_Body = NULL;	
}

// *****************************************************************************
// �������ƣ�Game_Reset
// ����������������Ϸ
// �������: /
// �������: /
// ���ز���: /
// *****************************************************************************
void Game_Reset(void)
{
	if(Snake_Param.Game_Result==SNAKE_GAME_OVER)
	{
		//����������Ϸ
		Dispose_Game();
		Sanke_Init();
	}
	else
	{
		//�����ٶ�
		Snake_Param.Snake_RefleshFg = SNAKE_REFLESH_OFF;
		
		Snake_Param.Snake_Speed++;
		if(Snake_Param.Snake_Speed>SNAKE_SPEED_MAX)Snake_Param.Snake_Speed = SNAKE_SPEED_MIN;
		
		//д��EEP
		AT24C02_WriteOneByte(SNAKE_SPEED,Snake_Param.Snake_Speed);
		Snake_Param.Snake_RefleshFg = SNAKE_REFLESH_ON;
	}
}

// *****************************************************************************
// �������ƣ�Game_SetSpeed
// ����������������Ϸ�ٶ�
// �������: /
// �������: /
// ���ز���: /
// *****************************************************************************
void Game_SetSpeed(U8 speed)
{
	if(speed<SNAKE_SPEED_MIN)speed = SNAKE_SPEED_MIN;
	Snake_Param.Snake_Speed = speed; 
}


