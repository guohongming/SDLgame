
OBJS=main.c my.res

WFLAGS=-Wall -Wextra -g3
CFLAGS=`sdl-config --libs` -lSDL_image -lSDL_ttf \
		$(WFLAGS)
STATIC_CFLAGS=`sdl-config --static-libs` --static\
				-lSDL_image -lSDL_ttf -lfreetype \
				-lpng -lz $(WFLAGS)
a: FORCE
	gcc $(OBJS) -o a.exe $(CFLAGS)

sa: $(OBJS)
	gcc $(OBJS) -o a.exe $(STATIC_CFLAGS)
clean: FORCE
	rm *.o
FORCE:

