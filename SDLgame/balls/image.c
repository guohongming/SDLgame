#include <SDL\SDL.h>
#include <SDL\SDL_image.h>
#include <string.h>

#include "image.h"

SDL_Surface* ImageLoad(char* fn)
{
	SDL_Surface* im_loaded = NULL;
	SDL_Surface* im_optimized = NULL;

	im_loaded = IMG_Load(fn);
	if(im_loaded != NULL)
	{
		im_optimized = SDL_DisplayFormat(im_loaded);
		SDL_FreeSurface(im_loaded);
	}
return im_optimized;
}

void ImageShow(int x, int y, SDL_Rect* clip, SDL_Surface* src, SDL_Surface* dest)
{
	SDL_Rect offset;
	offset.x = x;
	offset.y = y;

	SDL_BlitSurface(src,clip,dest,&offset);
}



Uint32 GetColorAt(SDL_Surface* surface, int x, int y)
{
#ifdef DEBUG
	if(x > surface->w || y > surface->h)
	{
		D("%s OverFlow,%d/%d, %d/%d",__FUNCTION__,x,surface->w,y,surface->h);
		return 0;
	}
#endif

    int bpp = surface->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to retrieve */
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;
	Uint32 pixelColor;

	switch(bpp)
	{
		case(1):
			pixelColor = *p;
			break;
		case(2):
			pixelColor = *(Uint16*)p;
			break;
		case(3):
			if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
				pixelColor = p[0] << 16 | p[1] << 8 | p[2];
			else
				pixelColor = p[0] | p[1] << 8 | p[2] << 16;
			break;
		case(4):
			pixelColor = *(Uint32*)p;
			break;
	}
	
	Uint8 red, green, blue, alpha;
	SDL_GetRGBA(pixelColor, surface->format, &red, &green, &blue, &alpha);
	
	Uint32 color = alpha << 24 | red << 16 | green << 8 | blue;
return color;
}


void SetColorAt(SDL_Surface *surface, int x, int y, Uint32 pixel)
{
#ifdef DEBUG
	if(x > surface->w || y > surface->h)
	{
		D("%s OverFlow,%d/%d, %d/%d",__FUNCTION__,x,surface->w,y,surface->h);
		return;
	}
#endif

    int bpp = surface->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to set */
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

    switch(bpp) {
    case 1:
        *p = pixel;
        break;

    case 2:
        *(Uint16 *)p = pixel;
        break;

    case 3:
        if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
            p[0] = (pixel >> 16) & 0xff;
            p[1] = (pixel >> 8) & 0xff;
            p[2] = pixel & 0xff;
        } else {
            p[0] = pixel & 0xff;
            p[1] = (pixel >> 8) & 0xff;
            p[2] = (pixel >> 16) & 0xff;
        }
        break;

    case 4:
        *(Uint32 *)p = pixel;
        break;
    }
}

int GetAlphaAt(SDL_Surface* surface, int x, int y)
{
#ifdef DEBUG
	if(x > surface->w || y > surface->h)
	{
		D("%s OverFlow,%d/%d, %d/%d",__FUNCTION__,x,surface->w,y,surface->h);
		return 0;
	}
#endif

	if(surface->flags & SDL_SRCALPHA)
	{
		return GetColorAt(surface,x,y) >> 24;
	}else if(surface->flags & SDL_SRCCOLORKEY)
	{
		Uint32 c = GetColorAt(surface,x,y) & 0xffffff;
		return ( c == surface->format->colorkey)? 0: 0xff;
	}else
		return 0xff;
}
