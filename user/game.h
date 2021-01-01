#ifndef _GAME_H
#define	_GAME_H

#define SNAKE_STRUCT_LEN sizeof(SNAKE_BODY)
#define SNAKE_AXIS_X    16           //XY轴设置
#define SNAKE_AXIS_Y    5
#define SNAKE_AXIS_MIN  1
#define SNAKE_RAND_X    17           //随机数据上限
#define SNAKE_RAND_Y    6
#define SNAKE_SPEED_MAX 15
#define SNAKE_SPEED_MIN 1
#define SNAKE_SCORE_MAX 29

typedef enum{
	SNAKE_FOOL_LOCAL_X = 0,
	SNAKE_FOOL_LOCAL_Y,
	SNAKE_FOOL_ITEM_NUM
}snake_fool_local_t;

typedef enum{
	SNAKE_DIR_UP = 1,
	SNAKE_DIR_DOWN,
	SNAKE_DIR_LEFT,
	SNAKE_DIR_RIGHT
}snake_dir_t;

typedef enum{
	SNAKE_REFLESH_OFF = 0,
	SNAKE_REFLESH_ON
}snake_reflesh_t;

typedef enum{
	SNAKE_GAME_RUNING = 0,
	SNAKE_GAME_OVER
}snake_result_t;

struct snake_body{
	U8 Snake_X;
	U8 Snake_Y;
	struct snake_body *snake_next;
}; //蛇身链表

typedef struct snake_body SNAKE_BODY;

typedef struct snake_param{
	U8 Sanke_Len;
	U8 Direct_Flag;
	U8 Snake_RefleshFg;
	U8 Game_Result;
	U8 Game_Score;
	U8 Snake_Speed;
}SNAKE_PARAM;



void Sanke_Init(void);
void Convert_StructMap(SNAKE_BODY body_temp,U8 len);
void Key_Down_Move(U8 key_type);
void Snake_RunStep(void);
void Snake_EatFool(U8 snake_x,U8 snake_y);
void Snake_Offset(U8 direct);
void Snake_Add(U8 snake_x,U8 snake_y);
U8 Check_Snake(U8 sanke_x,U8 sanke_y);
U8 Check_Rand(U8 rand_x,U8 rand_y,U8 *rand_buf);
void Update_Map(void);
void Clear_Map(void);
void Dispose_Game(void);
void Game_Reset(void);
void Game_SetSpeed(U8 speed);
void Game_Stop(U8 *str);

#endif
