
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* in the debug mode*/
#define DEBUG

#define BALL_Y			400
#define BALL_TYPE_COUNT 7
#define MAX_BALL_LEVEL	7
#include "game.h"
#include "sprite.h"

#include "image.c"
#include "sprite.c"
#include "timer.c"

#include "func.c"
#include "ball.c"
/* 参数宏定义 */
// 用于调试
#define DE(d)			D("ERROR:" d ":%s\n", (SDL_GetError()))
// 初始化参数
#define SDL_INIT_ITEMS	(SDL_INIT_EVERYTHING &\
						~SDL_INIT_CDROM &\
						~SDL_INIT_JOYSTICK)
// 屏幕显示参数
#define SCREEN_WIDTH	640
#define SCREEN_HEIGHT	480
#define SCREEN_BPP		32
#define COLOR_KEY		0xffffff

#define FRAMES_PRE_SECOND		100
#define gWeaponPosY		400
enum GAME_STATE
{
	GAME_QUITE = 0,
	GAME_START,
	GAME_MAINMENU,
	GAME_PLAYING,
	GAME_WIN,
	GAME_LOST
};

/* 全局的游戏状态 */
enum GAME_STATE gGameRun;
SDL_Surface* gScreen;

SDL_Surface* gBg;

sprite* gWeapon;
sprite* gBalls;

ball* gUserBall;

int gWeaponPosX = 320;
timer* gFPS;

int gBallNextType;
int gUserPiont;

void game()
{
	gScreen = NULL;
	gGameRun = GAME_START;
	gUserBall = NULL;
	gUserPiont = 0;
}

int OnExecute(int argc, char* argvp[])
{
	game();
	if(false == OnInit())
	{
		D("Game Init Failed");
		return -1;
	}

	/* printf("CK:%d, COLOR:%d\n",gBalls->format->colorkey */
	/* 			,*(Uint32*)gBalls->pixels); */

	SDL_Event e;
	gGameRun = GAME_START;
	int lev = 0;
	timer* tBallShoot = timer_init();
	timer_start(tBallShoot);
	

	while(gGameRun != GAME_QUITE)
	{
		timer_start(gFPS);
		while (SDL_PollEvent(&e))
		{
			OnEvent(&e);
		}
		OnLoop();
		OnRender();
		
		/* 初始的球 */
		if(gGameRun == GAME_START && timer_getticks(tBallShoot) >= 1000)
		{
			if(lev < MAX_BALL_LEVEL)
				BallGameInit();
			lev ++;
			/* 当最后一波球都到了的时候 */
			if(lev == MAX_BALL_LEVEL + 2)
			{
				gGameRun = GAME_PLAYING;
				timer_free(tBallShoot);
				gBallNextType = GetNextType();
				gUserBall = BallAddForGame(gBallNextType, gWeaponPosX - 8, BALL_Y, 0, 0);
				gUserBall->state = BALL_CONTROL;
			}else{
				timer_start(tBallShoot);
			}

		}

		int t = 1000/FRAMES_PRE_SECOND - timer_getticks(gFPS);
		if(t>0)
		{
			SDL_Delay(t);
		}
	}/* end of outer while */
	OnCleanup();

return 0;
}

bool OnInit()
{
	if(SDL_Init(SDL_INIT_ITEMS) < 0)
	{
		DE("SDL_Init");
		return false;
	}

	gScreen = SDL_SetVideoMode(SCREEN_WIDTH	\
				,SCREEN_HEIGHT,SCREEN_BPP	\
				,SDL_HWSURFACE | SDL_DOUBLEBUF);
	if(gScreen == NULL)
	{
		DE("screen init");
		return false;
	}
	/* load files */
	gBg = ImageLoad("img/bg.png");
	gBalls = SpriteLoad("img/balls.png",64,64);
	gWeapon = SpriteLoad("img/weapon.png",50,150);
	if(gBalls == NULL || gBg == NULL || gWeapon == NULL)
	{
		DE("load file failed");
		return false;
	}
	SpriteSetColorKey(gBalls, COLOR_KEY);
	SpriteSetColorKey(gWeapon, COLOR_KEY);
	
	/* 初始化一些其他的函数 */
	if(false == FuncInit())
	{
		D("FuncInit failed");
		return false;
	}
	/* 初始fps计时器 */
	gFPS = timer_init();
	/* 设置窗口标题 */
	SDL_WM_SetCaption("balls----Wener[http://wener.me]",NULL);


	/* 添加事件处理 */
	SDL_SetEventFilter(PlayingEventHandler);

	BallInit();

	/* static ball b; */
	/* b.typeid = 9; */
	/* b.x = 20,b.y = 460, b.vy = -0.1, b.ay = -0.01; */
	/* b.state = BALL_STOPPED; */
	/* BallAdded(&b); */

	/* BallAddForGame(1,90,460,-0.1,-0.03); */

return true;
}

void OnCleanup()
{
	SDL_FreeSurface(gScreen);
	SDL_FreeSurface(gBg);
	/* SDL_FreeSurface(gWeapon); */
	/* SDL_FreeSurface(gBalls); */
	SDL_Quit();
}

void OnEvent(SDL_Event* e)
{
	switch(e->type)
	{
		case SDL_QUIT:
			gGameRun = GAME_QUITE;
		break;
		default:
		break;
	}
}

void OnRender()
{
	ImageShow(0,0,NULL,gBg,gScreen);

	ImageShow(gWeaponPosX, 330, NULL, gWeapon->image, gScreen);

	/* 渲染球 */
	BallRender();
	/* 分数 */
	SDL_Surface *tmp;
	tmp = ImageFromInt(gUserPiont);
	ImageShow(0,gWeaponPosY, NULL, tmp, gScreen);
	SDL_FreeSurface(tmp);
	
	/* WIN or Lost */
	if(gGameRun == GAME_WIN || gGameRun == GAME_LOST)
	{
		if(gGameRun == GAME_WIN)
			tmp = ImageFromAlpha("YOU WIN");
		else
			tmp = ImageFromAlpha("YOU LOST");
		ImageShow((gScreen->w - tmp->w)/2,(gScreen->h-tmp->h)/2,NULL\
					,tmp, gScreen);
	}

	if(SDL_Flip(gScreen) < 0)
	{
		DE("SDL_Flip");
	}
}
void OnLoop()
{
	/* 对球的循环处理 */
	BallLoop();

	int rec = 0;
	if(gGameRun == GAME_PLAYING && gUserBall != NULL && gUserBall->state == BALL_STOPPED)
	{
		if(BallDisapearable(gUserBall))
		{
			BallRemove(gUserBall, &rec);
			gUserPiont += rec * rec;
		}
		/* 判断游戏是否已经完毕 */
		GameResult();

		gBallNextType = GetNextType();
		gUserBall = BallAddForGame(gBallNextType, gWeaponPosX - 8, BALL_Y, 0, 0);
		gUserBall->state = BALL_CONTROL;
	}
}


int PlayingEventHandler(const SDL_Event* e)
{
	if(gGameRun != GAME_PLAYING)
		return 1;
	switch(e->type)
	{
		case SDL_MOUSEMOTION:
			gWeaponPosX = e->button.x - gWeapon->width / 2;
			if(gWeaponPosX < 0)
				gWeaponPosX = 0;
			else if(gWeaponPosX > gScreen->w - gWeapon->width)
				gWeaponPosX = gScreen->w - gWeapon->width;

			if(gUserBall->state == BALL_CONTROL)
				gUserBall->x = gWeaponPosX - 8;
		break;
		case SDL_MOUSEBUTTONUP:
			switch(e->button.button)
			{
				case SDL_BUTTON_LEFT:
					if(gUserBall->state == BALL_CONTROL)
					{
						gUserBall->vy = -3;
						gUserBall->ay = -1;
						gUserBall->state = BALL_MOVING;
					}
				break;
			}
		break;
		default:
			return 1;
		break;
	}
return 0;
}
void GameResult()
{
	int i;
	ball* b;

	for(i  = 0; i < ballPool.count; i++)
	{
		b = ballPool.balls[i];
		if(b->state != BALL_DISAPEARED)
			break;
	}
	if(i == ballPool.count)
	{
		gGameRun = GAME_WIN;
		return;
	}

	for(i  = 0; i < ballPool.count; i++)
	{
		b = ballPool.balls[i];
		if(b->state != BALL_STOPPED)
			continue;
		if(SpriteCollision(gBalls, b->typeid, b->x, b->y\
						,gWeapon, 0, gWeaponPosX, gWeaponPosY\
						,NULL))
		{
			gGameRun = GAME_LOST;
			break;
		}
	}
	
}
/* 确保获取的类型是当前存在的 */
int GetNextType()
{
	int c;
	int i;

	c = 0;
	for(i = 0; i < BALL_TYPE_COUNT; i ++)
	{
		if(ballTypes[i] > 0)
			c ++;
	}
	
	/* 当c = 0 时 不能处理 */
	if(c == 0)
		return 0;
	c = rand() % c;
	for(i = 0; i < BALL_TYPE_COUNT; i++)
		if(ballTypes[i] > 0)
			break;
	for(; c > 0; i ++)
	{
		if(ballTypes[i] > 0)
			c --;
	}
return i;
}
