
typedef struct
{
	int startTicks;
	int pausedTicks;
	bool paused;
	bool started;
}timer;

timer *timer_init();
void timer_freee(timer *t);
void timer_start(timer *t);
void timer_stop(timer *t);
void timer_pause(timer *t);
void timer_unpause(timer *t);
int timer_getticks(timer *t);
void timer_free(timer *t);

timer *timer_init()
{
	timer *t;
	t = (timer*)malloc(sizeof(timer));
	t->startTicks = 0;
	t->pausedTicks = 0;
	t->paused = false;
	t->started = false;
return t;
}
void timer_start(timer *t)
{
	t->started = true;
	t->paused = false;
	t->startTicks = SDL_GetTicks();
}
void timer_stop(timer *t)
{
	t->started = false;
	t->paused = false;
}
void timer_pause(timer *t)
{
	if((t->started == true) && (t->paused == false))
	{
		t->paused = true;
		t->pausedTicks = SDL_GetTicks() - t->startTicks;
	}
}
void timer_unpause(timer *t)
{
	if(t->paused == true)
	{
		t->paused = false;
		
		t->startTicks = SDL_GetTicks() - t->pausedTicks;

		t->pausedTicks = 0;
	}
}
int timer_getticks(timer *t)
{
	if(t->started == true)
	{
		if(t->paused == true)
			return t->pausedTicks;
		else
			return SDL_GetTicks() - t->startTicks;
	}
return 0;
}
void timer_free(timer *t)
{
	free(t);
}
