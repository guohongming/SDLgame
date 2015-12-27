#ifndef GAME_H
#define GAME_H

#include <stdbool.h>

/* DEFINE the DEBUG  */
#ifdef DEBUG
#include <stdio.h>
	#define DD(condition,format,args...)\
		if(false == (condition))		\
			D(format,##args);
	#define D(format,args...)			\
		fprintf(stderr, "%s:%d:", __FUNCTION__,__LINE__); \
		fprintf(stderr, format "\n", ##args)
#else
	#define DD(condition,format,args...)	((void)0)
	#define D(format,args...)			((void)0)
#endif

#include <SDL\SDL.h>
#include <SDL\SDL_image.h>
#include <SDL\SDL_ttf.h>

int OnExecute(int argc, char* argv[]);
bool OnInit();
void OnEvent(SDL_Event* event);
void OnRender();
void OnLoop();
void OnCleanup();
void GameResult();
int GetNextType();
int PlayingEventHandler(const SDL_Event* e);


/* end of header definition */
#endif
