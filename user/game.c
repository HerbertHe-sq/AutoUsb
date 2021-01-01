#include "includes.h"
#include "map.h"

//内部变量
SNAKE_BODY* Sanke_Body;   //蛇身链表
SNAKE_PARAM Snake_Param={1,4,0,0,0,1};  //蛇参数 //蛇身长度  //方向标志位-默认向右  //刷新标志位   //游戏结果   //记分    
U8 Sanke_Fool_Buf[SNAKE_FOOL_ITEM_NUM]; //食物数组

//外部变量
extern SYSTEMSTRUCT SysParam;

// *****************************************************************************
// 函数名称：Sanke_Init
// 功能描述：初始化-贪吃蛇
// 输入参数: /
// 输出参数: /
// 返回参量: /
// *****************************************************************************
void Sanke_Init(void)
{	
	//释放空间
	free(Sanke_Body);	
	
	//申请空间
	Sanke_Body = (SNAKE_BODY *)malloc(SNAKE_STRUCT_LEN);
	Sanke_Body->snake_next = NULL;
	Sanke_Body->Snake_X = 1; 
	Sanke_Body->Snake_Y = 3;

	Snake_Param.Sanke_Len = 1;     //初始化长度
	Snake_Param.Direct_Flag = SNAKE_DIR_RIGHT;   //初始化方向
	
	//初始化食物
	Sanke_Fool_Buf[SNAKE_FOOL_LOCAL_X] = 8;
	Sanke_Fool_Buf[SNAKE_FOOL_LOCAL_Y] = 2;	
	
	//数据转换
	Convert_StructMap(*Sanke_Body,Snake_Param.Sanke_Len);
	
	//画图
	Update_Map();
	
	Snake_Param.Snake_RefleshFg = SNAKE_REFLESH_ON;  //开始游戏
	Snake_Param.Game_Result = SNAKE_GAME_RUNING;     //结果
	Snake_Param.Game_Score = 0;                      //分数
}

// *****************************************************************************
// 函数名称：Convert_StructMap
// 功能描述：修改对应数据
// 输入参数: /
// 输出参数: /
// 返回参量: /
// *****************************************************************************
void Convert_StructMap(SNAKE_BODY body_temp,U8 len)
{
	U8 i = 0;
	U8 *p_map;
	SNAKE_BODY struct_temp = body_temp;
	
	//处理食物
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
		
		//链表数据处理
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
		
		//获取下个链表
	  if(struct_temp.snake_next!=NULL)
		{
			struct_temp = *struct_temp.snake_next;
		}
	}
}

// *****************************************************************************
// 函数名称：Key_Down_Move
// 功能描述：方向选择
// 输入参数: /
// 输出参数: /
// 返回参量: /
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
// 函数名称：Snake_RunStep
// 功能描述：贪吃蛇-蛇的移动
// 输入参数: /
// 输出参数: /
// 返回参量: /
// *****************************************************************************
void Snake_RunStep(void)
{
	static U8 time_count = 0;
		
	srand(time_count);        //初始化随机数据
	time_count++;
	if(SysParam.SysMode==MODE_GAME && time_count%Snake_Param.Snake_Speed==0 && Snake_Param.Snake_RefleshFg == SNAKE_REFLESH_ON)
	{		
    //清理屏幕	  
		Clear_Map();		
		
		//判断蛇头是否吃到食物
		if(Sanke_Body->Snake_X==Sanke_Fool_Buf[SNAKE_FOOL_LOCAL_X] && Sanke_Body->Snake_Y==Sanke_Fool_Buf[SNAKE_FOOL_LOCAL_Y])
		{
			//链表增加
			Snake_EatFool(Sanke_Fool_Buf[SNAKE_FOOL_LOCAL_X],Sanke_Fool_Buf[SNAKE_FOOL_LOCAL_Y]);
						
			//产生随机数据
		  Check_Rand((rand()%SNAKE_RAND_X),(rand()%SNAKE_RAND_Y),Sanke_Fool_Buf);	

			//记分
			Snake_Param.Game_Score++;		

			if(Snake_Param.Game_Score>=SNAKE_SCORE_MAX)//胜利-由于RAM有限(4K)，只能申请到29个链表数据
			{
				Game_Stop((U8 *)"  GAME WINNER!!! ");
			}
		}
		else
		{
			//进行偏移
			Snake_Offset(Snake_Param.Direct_Flag);
			
			//检查是否碰撞
			if(Check_Snake(Sanke_Body->Snake_X,Sanke_Body->Snake_Y))
			{
        Game_Stop((U8 *)"  GAME OVER !!!   ");
			}
		}		
	  if(!Snake_Param.Game_Result)Convert_StructMap(*Sanke_Body,Snake_Param.Sanke_Len);		
	}
}

// *****************************************************************************
// 函数名称：Game_Print
// 功能描述：游戏打印
// 输入参数: /
// 输出参数: /
// 返回参量: /
// *****************************************************************************
void Game_Stop(U8 *tip_str)
{
	Snake_Param.Game_Result = SNAKE_GAME_OVER; //停止游戏
	Snake_Param.Snake_RefleshFg = SNAKE_REFLESH_OFF;
	OLED_Clear_Row(2,6);
	OLED_ShowString(0,2,tip_str,16);
}

// *****************************************************************************
// 函数名称:Snake_EatFool
// 功能描述:蛇身增加
// 输入参数: /
// 输出参数: /
// 返回参量: /
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
// 函数名称:Snake_Offset
// 功能描述:蛇身偏移
// 输入参数: /
// 输出参数: /
// 返回参量: /
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
	//删除末端数据
	for(i = 0;i<Snake_Param.Sanke_Len-1;i++)
	{
		 struct_temp = struct_temp->snake_next;
	}
	free(struct_temp->snake_next);
	struct_temp->snake_next = NULL;		
	Snake_Param.Sanke_Len--; 
	
	//增加一个数据
	Snake_Add(snake_x,snake_y);  
}

// *****************************************************************************
// 函数名称：Snake_Add
// 功能描述：增加链表长度
// 输入参数: /
// 输出参数: /
// 返回参量: /
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
// 函数名称：Check_Rand
// 功能描述：检查随机数据是否在链表
// 输入参数: /
// 输出参数: /
// 返回参量: /
// *****************************************************************************
U8 Check_Rand(U8 rand_x,U8 rand_y,U8 *rand_buf)
{
	U8 i = 0,result = 0;
	SNAKE_BODY *struct_temp = Sanke_Body;
	
	if(rand_x==0)rand_x+=1;
	if(rand_y==0)rand_y+=1;
	
	for(i = 0;i<Snake_Param.Sanke_Len;i++)
	{	
		//链表数据处理
		if(struct_temp->Snake_X==rand_x && struct_temp->Snake_Y==rand_y)
		{
			result = Check_Rand((rand()%SNAKE_RAND_X),(rand()%SNAKE_RAND_Y),rand_buf);
		}		
		//获取下个链表
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
// 函数名称：Check_Snake
// 功能描述：检查蛇身是否出现碰撞
// 输入参数: /
// 输出参数: /
// 返回参量: /
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
// 函数名称：Update_Map
// 功能描述：更新地图-贪吃蛇
// 输入参数: /
// 输出参数: /
// 返回参量: /
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
// 函数名称：Clear_Map
// 功能描述：根据链表清空地图
// 输入参数: /
// 输出参数: /
// 返回参量: /
// *****************************************************************************
void Clear_Map(void)
{
	U8 i = 0;
	U8 *p_map;
	SNAKE_BODY struct_temp = *Sanke_Body;
	for(i = 0;i<Snake_Param.Sanke_Len;i++)
	{
		p_map = &Game_Map[(struct_temp.Snake_Y-1)][(struct_temp.Snake_X-1)*8];
		
		//链表数据处理
		*p_map++=0x00;
		*p_map++=0x00;
		*p_map++=0x00;
		*p_map++=0x00;
		*p_map++=0x00;
		*p_map=0x00;
		
		//获取下个链表
	  if(struct_temp.snake_next!=NULL)
		{
			struct_temp = *struct_temp.snake_next;
		}
	}
	OLED_Clear_Row(2,6);
}

// *****************************************************************************
// 函数名称：Dispose_Game
// 功能描述：释放资源
// 输入参数: /
// 输出参数: /
// 返回参量: /
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
// 函数名称：Game_Reset
// 功能描述：重置游戏
// 输入参数: /
// 输出参数: /
// 返回参量: /
// *****************************************************************************
void Game_Reset(void)
{
	if(Snake_Param.Game_Result==SNAKE_GAME_OVER)
	{
		//重新启动游戏
		Dispose_Game();
		Sanke_Init();
	}
	else
	{
		//设置速度
		Snake_Param.Snake_RefleshFg = SNAKE_REFLESH_OFF;
		
		Snake_Param.Snake_Speed++;
		if(Snake_Param.Snake_Speed>SNAKE_SPEED_MAX)Snake_Param.Snake_Speed = SNAKE_SPEED_MIN;
		
		//写入EEP
		AT24C02_WriteOneByte(SNAKE_SPEED,Snake_Param.Snake_Speed);
		Snake_Param.Snake_RefleshFg = SNAKE_REFLESH_ON;
	}
}

// *****************************************************************************
// 函数名称：Game_SetSpeed
// 功能描述：设置游戏速度
// 输入参数: /
// 输出参数: /
// 返回参量: /
// *****************************************************************************
void Game_SetSpeed(U8 speed)
{
	if(speed<SNAKE_SPEED_MIN)speed = SNAKE_SPEED_MIN;
	Snake_Param.Snake_Speed = speed; 
}


