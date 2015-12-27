

typedef struct sprite
{
	SDL_Surface *image;
	int width;
	int height;
	SDL_Rect* clip;
	int count;

	int line;
	int column;
} sprite;




sprite* SpriteLoad(char* fn, int w, int h)
{
	/* 加载图片 */
	SDL_Surface* im;
	im = ImageLoad(fn);
	if(im == NULL)
	{
		D("%s load failed", fn);
		return NULL;
	}

	/* 设定 sprite */
	sprite* sp;
	sp = malloc(sizeof(sprite));
	if(sp == NULL)
	{
		D("SpriteLoad malloc failed");
		SDL_FreeSurface(im);
		return NULL;
	}

	sp->image = im;
	sp->width = w;
	sp->height = h;
	sp->line = im->h / h;
	sp->column = im->w / w;
	sp->count = sp->line * sp->column;

	if(sp->count <= 0)
	{
		D("Spirte count lt 0");
		SDL_FreeSurface(im);
		free(sp);
		return NULL;
	}

	/* 计算clip */
	SDL_Rect* clip;
	clip = malloc(sizeof(SDL_Rect) * sp->count);
	if(clip == NULL)
	{
		D("SpriteLoad clip malloc failed");
		SDL_FreeSurface(im);
		free(sp);
		return NULL;
	}
	
	int i,j,n;
	for(i = 0; i < sp->line; i++)
	{
		for(j = 0; j < sp->column; j++ )
		{
			n = i * sp->column + j;
			clip[n].x = w * j;
			clip[n].y = h * i;
			clip[n].w = w;
			clip[n].h = h;
		}
	}

	sp->clip = clip;
return sp;
}

void SpriteFree(sprite** sp)
{
	SDL_FreeSurface((*sp)->image);
	free((*sp)->clip);
	free(*sp);
	*sp = NULL;
}

int SpriteSetColorKey(sprite* sp, Uint32 key)
{
	return SDL_SetColorKey(sp->image,SDL_SRCCOLORKEY,key);
}

bool SpriteCollision( sprite *spa, int ac,int ax, int ay \
					, sprite *spb, int bc, int bx, int by\
					, SDL_Rect** ri)
{
	SDL_Rect rectA, rectB, *rectIntersection;

	rectIntersection = malloc(sizeof(SDL_Rect));
	if(rectIntersection == NULL)
	{
		D("rectIntersection malloc Failed");
		return false;
	}

	rectA.x = ax, rectA.y = ay;
	rectA.w = spa->width,rectA.h = spa->height;

	rectB.x = bx, rectB.y = by;
	rectB.w = spb->width,rectB.h = spb->height;

	// 求交叉范围
	IntersectionRect(&rectA, &rectB, rectIntersection);
	// 检查是否有交叉
	if(rectIntersection->w == 0 && rectIntersection->h == 0)
	{
		free(rectIntersection);
		return false;
	}
	// 计算在图像上的交叉点 需要还原移动点

	rectA.x = spa->clip[ac].x + rectIntersection->x - ax;
	rectA.y = spa->clip[ac].y + rectIntersection->y - ay;
	rectB.x = spb->clip[bc].x + rectIntersection->x - bx;
	rectB.y = spb->clip[bc].y + rectIntersection->y - by;
	// 相交的范围内检测碰撞

	SDL_LockSurface(spa->image);
	SDL_LockSurface(spb->image);
	int x, y, isColl = false;
	for(y = 0; y < rectIntersection->h; y++)
		for(x = 0; x < rectIntersection->w; x++)
		{
			if(GetAlphaAt(spa->image,rectA.x + x, rectA.y + y)
				&& GetAlphaAt(spb->image,rectB.x + x, rectB.y + y))
			{
				isColl = true;
				break;
			}
		}

	SDL_UnlockSurface(spa->image);
	SDL_UnlockSurface(spb->image);
	if(ri != NULL)
	{
		*ri = rectIntersection;
	}else
	{
		free(rectIntersection);
	}
return isColl;
}
bool IntersectionRect(SDL_Rect *ra, SDL_Rect *rb, SDL_Rect *ri)
{
	int x1 = Maximum(ra->x, rb->x);
	int y1 = Maximum(ra->y, rb->y);
	int x2 = Minimum(ra->x + ra->w, rb->x + rb->w);
	int y2 = Minimum(ra->y + ra->h, rb->y + rb->h);

	int w = x2 - x1;
	int h = y2 - y1;

	if(w > 0 && h > 0)
	{
		ri->x = x1;
		ri->y = y1;
		ri->w = w;
		ri->h = h;
	}else
	{
		ri->x = 0;
		ri->y = 0;
		ri->w = 0;
		ri->h = 0;
	}
return w <= 0 || h <= 0;
}
