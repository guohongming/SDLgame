
#define IMAGE_NUM_PATH		"img/num.png"
#define IMAGE_ALPHA_PATH	"img/alpha.png"
#define IMAGE_TEXT_SIZE		32
#define IMAGE_TEXT_COLORKEY	0xffffff

static int sspinit = false;
sprite* sspNum = NULL;
sprite* sspAlpha = NULL;

bool FuncInit()
{
	if(sspinit == true)
	{
		D("FUncInit reinit");
		return true;
	}

	sspNum = SpriteLoad(IMAGE_NUM_PATH,IMAGE_TEXT_SIZE,IMAGE_TEXT_SIZE);
	sspAlpha = SpriteLoad(IMAGE_ALPHA_PATH,IMAGE_TEXT_SIZE,IMAGE_TEXT_SIZE);
	if(sspNum == NULL || sspAlpha == NULL)
	{
		D("FuncInit text static init failed");
		return false;
	}
	SpriteSetColorKey(sspNum,IMAGE_TEXT_COLORKEY);
	SpriteSetColorKey(sspAlpha,IMAGE_TEXT_COLORKEY);

	sspinit = true;
return true;
}

SDL_Surface* ImageFromInt(int num)
{
	if(false ==  sspinit)
	{
		D("Ini 图片尚未初始化");
		return NULL;
	}
	SDL_Surface* surNum = NULL;
	int len;
	char strbuf[20];
	
	sprintf(strbuf,"%d",num);
	len = strlen(strbuf);
	/* 设置surNum */
	surNum= SDL_CreateRGBSurface(SDL_HWSURFACE\
			,len * IMAGE_TEXT_SIZE
			,IMAGE_TEXT_SIZE
			,32,0,0,0,0); 
	SDL_FillRect(surNum,NULL,IMAGE_TEXT_COLORKEY);
	SDL_SetColorKey(surNum,SDL_SRCCOLORKEY,IMAGE_TEXT_COLORKEY);

	int i,n;
	SDL_Rect rectDest;
	rectDest.x = 0;
	rectDest.y = 0;

	for(i = 0; i < len; i ++)
	{
		n = strbuf[i] - '0';
		rectDest.x = IMAGE_TEXT_SIZE * i;
		SDL_BlitSurface(sspNum->image
				,&sspNum->clip[n]
				,surNum
				,&rectDest);
	}

return surNum;	
}
SDL_Surface* ImageFromAlpha(char* strbuf)
{
	if(false ==  sspinit)
	{
		D("Alpha 图片尚未初始化");
		return NULL;
	}

	SDL_Surface* surAlpha = NULL;
	int len;
	
	len = strlen(strbuf);
	/* 设置surNum */
	surAlpha = SDL_CreateRGBSurface(SDL_HWSURFACE\
			,len * IMAGE_TEXT_SIZE
			,IMAGE_TEXT_SIZE
			,32,0,0,0,0); 
	SDL_FillRect(surAlpha,NULL,IMAGE_TEXT_COLORKEY);
	SDL_SetColorKey(surAlpha,SDL_SRCCOLORKEY,IMAGE_TEXT_COLORKEY);

	int i,n;
	char c;
	sprite* spCur;
	SDL_Rect rectDest;
	rectDest.x = 0;
	rectDest.y = 0;

	for(i = 0; i < len; i ++)
	{
		c = strbuf[i];
		n = 0;
		if(c >= '0' && c <= '9')
		{
			n = strbuf[i] - '0';
			spCur = sspNum;
		}else if( c == ' ')
		{
			rectDest.x = IMAGE_TEXT_SIZE * i;
			continue;
		}else{
			if(c >= 'A' && c <= 'Z')
			{
				n = sspAlpha->column;
				c += 'a' - 'A';
			}
			n += c - 'a';
			spCur = sspAlpha;
		}

		rectDest.x = IMAGE_TEXT_SIZE * i;

		SDL_BlitSurface(spCur->image
				,&spCur->clip[n]
				,surAlpha
				,&rectDest);
	}
return surAlpha;
}
