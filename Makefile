
.PHONY = clean

CFLAGS = -I/usr/include/ffmpeg -I/usr/include/SDL2 -D_REENTRANT -I$(HOME)/.local/include -L$(HOME)/.local/lib
LDFLAGS = -lavcodec -lavformat -lswscale -lmpv -lSDL2 -Wl,-rpath=_ORIGIN

vabslider: vabslider.c
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS) 
	chrpath -r '$$ORIGIN' $@

clean:
	rm -rf vabslider
