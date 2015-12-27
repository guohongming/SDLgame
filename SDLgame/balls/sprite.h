#ifndef SPRITE_H
#define SPRITE_H
typedef struct sprite sprite;
sprite* SpriteLoad(char* fn, int w, int h);
void SpriteFree(sprite** sp);
int SpriteSetColorKey(sprite* sp, Uint32 key);


#define Maximum(a, b) ((a > b) ? a : b)
#define Minimum(a, b) ((a < b) ? a : b)

bool IntersectionRect(SDL_Rect *ra, SDL_Rect *rb, SDL_Rect *ri);

/* 在进行碰撞检测前需要对其进行初始化 */
bool SpriteCollision( sprite *spa, int ac,int ax, int ay \
					, sprite *spb, int bc, int bx, int by
					, SDL_Rect **ri);

#endif
