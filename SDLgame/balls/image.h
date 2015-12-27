#ifndef IMAGE_H
#define IMAGE_H

SDL_Surface* ImageLoad(char* fn);
void ImageShow(int x, int y, SDL_Rect* clip, SDL_Surface* src, SDL_Surface* dest);
SDL_Surface* ImageFromInt(int num);
SDL_Surface* ImageFromAlpha(char* str);

Uint32 GetColorAt(SDL_Surface* surface, int x, int y);
void SetColorAt(SDL_Surface* surface, int x, int y, Uint32 pixel);
#endif
