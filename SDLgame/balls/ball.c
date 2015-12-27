
enum BALL_STATE
{
	BALL_MOVING,
	BALL_STOPPED,
	BALL_DISAPEARED,
	BALL_CONTROL
};

typedef struct ball
{
	int typeid;
	enum BALL_STATE state;

	double x,y;
	double vx,vy;
	double ax,ay;

	struct ball** nears;
	int nearCount;
	int nearMax;
} ball;

/* 全局变量 */
struct
{
	ball** balls;
	int count;
	int max;
}ballPool = {NULL, 0, 0};

#define BALL_FRICTION	0.9

sprite* spBalls;
SDL_Rect rectScreen;
extern sprite* gBalls;
extern SDL_Surface* gScreen;

int ballTypes[BALL_TYPE_COUNT] = {0};

bool BallInit();
bool BallAdded(ball* b);	/* 将Ball添加到ballpool */
void BallLoop();				/* 使BallPool里面的球运动 */
void BallRender();
bool BallDisapearable(ball* b);
void BallRemove(ball* b, int *rec);
bool BallNearby(ball* ba, ball* bb);



bool BallInit()
{
	static bool isInit = false;

	if(isInit)
	{
		D("Init Again");
		return true;
	}else
		isInit = true;
	
	/* 初始一些变量 */
	ballPool.max = 256;
	ballPool.balls = (ball**)malloc(sizeof(ball*) * 256);

	/* 屏幕区域 */
	rectScreen.x = 0, rectScreen.y = 0;
	rectScreen.w = gScreen->w, rectScreen.h = gScreen->h;

return true;
}
bool BallDisapearable(ball* b)
{
	int sameBallCount = 0;
	int i,j;
	ball *bb,*bc;

	/* 只检测两层深度 因为如果可以消失 这样就够了 */
	for(i = 0; i < b->nearCount && sameBallCount < 3; i ++)
	{
		bb = b->nears[i];
		if(bb->state != BALL_DISAPEARED && bb->typeid == b->typeid)
		{
			sameBallCount ++;

			for(j = 0; j < bb->nearCount && sameBallCount < 3; j ++)
			{
				bc = bb->nears[j];
				if(bc->state != BALL_DISAPEARED && bc->typeid == b->typeid)
					sameBallCount ++;
			}
		}
	}

	if(sameBallCount < 3)
		return false;
	else
		return true;
}
void BallRemove(ball* b, int *rec)
{
	if(b->state == BALL_DISAPEARED)
		return;
	else
	{
		(*rec) ++;
		b->state = BALL_DISAPEARED;
		/* 减去对应类型的统计 */
		ballTypes[b->typeid] --;
	}

	int i;
	for(i = 0; i < b->nearCount; i ++)
		if(b->nears[i]->typeid == b->typeid)
			BallRemove(b->nears[i], rec);
}
bool BallNearby(ball* ba, ball* bb)
{
	if(ba->nearCount >= ba->nearMax)
	{
		ba->nearMax += 256;
		ba->nears = realloc(ba->nears, sizeof(int) * ba->nearMax);
	}
	if(bb->nearCount >= bb->nearMax)
	{
		bb->nearMax += 256;
		bb->nears = realloc(bb->nears, sizeof(int) * bb->nearMax);
	}
	
	ba->nears[ba->nearCount ++] = bb;
	bb->nears[bb->nearCount ++] = ba;

return true;
}

bool BallAdded(ball* b)
{
	if(NULL == b)
	{
		D("expection param NULL");
		return false;
	}
	if(ballPool.count == ballPool.max)
	{
		ballPool.max += 256;
		ballPool.balls = (ball**)realloc(ballPool.balls,sizeof(ball*) * ballPool.max);
	}
	ballPool.balls[ballPool.count++] = b;

	/* 添加对应类型的总数统计 */
	ballTypes[b->typeid] ++;

return true;
}
void BallLoop()
{
	int i,j;
	ball* b;
	ball* bb;
		/* 运动计算 */
	for(i = 0; i < ballPool.count; i++)
	{
		b = ballPool.balls[i];
		if(b->state != BALL_MOVING)
			continue;
		/* b->vx += b->ax; */
		b->vy += b->ay;
		/* b->x  += b->vx; */
		b->y  += b->vy;

		if(b->y < 0)
		{
			b->y = 0;
			b->state = BALL_STOPPED;
		}
		/* TODO 没有对其他边界计算，因为在这里都是垂直运动，甚至都只有Y的运动 */
	}
	/* return ; */
	/* 删除之前的碰撞联系 */
	for(i = 0; i < ballPool.count; i++)
		ballPool.balls[i]->nearCount = 0;
		/* 碰撞计算 */
	bool isCollision;
	for(i = 0; i < ballPool.count; i++)
	{
		b = ballPool.balls[i];
		/* 不检测不显示的球 */
		if(b->state == BALL_DISAPEARED)
			continue;
		for(j = i + 1; j < ballPool.count; j ++)
		{
			bb = ballPool.balls[j];
			if(bb->state == BALL_DISAPEARED)
				continue;

			isCollision = SpriteCollision(gBalls,b->typeid,(int)b->x,(int)b->y\
										,gBalls,bb->typeid,(int)bb->x,(int)bb->y\
										,NULL);
			if(isCollision)
			{
				bb->state = b->state = BALL_STOPPED;
				BallNearby(b,bb);
			}
		}
	}
}
void BallRender()
{
	int i;
	ball* b;
	for(i = 0; i < ballPool.count; i++)
	{
		b = ballPool.balls[i];
		if(b->state == BALL_DISAPEARED)
			continue;
		ImageShow(b->x,b->y,&gBalls->clip[b->typeid],gBalls->image, gScreen);
	}

}

ball* BallAddForGame(int typeid, double x, double y, double vy, double ay)
{
	ball* b;
	b = malloc(sizeof(ball));
	
	b->typeid = typeid;
	b->x = x;
	b->y = y;
	b->vy = vy;
	b->ay = ay;
	b->state = BALL_MOVING;
	b->nears = NULL; 
	b->nearCount = b->nearMax = b->vx = b->ay = 0;

	BallAdded(b);
return b;
}

void BallGameInit()
{
	static int odd = 0;
	odd ++;
	int i, r;
	int oddOffset = 35 * (odd % 2);

	srand( time(NULL) );
	for(i = 0; i < 9 - odd % 2; i ++)
	{
		r = rand();
		BallAddForGame(r%BALL_TYPE_COUNT,oddOffset + i * 70 - r % BALL_TYPE_COUNT, BALL_Y, -6, -2);
	}
}
